// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_TEST_WEBDRIVER_WEBDRIVER_AUTOMATION_H_
#define CHROME_TEST_WEBDRIVER_WEBDRIVER_AUTOMATION_H_

#include <map>
#include <string>
#include <vector>

#include "base/command_line.h"
#include "base/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/task.h"
#include "chrome/common/automation_constants.h"
#include "chrome/test/webdriver/webdriver_logging.h"
#include "ui/base/keycodes/keyboard_codes.h"

class AutomationId;
class AutomationProxy;
class ProxyLauncher;
struct WebKeyEvent;
class WebViewId;
struct WebViewInfo;
class WebViewLocator;

namespace base {
class DictionaryValue;
class ListValue;
class Value;
}

namespace webdriver {

class Error;
class FramePath;
class Point;

// Creates and controls the Chrome instance.
// This class should be created and accessed on a single thread.
// Note: All member functions are void because they are invoked
// by posting a task.
class Automation {
 public:
  struct BrowserOptions {
    BrowserOptions();
    ~BrowserOptions();

    // The command line to use for launching the browser. If no program is
    // specified, the default browser executable will be used.
    CommandLine command;

    // The user data directory to be copied and used. If empty, a temporary
    // directory will be used.
    FilePath user_data_dir;

    // The channel ID of an already running browser to connect to. If empty,
    // the browser will be launched with an anonymous channel.
    std::string channel_id;

    // True if the Chrome process should only be terminated if quit is called.
    // If false, Chrome will also be terminated if this process is killed or
    // shutdown.
    bool detach_process;
  };

  explicit Automation(const Logger& logger);
  virtual ~Automation();

  // Start the system's default Chrome binary.
  void Init(const BrowserOptions& options, Error** error);

  // Terminates this session and disconnects its automation proxy. After
  // invoking this method, the Automation can safely be deleted.
  void Terminate();

  // Executes the given |script| in the specified frame of the current
  // tab. |result| will be set to the JSON result. Returns true on success.
  void ExecuteScript(const WebViewId& view_id,
                     const FramePath& frame_path,
                     const std::string& script,
                     std::string* result,
                     Error** error);

  // Sends a webkit key event to the current browser. Waits until the key has
  // been processed by the web page.
  void SendWebKeyEvent(const WebViewId& view_id,
                       const WebKeyEvent& key_event,
                       Error** error);

  // Sends an OS level key event to the current browser. Waits until the key
  // has been processed by the browser.
  void SendNativeKeyEvent(const WebViewId& view_id,
                          ui::KeyboardCode key_code,
                          int modifiers,
                          Error** error);

  // Drag and drop the file paths to the given location.
  void DragAndDropFilePaths(const WebViewId& view_id,
                            const Point& location,
                            const std::vector<FilePath::StringType>& paths,
                            Error** error);

  // Captures a snapshot of the tab to the specified path.  The  PNG will
  // contain the entire page, including what is not in the current view
  // on the  screen.
  void CaptureEntirePageAsPNG(
      const WebViewId& view_id, const FilePath& path, Error** error);

  void NavigateToURL(
      const WebViewId& view_id, const std::string& url, Error** error);
  void NavigateToURLAsync(
      const WebViewId& view_id, const std::string& url, Error** error);
  void GoForward(const WebViewId& view_id, Error** error);
  void GoBack(const WebViewId& view_id, Error** error);
  void Reload(const WebViewId& view_id, Error** error);

  void GetCookies(const std::string& url,
                  base::ListValue** cookies,
                  Error** error);
  void DeleteCookie(const std::string& url,
                    const std::string& cookie_name,
                    Error** error);
  void SetCookie(const std::string& url,
                 base::DictionaryValue* cookie_dict,
                 Error** error);

  void MouseMove(const WebViewId& view_id, const Point& p, Error** error);
  void MouseClick(const WebViewId& view_id,
                  const Point& p,
                  automation::MouseButton button,
                  Error** error);
  void MouseDrag(const WebViewId& view_id,
                 const Point& start,
                 const Point& end,
                 Error** error);
  void MouseButtonDown(const WebViewId& view_id,
                       const Point& p,
                       Error** error);
  void MouseButtonUp(const WebViewId& view_id,
                     const Point& p,
                     Error** error);
  void MouseDoubleClick(const WebViewId& view_id,
                        const Point& p,
                        Error** error);

  // Get info for all views currently open.
  void GetViews(std::vector<WebViewInfo>* views, Error** error);

  // Check if the given view exists currently.
  void DoesViewExist(const WebViewId& view_id, bool* does_exist, Error** error);

  // Closes the given view.
  void CloseView(const WebViewId& view_id, Error** error);

  // Gets the active JavaScript modal dialog's message.
  void GetAppModalDialogMessage(std::string* message, Error** error);

  // Accepts or dismisses the active JavaScript modal dialog.
  void AcceptOrDismissAppModalDialog(bool accept, Error** error);

  // Accepts an active prompt JavaScript modal dialog, using the given
  // prompt text as the result of the prompt.
  void AcceptPromptAppModalDialog(const std::string& prompt_text,
                                  Error** error);

  // Gets the version of the runing browser.
  void GetBrowserVersion(std::string* version);

  // Gets the ChromeDriver automation version supported by the automation
  // server.
  void GetChromeDriverAutomationVersion(int* version, Error** error);

  // Waits for all views to stop loading.
  void WaitForAllViewsToStopLoading(Error** error);

  // Install packed extension.
  void InstallExtensionDeprecated(const FilePath& path, Error** error);

  // Install a packed or unpacked extension. If the path ends with '.crx',
  // the extension is assumed to be packed.
  void InstallExtension(const FilePath& path, std::string* extension_id,
                        Error** error);

  // Gets a list of dictionary information about all installed extensions.
  void GetExtensionsInfo(base::ListValue* extensions_list, Error** error);

  // Gets a list of dictionary information about all installed extensions.
  void IsPageActionVisible(const WebViewId& tab_id,
                           const std::string& extension_id,
                           bool* is_visible,
                           Error** error);

  // Sets whether the extension is enabled or not.
  void SetExtensionState(const std::string& extension_id,
                         bool enable,
                         Error** error);

  // Clicks the extension action button. If |browser_action| is false, the
  // page action will be clicked.
  void ClickExtensionButton(const std::string& extension_id,
                            bool browser_action,
                            Error** error);

  // Uninstalls the given extension.
  void UninstallExtension(const std::string& extension_id, Error** error);

  // Set a local state preference, which is not associated with any profile.
  // Ownership of |value| is taken by this function.
  void SetLocalStatePreference(const std::string& pref,
                               base::Value* value,
                               Error** error);

  // Set a user preference, which is associated with the current profile.
  // Ownership of |value| is taken by this fucntion.
  void SetPreference(const std::string& pref,
                     base::Value* value,
                     Error** error);

 private:
  AutomationProxy* automation() const;
  Error* ConvertViewIdToLocator(const WebViewId& view_id,
                                WebViewLocator* view_locator);
  Error* CompareVersion(int client_build_no,
                        int client_patch_no,
                        bool* is_newer_or_equal);
  Error* CheckVersion(int client_build_no,
                      int client_patch_no,
                      const std::string& error_msg);
  Error* CheckAlertsSupported();
  Error* CheckAdvancedInteractionsSupported();
  Error* CheckNewExtensionInterfaceSupported();

  const Logger& logger_;
  scoped_ptr<ProxyLauncher> launcher_;

  DISALLOW_COPY_AND_ASSIGN(Automation);
};

}  // namespace webdriver

DISABLE_RUNNABLE_METHOD_REFCOUNT(webdriver::Automation);

#endif  // CHROME_TEST_WEBDRIVER_WEBDRIVER_AUTOMATION_H_
