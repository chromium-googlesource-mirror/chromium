// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/test/webdriver/webdriver_automation.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#include "base/base_paths.h"
#include "base/basictypes.h"
#include "base/callback.h"
#include "base/environment.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/json/json_writer.h"
#include "base/memory/ref_counted.h"
#include "base/path_service.h"
#include "base/string_number_conversions.h"
#include "base/string_split.h"
#include "base/stringprintf.h"
#include "base/synchronization/waitable_event.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "chrome/common/automation_constants.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/url_constants.h"
#include "chrome/test/automation/automation_json_requests.h"
#include "chrome/test/automation/automation_proxy.h"
#include "chrome/test/automation/browser_proxy.h"
#include "chrome/test/automation/extension_proxy.h"
#include "chrome/test/automation/proxy_launcher.h"
#include "chrome/test/automation/tab_proxy.h"
#include "chrome/test/base/chrome_process_util.h"
#include "chrome/test/webdriver/frame_path.h"
#include "chrome/test/webdriver/webdriver_basic_types.h"
#include "chrome/test/webdriver/webdriver_error.h"
#include "chrome/test/webdriver/webdriver_util.h"

#if defined(OS_WIN)
#include "base/win/registry.h"
#include "base/win/windows_version.h"
#endif

namespace {

// Iterates through each browser executable path, and checks if the path exists
// in any of the given locations. If found, returns true and sets |browser_exe|.
bool CheckForChromeExe(const std::vector<FilePath>& browser_exes,
                       const std::vector<FilePath>& locations,
                       FilePath* browser_exe) {
  for (size_t i = 0; i < browser_exes.size(); ++i) {
    for (size_t j = 0; j < locations.size(); ++j) {
      FilePath path = locations[j].Append(browser_exes[i]);
      if (file_util::PathExists(path)) {
        *browser_exe = path;
        return true;
      }
    }
  }
  return false;
}

// Gets the path to the default Chrome executable. Returns true on success.
bool GetDefaultChromeExe(FilePath* browser_exe) {
  // Instead of using chrome constants, we hardcode these constants here so
  // that we can locate chrome or chromium regardless of the branding
  // chromedriver is built with. It may be argued that then we need to keep
  // these in sync with chrome constants. However, if chrome constants changes,
  // we need to look for the previous and new versions to support some
  // backwards compatibility.
#if defined(OS_WIN)
  FilePath browser_exes_array[] = {
      FilePath(L"chrome.exe")
  };
#elif defined(OS_MACOSX)
  FilePath browser_exes_array[] = {
      FilePath("Google Chrome.app/Contents/MacOS/Google Chrome"),
      FilePath("Chromium.app/Contents/MacOS/Chromium")
  };
#elif defined(OS_LINUX)
  FilePath browser_exes_array[] = {
      FilePath("google-chrome"),
      FilePath("chrome"),
      FilePath("chromium"),
      FilePath("chromium-browser")
  };
#endif
  std::vector<FilePath> browser_exes(
      browser_exes_array, browser_exes_array + arraysize(browser_exes_array));

  // Step 1: Check the directory this module resides in. This is done
  // before all else so that the tests will pickup the built chrome.
  FilePath module_dir;
  if (PathService::Get(base::DIR_MODULE, &module_dir)) {
    for (size_t j = 0; j < browser_exes.size(); ++j) {
      FilePath path = module_dir.Append(browser_exes[j]);
      if (file_util::PathExists(path)) {
        *browser_exe = path;
        return true;
      }
    }
  }

  // Step 2: Add all possible install locations, in order they should be
  // searched. If a location can only hold a chromium install, add it to
  // |chromium_locations|. Since on some platforms we cannot tell by the binary
  // name whether it is chrome or chromium, we search these locations last.
  // We attempt to run chrome before chromium, if any install can be found.
  std::vector<FilePath> locations;
  std::vector<FilePath> chromium_locations;
#if defined(OS_WIN)
  // Add the App Paths registry key location.
  const wchar_t kSubKey[] =
      L"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe";
  base::win::RegKey key(HKEY_CURRENT_USER, kSubKey, KEY_READ);
  std::wstring path;
  if (key.ReadValue(L"path", &path) == ERROR_SUCCESS)
    locations.push_back(FilePath(path));
  base::win::RegKey sys_key(HKEY_LOCAL_MACHINE, kSubKey, KEY_READ);
  if (sys_key.ReadValue(L"path", &path) == ERROR_SUCCESS)
    locations.push_back(FilePath(path));

  // Add the user-level location for Chrome.
  FilePath app_from_google(L"Google\\Chrome\\Application");
  FilePath app_from_chromium(L"Chromium\\Application");
  scoped_ptr<base::Environment> env(base::Environment::Create());
  std::string home_dir;
  if (env->GetVar("userprofile", &home_dir)) {
    FilePath default_location(UTF8ToWide(home_dir));
    if (base::win::GetVersion() < base::win::VERSION_VISTA) {
      default_location = default_location.Append(
          L"Local Settings\\Application Data");
    } else {
      default_location = default_location.Append(L"AppData\\Local");
    }
    locations.push_back(default_location.Append(app_from_google));
    chromium_locations.push_back(default_location.Append(app_from_chromium));
  }

  // Add the system-level location for Chrome.
  std::string program_dir;
  if (env->GetVar("ProgramFiles", &program_dir)) {
    locations.push_back(FilePath(UTF8ToWide(program_dir))
        .Append(app_from_google));
    chromium_locations.push_back(FilePath(UTF8ToWide(program_dir))
        .Append(app_from_chromium));
  }
  if (env->GetVar("ProgramFiles(x86)", &program_dir)) {
    locations.push_back(FilePath(UTF8ToWide(program_dir))
        .Append(app_from_google));
    chromium_locations.push_back(FilePath(UTF8ToWide(program_dir))
        .Append(app_from_chromium));
  }
#elif defined(OS_MACOSX)
  std::vector<FilePath> app_dirs;
  webdriver::GetApplicationDirs(&app_dirs);
  locations.insert(locations.end(), app_dirs.begin(), app_dirs.end());
#elif defined(OS_LINUX)
  locations.push_back(FilePath("/opt/google/chrome"));
  locations.push_back(FilePath("/usr/local/bin"));
  locations.push_back(FilePath("/usr/local/sbin"));
  locations.push_back(FilePath("/usr/bin"));
  locations.push_back(FilePath("/usr/sbin"));
  locations.push_back(FilePath("/bin"));
  locations.push_back(FilePath("/sbin"));
#endif

  // Add the current directory.
  FilePath current_dir;
  if (file_util::GetCurrentDirectory(&current_dir))
    locations.push_back(current_dir);

  // Step 3: For each browser exe path, check each location to see if the
  // browser is installed there. Check the chromium locations lastly.
  return CheckForChromeExe(browser_exes, locations, browser_exe) ||
      CheckForChromeExe(browser_exes, chromium_locations, browser_exe);
}

}  // namespace

namespace webdriver {

Automation::BrowserOptions::BrowserOptions()
    : command(CommandLine::NO_PROGRAM),
      detach_process(false) {}

Automation::BrowserOptions::~BrowserOptions() {}

Automation::Automation(const Logger& logger) : logger_(logger) {}

Automation::~Automation() {}

void Automation::Init(const BrowserOptions& options, Error** error) {
  // Prepare Chrome's command line.
  CommandLine command(CommandLine::NO_PROGRAM);
  command.AppendSwitch(switches::kDisableHangMonitor);
  command.AppendSwitch(switches::kDisablePromptOnRepost);
  command.AppendSwitch(switches::kDomAutomationController);
  command.AppendSwitch(switches::kFullMemoryCrashReport);
  command.AppendSwitch(switches::kNoDefaultBrowserCheck);
  command.AppendSwitch(switches::kNoFirstRun);
  if (options.detach_process)
    command.AppendSwitch(switches::kAutomationReinitializeOnChannelError);
  if (options.user_data_dir.empty())
    command.AppendSwitchASCII(switches::kHomePage, chrome::kAboutBlankURL);

  command.AppendArguments(options.command, true /* include_program */);

  // Find the Chrome binary.
  if (command.GetProgram().empty()) {
    FilePath browser_exe;
    if (!GetDefaultChromeExe(&browser_exe)) {
      *error = new Error(kUnknownError, "Could not find default Chrome binary");
      return;
    }
    command.SetProgram(browser_exe);
  }
  if (!file_util::PathExists(command.GetProgram())) {
    std::string message = base::StringPrintf(
        "Could not find Chrome binary at: %" PRFilePath,
        command.GetProgram().value().c_str());
    *error = new Error(kUnknownError, message);
    return;
  }
  std::string chrome_details = base::StringPrintf(
      "Using Chrome binary at: %" PRFilePath,
      command.GetProgram().value().c_str());

  // Create the ProxyLauncher and launch Chrome.
  // In Chrome 13/14, the only way to detach the browser process is to use a
  // named proxy launcher.
  // TODO(kkania): Remove this when Chrome 15 is stable.
  std::string channel_id = options.channel_id;
  bool launch_browser = false;
  if (options.detach_process) {
    launch_browser = true;
    if (!channel_id.empty()) {
      *error = new Error(
          kUnknownError, "Cannot detach an already running browser process");
      return;
    }
#if defined(OS_WIN)
    channel_id = "chromedriver" + GenerateRandomID();
#elif defined(OS_POSIX)
    FilePath temp_file;
    if (!file_util::CreateTemporaryFile(&temp_file) ||
        !file_util::Delete(temp_file, false /* recursive */)) {
      *error = new Error(kUnknownError, "Could not create temporary filename");
      return;
    }
    channel_id = temp_file.value();
#endif
  }
  if (channel_id.empty()) {
    std::string command_line_str;
#if defined(OS_WIN)
    command_line_str = WideToUTF8(command.GetCommandLineString());
#elif defined(OS_POSIX)
    command_line_str = command.GetCommandLineString();
#endif
    logger_.Log(kInfoLogLevel, "Launching chrome: " + command_line_str);
    launcher_.reset(new AnonymousProxyLauncher(false));
  } else {
    logger_.Log(kInfoLogLevel, "Using named testing interface");
    launcher_.reset(new NamedProxyLauncher(channel_id, launch_browser, false));
  }
  ProxyLauncher::LaunchState launch_props = {
      false,  // clear_profile
      options.user_data_dir,  // template_user_data
      base::Closure(),
      command,
      true,  // include_testing_id
      true   // show_window
  };
  if (!launcher_->InitializeConnection(launch_props, true)) {
    logger_.Log(kSevereLogLevel, "Failed to initialize connection");
    *error = new Error(
        kUnknownError,
        "Unable to either launch or connect to Chrome. Please check that "
            "ChromeDriver is up-to-date. " + chrome_details);
    return;
  }

  launcher_->automation()->set_action_timeout_ms(base::kNoTimeout);
  logger_.Log(kInfoLogLevel, "Connected to Chrome successfully. Version: " +
                  automation()->server_version());

  // Check the version of Chrome is compatible with this ChromeDriver.
  chrome_details += ", version (" + automation()->server_version() + ")";
  int version = 0;
  automation::Error auto_error;
  if (!SendGetChromeDriverAutomationVersion(
          automation(), &version, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
    return;
  }
  if (version > automation::kChromeDriverAutomationVersion) {
    *error = new Error(
        kUnknownError,
        "ChromeDriver is not compatible with this version of Chrome. " +
            chrome_details);
    return;
  }
}

void Automation::Terminate() {
  if (launcher_.get() && launcher_->process() != base::kNullProcessHandle) {
#if defined(OS_MACOSX)
    // There's currently no way to shutdown gracefully with mac chrome.
    // An alert could be open or (open before shutdown is started) and stop
    // the whole process. Close any current dialog, try to kill gently, and
    // if all else fails, kill it hard.
    Error* error = NULL;
    AcceptOrDismissAppModalDialog(true /* accept */, &error);
    scoped_ptr<Error> scoped_error(error);

    kill(launcher_->process(), SIGTERM);
    int exit_code = -1;
    if (!launcher_->WaitForBrowserProcessToQuit(10000, &exit_code)) {
      TerminateAllChromeProcesses(launcher_->process_id());
    }
    base::CloseProcessHandle(launcher_->process());
#else
    launcher_->TerminateBrowser();
#endif
  }
}

void Automation::ExecuteScript(const WebViewId& view_id,
                               const FramePath& frame_path,
                               const std::string& script,
                               std::string* result,
                               Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  Value* unscoped_value;
  automation::Error auto_error;
  if (!SendExecuteJavascriptJSONRequest(automation(), view_locator,
                                        frame_path.value(), script,
                                        &unscoped_value, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
    return;
  }
  scoped_ptr<Value> value(unscoped_value);
  if (!value->GetAsString(result))
    *error = new Error(kUnknownError, "Execute script did not return string");
}

void Automation::MouseMove(const WebViewId& view_id,
                           const Point& p,
                           Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendMouseMoveJSONRequest(
          automation(), view_locator, p.rounded_x(), p.rounded_y(),
          &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::MouseClick(const WebViewId& view_id,
                            const Point& p,
                            automation::MouseButton button,
                            Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendMouseClickJSONRequest(
          automation(), view_locator, button, p.rounded_x(),
          p.rounded_y(), &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::MouseDrag(const WebViewId& view_id,
                           const Point& start,
                           const Point& end,
                           Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendMouseDragJSONRequest(
          automation(), view_locator, start.rounded_x(), start.rounded_y(),
          end.rounded_x(), end.rounded_y(), &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::MouseButtonUp(const WebViewId& view_id,
                               const Point& p,
                               Error** error) {
  *error = CheckAdvancedInteractionsSupported();
  if (*error)
    return;

  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendMouseButtonUpJSONRequest(
          automation(), view_locator, p.rounded_x(), p.rounded_y(),
          &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::MouseButtonDown(const WebViewId& view_id,
                                 const Point& p,
                                 Error** error) {
  *error = CheckAdvancedInteractionsSupported();
  if (*error)
    return;

  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendMouseButtonDownJSONRequest(
          automation(), view_locator, p.rounded_x(), p.rounded_y(),
          &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::MouseDoubleClick(const WebViewId& view_id,
                                  const Point& p,
                                  Error** error) {
  *error = CheckAdvancedInteractionsSupported();
  if (*error)
    return;

  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendMouseDoubleClickJSONRequest(
          automation(), view_locator, p.rounded_x(), p.rounded_y(),
          &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::DragAndDropFilePaths(
    const WebViewId& view_id, const Point& location,
    const std::vector<FilePath::StringType>& paths, Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error) {
    return;
  }

  automation::Error auto_error;
  if (!SendDragAndDropFilePathsJSONRequest(
          automation(), view_locator, location.rounded_x(),
          location.rounded_y(), paths, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::SendWebKeyEvent(const WebViewId& view_id,
                                 const WebKeyEvent& key_event,
                                 Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendWebKeyEventJSONRequest(
          automation(), view_locator, key_event, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::SendNativeKeyEvent(const WebViewId& view_id,
                                    ui::KeyboardCode key_code,
                                    int modifiers,
                                    Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendNativeKeyEventJSONRequest(
         automation(), view_locator, key_code, modifiers, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::CaptureEntirePageAsPNG(const WebViewId& view_id,
                                        const FilePath& path,
                                        Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendCaptureEntirePageJSONRequest(
          automation(), view_locator, path, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::NavigateToURL(const WebViewId& view_id,
                               const std::string& url,
                               Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  AutomationMsg_NavigationResponseValues navigate_response;
  automation::Error auto_error;
  if (!SendNavigateToURLJSONRequest(automation(), view_locator,
                                    url, 1, &navigate_response,
                                    &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
    return;
  }
  // TODO(kkania): Do not rely on this enum.
  if (navigate_response == AUTOMATION_MSG_NAVIGATION_ERROR)
    *error = new Error(kUnknownError, "Navigation error occurred");
}

void Automation::NavigateToURLAsync(const WebViewId& view_id,
                                    const std::string& url,
                                    Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!view_id.old_style()) {
    AutomationMsg_NavigationResponseValues navigate_response;
    if (!SendNavigateToURLJSONRequest(automation(), view_locator, url,
                                      0, &navigate_response, &auto_error)) {
      *error = Error::FromAutomationError(auto_error);
      return;
    }
  } else {
    scoped_refptr<BrowserProxy> browser =
        automation()->GetBrowserWindow(view_locator.browser_index());
    if (!browser) {
      *error = new Error(kUnknownError, "Couldn't obtain browser proxy");
      return;
    }
    scoped_refptr<TabProxy> tab = browser->GetTab(view_locator.tab_index());
    if (!tab) {
      *error = new Error(kUnknownError, "Couldn't obtain tab proxy");
      return;
    }
    if (!tab->NavigateToURLAsync(GURL(url))) {
      *error = new Error(kUnknownError, "Unable to navigate to url");
      return;
    }
  }
}

void Automation::GoForward(const WebViewId& view_id, Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendGoForwardJSONRequest(
          automation(), view_locator, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::GoBack(const WebViewId& view_id, Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendGoBackJSONRequest(automation(), view_locator, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::Reload(const WebViewId& view_id, Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendReloadJSONRequest(automation(), view_locator, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::GetCookies(const std::string& url,
                            ListValue** cookies,
                            Error** error) {
  automation::Error auto_error;
  if (!SendGetCookiesJSONRequest(automation(), url, cookies, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::DeleteCookie(const std::string& url,
                              const std::string& cookie_name,
                              Error** error) {
  automation::Error auto_error;
  if (!SendDeleteCookieJSONRequest(
          automation(), url, cookie_name, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::SetCookie(const std::string& url,
                           DictionaryValue* cookie_dict,
                           Error** error) {
  automation::Error auto_error;
  if (!SendSetCookieJSONRequest(automation(), url, cookie_dict, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::GetViews(std::vector<WebViewInfo>* views,
                          Error** error) {
  bool has_views = false;
  *error = CompareVersion(963, 0, &has_views);
  if (*error)
    return;

  automation::Error auto_error;
  if (has_views) {
    if (!SendGetWebViewsJSONRequest(automation(), views, &auto_error))
      *error = Error::FromAutomationError(auto_error);
  } else {
    if (!SendGetTabIdsJSONRequest(automation(), views, &auto_error))
      *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::DoesViewExist(
    const WebViewId& view_id, bool* does_exist, Error** error) {
  automation::Error auto_error;
  if (view_id.old_style()) {
    if (!SendIsTabIdValidJSONRequest(
            automation(), view_id, does_exist, &auto_error))
      *error = Error::FromAutomationError(auto_error);
  } else {
    if (!SendDoesAutomationObjectExistJSONRequest(
            automation(), view_id, does_exist, &auto_error))
      *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::CloseView(const WebViewId& view_id, Error** error) {
  WebViewLocator view_locator;
  *error = ConvertViewIdToLocator(view_id, &view_locator);
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendCloseViewJSONRequest(automation(), view_locator, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::GetAppModalDialogMessage(std::string* message, Error** error) {
  *error = CheckAlertsSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendGetAppModalDialogMessageJSONRequest(
          automation(), message, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::AcceptOrDismissAppModalDialog(bool accept, Error** error) {
  *error = CheckAlertsSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendAcceptOrDismissAppModalDialogJSONRequest(
          automation(), accept, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::AcceptPromptAppModalDialog(const std::string& prompt_text,
                                            Error** error) {
  *error = CheckAlertsSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendAcceptPromptAppModalDialogJSONRequest(
          automation(), prompt_text, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }
}

void Automation::GetBrowserVersion(std::string* version) {
  *version = automation()->server_version();
}

void Automation::GetChromeDriverAutomationVersion(int* version, Error** error) {
  automation::Error auto_error;
  if (!SendGetChromeDriverAutomationVersion(automation(), version, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::WaitForAllViewsToStopLoading(Error** error) {
  automation::Error auto_error;
  if (!SendWaitForAllViewsToStopLoadingJSONRequest(automation(), &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::InstallExtensionDeprecated(
    const FilePath& path, Error** error) {
  if (!launcher_->automation()->InstallExtension(path, false).get())
    *error = new Error(kUnknownError, "Failed to install extension");
}

void Automation::InstallExtension(
    const FilePath& path, std::string* extension_id, Error** error) {
  *error = CheckNewExtensionInterfaceSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendInstallExtensionJSONRequest(
          automation(), path, false /* with_ui */, extension_id,
          &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::GetExtensionsInfo(
    base::ListValue* extensions_list, Error** error) {
  *error = CheckNewExtensionInterfaceSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendGetExtensionsInfoJSONRequest(
          automation(), extensions_list, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::IsPageActionVisible(
    const WebViewId& tab_id,
    const std::string& extension_id,
    bool* is_visible,
    Error** error) {
  *error = CheckNewExtensionInterfaceSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendIsPageActionVisibleJSONRequest(
          automation(), tab_id, extension_id, is_visible, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::SetExtensionState(
    const std::string& extension_id,
    bool enable,
    Error** error) {
  *error = CheckNewExtensionInterfaceSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendSetExtensionStateJSONRequest(
          automation(), extension_id, enable /* enable */,
          false /* allow_in_incognito */, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::ClickExtensionButton(
    const std::string& extension_id,
    bool browser_action,
    Error** error) {
  automation::Error auto_error;
  if (!SendClickExtensionButtonJSONRequest(
          automation(), extension_id, browser_action, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::UninstallExtension(
    const std::string& extension_id, Error** error) {
  *error = CheckNewExtensionInterfaceSupported();
  if (*error)
    return;

  automation::Error auto_error;
  if (!SendUninstallExtensionJSONRequest(
          automation(), extension_id, &auto_error))
    *error = Error::FromAutomationError(auto_error);
}

void Automation::SetLocalStatePreference(const std::string& pref,
                                         base::Value* value,
                                         Error** error) {
  scoped_ptr<Value> scoped_value(value);
  bool has_new_local_state_api = false;
  // In version 927, SetLocalStatePrefs was changed from taking a browser
  // handle to a browser index.
  *error = CompareVersion(927, 0, &has_new_local_state_api);
  if (*error)
    return;

  if (has_new_local_state_api) {
    automation::Error auto_error;
    if (!SendSetLocalStatePreferenceJSONRequest(
            automation(), pref, scoped_value.release(), &auto_error))
      *error = Error::FromAutomationError(auto_error);
  } else {
    std::string request, response;
    DictionaryValue request_dict;
    request_dict.SetString("command", "SetLocalStatePrefs");
    request_dict.SetString("path", pref);
    request_dict.Set("value", scoped_value.release());
    base::JSONWriter::Write(&request_dict, false, &request);
    if (!automation()->GetBrowserWindow(0)->SendJSONRequest(
            request, -1, &response)) {
      *error = new Error(kUnknownError, base::StringPrintf(
          "Failed to set local state pref '%s'", pref.c_str()));
    }
  }
}

void Automation::SetPreference(const std::string& pref,
                               base::Value* value,
                               Error** error) {
  scoped_ptr<Value> scoped_value(value);
  bool has_new_pref_api = false;
  // Chrome 17 is on the 963 branch. The first released 18 build should have
  // the new SetPrefs method which uses a browser index instead of handle.
  *error = CompareVersion(964, 0, &has_new_pref_api);
  if (*error)
    return;

  if (has_new_pref_api) {
    automation::Error auto_error;
    if (!SendSetPreferenceJSONRequest(
            automation(), pref, scoped_value.release(), &auto_error))
      *error = Error::FromAutomationError(auto_error);
  } else {
    std::string request, response;
    DictionaryValue request_dict;
    request_dict.SetString("command", "SetPrefs");
    request_dict.SetInteger("windex", 0);
    request_dict.SetString("path", pref);
    request_dict.Set("value", scoped_value.release());
    base::JSONWriter::Write(&request_dict, false, &request);
    if (!automation()->GetBrowserWindow(0)->SendJSONRequest(
            request, -1, &response)) {
      *error = new Error(kUnknownError, base::StringPrintf(
          "Failed to set pref '%s'", pref.c_str()));
    }
  }
}

AutomationProxy* Automation::automation() const {
  return launcher_->automation();
}

Error* Automation::ConvertViewIdToLocator(
    const WebViewId& view_id, WebViewLocator* view_locator) {
  if (view_id.old_style()) {
    int browser_index, tab_index;
    automation::Error auto_error;
    if (!SendGetIndicesFromTabIdJSONRequest(
            automation(), view_id.tab_id(), &browser_index, &tab_index,
            &auto_error))
      return Error::FromAutomationError(auto_error);
    *view_locator = WebViewLocator::ForIndexPair(browser_index, tab_index);
  } else {
    *view_locator = WebViewLocator::ForViewId(view_id.GetId());
  }
  return NULL;
}

Error* Automation::CompareVersion(int client_build_no,
                                  int client_patch_no,
                                  bool* is_newer_or_equal) {
  std::string version = automation()->server_version();
  std::vector<std::string> split_version;
  base::SplitString(version, '.', &split_version);
  if (split_version.size() != 4) {
    return new Error(
        kUnknownError, "Browser version has unrecognized format: " + version);
  }
  int build_no, patch_no;
  if (!base::StringToInt(split_version[2], &build_no) ||
      !base::StringToInt(split_version[3], &patch_no)) {
    return new Error(
        kUnknownError, "Browser version has unrecognized format: " + version);
  }
  if (build_no < client_build_no)
    *is_newer_or_equal = false;
  else if (build_no > client_build_no)
    *is_newer_or_equal = true;
  else
    *is_newer_or_equal = patch_no >= client_patch_no;
  return NULL;
}

Error* Automation::CheckVersion(int client_build_no,
                                int client_patch_no,
                                const std::string& error_msg) {
  bool version_is_ok = false;
  Error* error = CompareVersion(
      client_build_no, client_patch_no, &version_is_ok);
  if (error)
    return error;
  if (!version_is_ok)
    return new Error(kUnknownError, error_msg);
  return NULL;
}

Error* Automation::CheckAlertsSupported() {
  return CheckVersion(
      768, 0, "Alerts are not supported for this version of Chrome");
}

Error* Automation::CheckAdvancedInteractionsSupported() {
  const char* message =
      "Advanced user interactions are not supported for this version of Chrome";
  return CheckVersion(750, 0, message);
}

Error* Automation::CheckNewExtensionInterfaceSupported() {
  const char* message =
      "Extension interface is not supported for this version of Chrome";
  return CheckVersion(947, 0, message);
}

}  // namespace webdriver
