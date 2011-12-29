// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome_frame/test/net/fake_external_tab.h"

#include <atlbase.h>
#include <atlcom.h>
#include <exdisp.h>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/debug/debugger.h"
#include "base/file_util.h"
#include "base/file_version_info.h"
#include "base/i18n/icu_util.h"
#include "base/lazy_instance.h"
#include "base/memory/scoped_ptr.h"
#include "base/path_service.h"
#include "base/scoped_temp_dir.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/system_monitor/system_monitor.h"
#include "base/test/test_timeouts.h"
#include "base/threading/platform_thread.h"
#include "base/threading/thread.h"
#include "base/win/scoped_com_initializer.h"
#include "base/win/scoped_comptr.h"
#include "base/win/scoped_handle.h"
#include "chrome/app/chrome_main_delegate.h"
#include "chrome/browser/automation/automation_provider_list.h"
#include "chrome/browser/chrome_content_browser_client.h"
#include "chrome/browser/prefs/browser_prefs.h"
#include "chrome/browser/prefs/pref_service.h"
#include "chrome/browser/prefs/proxy_config_dictionary.h"
#include "chrome/browser/process_singleton.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/renderer_host/web_cache_manager.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_content_client.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_paths_internal.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/pref_names.h"
#include "chrome/renderer/chrome_content_renderer_client.h"
#include "chrome_frame/crash_server_init.h"
#include "chrome_frame/test/chrome_frame_test_utils.h"
#include "chrome_frame/test/net/test_automation_resource_message_filter.h"
#include "chrome_frame/test/simulate_input.h"
#include "chrome_frame/test/win_event_receiver.h"
#include "chrome_frame/utils.h"
#include "content/app/content_main.h"
#include "content/public/app/startup_helper_win.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_paths.h"
#include "sandbox/src/sandbox_types.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"

using content::BrowserThread;

namespace {

// We must store this globally so that our main delegate can set it.
static CFUrlRequestUnittestRunner* g_test_suite = NULL;

// Copied here for access by CreateBrowserMainParts and InitGoogleTest.
static int g_argc = 0;
static char** g_argv = NULL;

// A special command line switch to allow developers to manually launch the
// browser and debug CF inside the browser.
const char kManualBrowserLaunch[] = "manual-browser";

// Pops up a message box after the test environment has been set up
// and before tearing it down.  Useful for when debugging tests and not
// the test environment that's been set up.
const char kPromptAfterSetup[] = "prompt-after-setup";

const int kTestServerPort = 4666;
// The test HTML we use to initialize Chrome Frame.
// Note that there's a little trick in there to avoid an extra URL request
// that the browser will otherwise make for the site's favicon.
// If we don't do this the browser will create a new URL request after
// the CF page has been initialized and that URL request will confuse the
// global URL instance counter in the unit tests and subsequently trip
// some DCHECKs.
const char kChromeFrameHtml[] = "<html><head>"
    "<meta http-equiv=\"X-UA-Compatible\" content=\"chrome=1\" />"
    "<link rel=\"shortcut icon\" href=\"file://c:\\favicon.ico\"/>"
    "</head><body>Chrome Frame should now be loaded</body></html>";

bool ShouldLaunchBrowser() {
  return !CommandLine::ForCurrentProcess()->HasSwitch(kManualBrowserLaunch);
}

bool PromptAfterSetup() {
  return CommandLine::ForCurrentProcess()->HasSwitch(kPromptAfterSetup);
}

// Uses the IAccessible interface for the window to set the focus.
// This can be useful when you don't have control over the thread that
// owns the window.
// NOTE: this depends on oleacc.lib which the net tests already depend on
// but other unit tests don't depend on oleacc so we can't just add the method
// directly into chrome_frame_test_utils.cc (without adding a
// #pragma comment(lib, "oleacc.lib")).
bool SetFocusToAccessibleWindow(HWND hwnd) {
  bool ret = false;
  base::win::ScopedComPtr<IAccessible> acc;
  AccessibleObjectFromWindow(hwnd, OBJID_WINDOW, IID_IAccessible,
      reinterpret_cast<void**>(acc.Receive()));
  if (acc) {
    VARIANT self = { VT_I4 };
    self.lVal = CHILDID_SELF;
    ret = SUCCEEDED(acc->accSelect(SELFLAG_TAKEFOCUS, self));
  }
  return ret;
}

class FakeContentBrowserClient : public chrome::ChromeContentBrowserClient {
 public:
  virtual ~FakeContentBrowserClient() {}

  virtual content::BrowserMainParts* CreateBrowserMainParts(
      const content::MainFunctionParams& parameters) OVERRIDE;
};

base::LazyInstance<chrome::ChromeContentClient>
    g_chrome_content_client = LAZY_INSTANCE_INITIALIZER;

// Override the default ContentBrowserClient to let Chrome participate in
// content logic.  Must be done before any tabs are created.
base::LazyInstance<FakeContentBrowserClient>
    g_browser_client = LAZY_INSTANCE_INITIALIZER;

base::LazyInstance<chrome::ChromeContentRendererClient>
    g_renderer_client = LAZY_INSTANCE_INITIALIZER;

class FakeMainDelegate : public content::ContentMainDelegate {
 public:
  virtual ~FakeMainDelegate() {}

  virtual bool BasicStartupComplete(int* exit_code) OVERRIDE {
    return false;
  }

  virtual void PreSandboxStartup() OVERRIDE {
    // Initialize the content client.
    content::SetContentClient(&g_chrome_content_client.Get());

    // Override the default ContentBrowserClient to let Chrome participate in
    // content logic.  We use a subclass of Chrome's implementation,
    // FakeContentBrowserClient, to override CreateBrowserMainParts.  Must
    // be done before any tabs are created.
    content::GetContentClient()->set_browser(&g_browser_client.Get());

    content::GetContentClient()->set_renderer(&g_renderer_client.Get());
  }

  virtual void SandboxInitialized(const std::string& process_type) OVERRIDE {}

  virtual int RunProcess(
      const std::string& process_type,
      const content::MainFunctionParams& main_function_params) OVERRIDE {
    return -1;
  }
  virtual void ProcessExiting(const std::string& process_type) OVERRIDE {}
};

void FilterDisabledTests() {
  if (::testing::FLAGS_gtest_filter.length() &&
      ::testing::FLAGS_gtest_filter.Compare("*") != 0) {
    // Don't override user specified filters.
    return;
  }

  const char* disabled_tests[] = {
    // Tests disabled since they're testing the same functionality used
    // by the TestAutomationProvider.
    "URLRequestTest.Intercept",
    "URLRequestTest.InterceptNetworkError",
    "URLRequestTest.InterceptRestartRequired",
    "URLRequestTest.InterceptRespectsCancelMain",
    "URLRequestTest.InterceptRespectsCancelRedirect",
    "URLRequestTest.InterceptRespectsCancelFinal",
    "URLRequestTest.InterceptRespectsCancelInRestart",
    "URLRequestTest.InterceptRedirect",
    "URLRequestTest.InterceptServerError",
    "URLRequestTestFTP.*",

    // Tests that are currently not working:

    // Temporarily disabled because they needs user input (login dialog).
    "URLRequestTestHTTP.BasicAuth",
    "URLRequestTestHTTP.BasicAuthWithCookies",

    // HTTPS tests temporarily disabled due to the certificate error dialog.
    // TODO(tommi): The tests currently fail though, so need to fix.
    "HTTPSRequestTest.HTTPSMismatchedTest",
    "HTTPSRequestTest.HTTPSExpiredTest",
    "HTTPSRequestTest.ClientAuthTest",

    // Tests chrome's network stack's cache (might not apply to CF).
    "URLRequestTestHTTP.VaryHeader",
    "URLRequestTestHTTP.GetZippedTest",

    // I suspect we can only get this one to work (if at all) on IE8 and
    // later by using the new INTERNET_OPTION_SUPPRESS_BEHAVIOR flags
    // See http://msdn.microsoft.com/en-us/library/aa385328(VS.85).aspx
    "URLRequestTest.DoNotSaveCookies",
    "URLRequestTest.DelayedCookieCallback",

    // TODO(ananta): This test has been consistently failing. Disabling it for
    // now.
    "URLRequestTestHTTP.GetTest_NoCache",

    // These tests have been disabled as the Chrome cookie policies don't make
    // sense or have not been implemented for the host network stack.
    "URLRequestTest.DoNotSaveCookies_ViaPolicy",
    "URLRequestTest.DoNotSendCookies_ViaPolicy",
    "URLRequestTest.DoNotSaveCookies_ViaPolicy_Async",
    "URLRequestTest.CookiePolicy_ForceSession",
    "URLRequestTest.DoNotSendCookies",
    "URLRequestTest.DoNotSendCookies_ViaPolicy_Async",
    "URLRequestTest.CancelTest_During_OnGetCookies",
    "URLRequestTest.CancelTest_During_OnSetCookie",

    // These tests are disabled as the rely on functionality provided by
    // Chrome's HTTP stack like the ability to set the proxy for a URL, etc.
    "URLRequestTestHTTP.ProxyTunnelRedirectTest",
    "URLRequestTestHTTP.UnexpectedServerAuthTest",

    // This test is disabled as it expects an empty UA to be echoed back from
    // the server which is not the case in ChromeFrame.
    "URLRequestTestHTTP.DefaultUserAgent",
    // This test modifies the UploadData object after it has been marshaled to
    // ChromeFrame. We don't support this.
    "URLRequestTestHTTP.TestPostChunkedDataAfterStart",

    // Do not work in CF, it may well be that IE is unconditionally
    // adding Accept-Encoding header by default to outgoing requests.
    "URLRequestTestHTTP.DefaultAcceptEncoding",
    "URLRequestTestHTTP.OverrideAcceptEncoding",

    // Not supported in ChromeFrame as we use IE's network stack.
    "URLRequestTest.NetworkDelegateProxyError",

    // URLRequestAutomationJob needs to support NeedsAuth.
    // http://crbug.com/98446
    "URLRequestTestHTTP.NetworkDelegateOnAuthRequiredSyncNoAction",
    "URLRequestTestHTTP.NetworkDelegateOnAuthRequiredSyncSetAuth",
    "URLRequestTestHTTP.NetworkDelegateOnAuthRequiredSyncCancel",
    "URLRequestTestHTTP.NetworkDelegateOnAuthRequiredAsyncNoAction",
    "URLRequestTestHTTP.NetworkDelegateOnAuthRequiredAsyncSetAuth",
    "URLRequestTestHTTP.NetworkDelegateOnAuthRequiredAsyncCancel",

    // Flaky on the tryservers, http://crbug.com/103097
    "URLRequestTestHTTP.MultipleRedirectTest",
    "URLRequestTestHTTP.NetworkDelegateRedirectRequest",

    // These tests are unsupported in CF.
    "HTTPSRequestTest.HTTPSPreloadedHSTSTest",
    "HTTPSRequestTest.ResumeTest",
    "HTTPSRequestTest.SSLSessionCacheShardTest",
  };

  const char* ie9_disabled_tests[] = {
    // These always hang on Joi's box with IE9, http://crbug.com/105435.
    // Several other tests, e.g. URLRequestTestHTTP.CancelTest2, 3 and
    // 5, often hang but not always.
    "URLRequestTestHTTP.NetworkDelegateRedirectRequestPost",
    "URLRequestTestHTTP.GetTest",
    "HTTPSRequestTest.HTTPSPreloadedHSTSTest",
  };

  std::string filter("-");  // All following filters will be negative.
  for (int i = 0; i < arraysize(disabled_tests); ++i) {
    if (i > 0)
      filter += ":";
    filter += disabled_tests[i];
  }

  if (chrome_frame_test::GetInstalledIEVersion() >= IE_9) {
    for (int i = 0; i < arraysize(ie9_disabled_tests); ++i) {
      filter += ":";
      filter += ie9_disabled_tests[i];
    }
  }

  ::testing::FLAGS_gtest_filter = filter;
}

}  // namespace

// Same as BrowserProcessImpl, but uses custom profile manager.
class FakeBrowserProcessImpl : public BrowserProcessImpl {
 public:
  explicit FakeBrowserProcessImpl(const CommandLine& command_line)
      : BrowserProcessImpl(command_line) {
    profiles_dir_.CreateUniqueTempDir();
  }

  virtual ~FakeBrowserProcessImpl() {}

  virtual ProfileManager* profile_manager() OVERRIDE {
    if (!profile_manager_.get()) {
      profile_manager_.reset(
          new ProfileManagerWithoutInit(profiles_dir_.path()));
    }
    return profile_manager_.get();
  }

  virtual MetricsService* metrics_service() OVERRIDE {
    return NULL;
  }

  void DestroyProfileManager() {
    profile_manager_.reset();
  }

 private:
  ScopedTempDir profiles_dir_;
  scoped_ptr<ProfileManager> profile_manager_;
};

class SupplyProxyCredentials : public WindowObserver {
 public:
  SupplyProxyCredentials(const char* username, const char* password);

 protected:
  struct DialogProps {
    HWND username_;
    HWND password_;
  };

  virtual void OnWindowOpen(HWND hwnd);
  virtual void OnWindowClose(HWND hwnd);
  static BOOL CALLBACK EnumChildren(HWND hwnd, LPARAM param);

 protected:
  std::string username_;
  std::string password_;
};


SupplyProxyCredentials::SupplyProxyCredentials(const char* username,
                                               const char* password)
    : username_(username), password_(password) {
}

void SupplyProxyCredentials::OnWindowClose(HWND hwnd) { }

void SupplyProxyCredentials::OnWindowOpen(HWND hwnd) {
  DialogProps props = {0};
  ::EnumChildWindows(hwnd, EnumChildren, reinterpret_cast<LPARAM>(&props));
  DCHECK(::IsWindow(props.username_));
  DCHECK(::IsWindow(props.password_));

  // We can't use SetWindowText to set the username/password, so simulate
  // keyboard input instead.
  simulate_input::ForceSetForegroundWindow(hwnd);
  CHECK(SetFocusToAccessibleWindow(props.username_));
  simulate_input::SendStringA(username_.c_str());
  Sleep(100);

  simulate_input::SendCharA(VK_TAB, simulate_input::NONE);
  Sleep(100);
  simulate_input::SendStringA(password_.c_str());

  Sleep(100);
  simulate_input::SendCharA(VK_RETURN, simulate_input::NONE);
}

// static
BOOL SupplyProxyCredentials::EnumChildren(HWND hwnd, LPARAM param) {
  if (!::IsWindowVisible(hwnd))
    return TRUE;  // Ignore but continue to enumerate.

  DialogProps* props = reinterpret_cast<DialogProps*>(param);

  char class_name[MAX_PATH] = {0};
  ::GetClassNameA(hwnd, class_name, arraysize(class_name));
  if (lstrcmpiA(class_name, "Edit") == 0) {
    if (props->username_ == NULL || props->username_ == hwnd) {
      props->username_ = hwnd;
    } else if (props->password_ == NULL) {
      props->password_ = hwnd;
    }
  } else {
    EnumChildWindows(hwnd, EnumChildren, param);
  }

  return TRUE;
}

FakeExternalTab::FakeExternalTab() {
  user_data_dir_ = chrome_frame_test::GetProfilePathForIE();
  PathService::Get(chrome::DIR_USER_DATA, &overridden_user_dir_);
  PathService::Override(chrome::DIR_USER_DATA, user_data_dir_);
  process_singleton_.reset(new ProcessSingleton(user_data_dir_));
}

FakeExternalTab::~FakeExternalTab() {
  if (!overridden_user_dir_.empty()) {
    PathService::Override(chrome::DIR_USER_DATA, overridden_user_dir_);
  }
}

void FakeExternalTab::Initialize() {
  DCHECK(g_browser_process == NULL);

  TestTimeouts::Initialize();

  // Load Chrome.dll as our resource dll.
  FilePath dll;
  PathService::Get(base::DIR_MODULE, &dll);
  dll = dll.Append(chrome::kBrowserResourcesDll);
  HMODULE res_mod = ::LoadLibraryExW(dll.value().c_str(),
      NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
  DCHECK(res_mod);
  _AtlBaseModule.SetResourceInstance(res_mod);

  ResourceBundle::InitSharedInstance("en-US");

  CommandLine* cmd = CommandLine::ForCurrentProcess();
  cmd->AppendSwitch(switches::kDisableWebResources);
  cmd->AppendSwitch(switches::kSingleProcess);

  browser_process_.reset(new FakeBrowserProcessImpl(*cmd));
  // BrowserProcessImpl's constructor should set g_browser_process.
  DCHECK(g_browser_process);
  g_browser_process->SetApplicationLocale("en-US");

  content::RenderProcessHost::set_run_renderer_in_process(true);

  browser_process_->local_state()->RegisterBooleanPref(
      prefs::kMetricsReportingEnabled, false);
}

void FakeExternalTab::InitializePostThreadsCreated() {
  FilePath profile_path(ProfileManager::GetDefaultProfileDir(user_data()));
  Profile* profile =
      g_browser_process->profile_manager()->GetProfile(profile_path);
}

void FakeExternalTab::Shutdown() {
  browser_process_.reset();
  g_browser_process = NULL;
  process_singleton_.reset();

  ResourceBundle::CleanupSharedInstance();
}

FakeBrowserProcessImpl* FakeExternalTab::browser_process() const {
  return browser_process_.get();
}

CFUrlRequestUnittestRunner::CFUrlRequestUnittestRunner(int argc, char** argv)
    : NetTestSuite(argc, argv, false),
      chrome_frame_html_("/chrome_frame", kChromeFrameHtml),
      registrar_(chrome_frame_test::GetTestBedType()),
      test_result_(0) {
}

CFUrlRequestUnittestRunner::~CFUrlRequestUnittestRunner() {
}

void CFUrlRequestUnittestRunner::StartChromeFrameInHostBrowser() {
  if (!ShouldLaunchBrowser())
    return;

  base::win::ScopedCOMInitializer com;
  chrome_frame_test::CloseAllIEWindows();

  test_http_server_.reset(new test_server::SimpleWebServer(kTestServerPort));
  test_http_server_->AddResponse(&chrome_frame_html_);
  std::wstring url(base::StringPrintf(L"http://localhost:%i/chrome_frame",
                                      kTestServerPort).c_str());

  // Launch IE.  This launches IE correctly on Vista too.
  base::win::ScopedHandle ie_process(chrome_frame_test::LaunchIE(url));
  EXPECT_TRUE(ie_process.IsValid());

  // NOTE: If you're running IE8 and CF is not being loaded, you need to
  // disable IE8's prebinding until CF properly handles that situation.
  //
  // HKCU\Software\Microsoft\Internet Explorer\Main
  // Value name: EnablePreBinding (REG_DWORD)
  // Value: 0
}

void CFUrlRequestUnittestRunner::ShutDownHostBrowser() {
  if (ShouldLaunchBrowser()) {
    base::win::ScopedCOMInitializer com;
    chrome_frame_test::CloseAllIEWindows();
  }
}

// Override virtual void Initialize to not call icu initialize
void CFUrlRequestUnittestRunner::Initialize() {
  DCHECK(::GetCurrentThreadId() == test_thread_id_);

  // Start by replicating some of the steps that would otherwise be
  // done by TestSuite::Initialize.  We can't call the base class
  // directly because it will attempt to initialize some things such as
  // ICU that have already been initialized for this process.
  CFUrlRequestUnittestRunner::InitializeLogging();

  SuppressErrorDialogs();
  base::debug::SetSuppressDebugUI(true);
  logging::SetLogAssertHandler(UnitTestAssertHandler);

  // Next, do some initialization for NetTestSuite.
  NetTestSuite::InitializeTestThreadNoNetworkChangeNotifier();
}

void CFUrlRequestUnittestRunner::Shutdown() {
  DCHECK(::GetCurrentThreadId() == test_thread_id_);
  NetTestSuite::Shutdown();
  OleUninitialize();
}

void CFUrlRequestUnittestRunner::OnConnectAutomationProviderToChannel(
    const std::string& channel_id) {
  Profile* profile = g_browser_process->profile_manager()->
      GetDefaultProfile(fake_chrome_->user_data());

  AutomationProviderList* list = g_browser_process->GetAutomationProviderList();
  DCHECK(list);
  list->AddProvider(
      TestAutomationProvider::NewAutomationProvider(profile, channel_id, this));
}

void CFUrlRequestUnittestRunner::OnInitialTabLoaded() {
  test_http_server_.reset();
  StartTests();
}

void CFUrlRequestUnittestRunner::StartTests() {
  if (PromptAfterSetup())
    MessageBoxA(NULL, "click ok to run", "", MB_OK);

  DCHECK_EQ(test_thread_.IsValid(), false);
  test_thread_.Set(::CreateThread(NULL, 0, RunAllUnittests, this, 0,
                                  &test_thread_id_));
  DCHECK(test_thread_.IsValid());
}

// static
DWORD CFUrlRequestUnittestRunner::RunAllUnittests(void* param) {
  base::PlatformThread::SetName("CFUrlRequestUnittestRunner");
  CFUrlRequestUnittestRunner* me =
      reinterpret_cast<CFUrlRequestUnittestRunner*>(param);
  me->test_result_ = me->Run();
  BrowserThread::PostTask(BrowserThread::UI,
                          FROM_HERE,
                          base::Bind(TakeDownBrowser, me));
  return 0;
}

// static
void CFUrlRequestUnittestRunner::TakeDownBrowser(
    CFUrlRequestUnittestRunner* me) {
  if (PromptAfterSetup())
    MessageBoxA(NULL, "click ok to exit", "", MB_OK);

  me->ShutDownHostBrowser();
  BrowserThread::PostDelayedTask(BrowserThread::UI,
                                 FROM_HERE,
                                 MessageLoop::QuitClosure(),
                                 TestTimeouts::tiny_timeout_ms());
}

void CFUrlRequestUnittestRunner::InitializeLogging() {
  FilePath exe;
  PathService::Get(base::FILE_EXE, &exe);
  FilePath log_filename = exe.ReplaceExtension(FILE_PATH_LITERAL("log"));
  logging::InitLogging(
      log_filename.value().c_str(),
      logging::LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG,
      logging::LOCK_LOG_FILE,
      logging::DELETE_OLD_LOG_FILE,
      logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);
  // We want process and thread IDs because we may have multiple processes.
  // Note: temporarily enabled timestamps in an effort to catch bug 6361.
  logging::SetLogItems(true, true, true, true);
}

void CFUrlRequestUnittestRunner::PreEarlyInitialization() {
  testing::InitGoogleTest(&g_argc, g_argv);
  FilterDisabledTests();
}

void CFUrlRequestUnittestRunner::PreCreateThreads() {
  fake_chrome_.reset(new FakeExternalTab());
  fake_chrome_->Initialize();

  pss_subclass_.reset(new ProcessSingletonSubclass(this));
  EXPECT_TRUE(pss_subclass_->Subclass(fake_chrome_->user_data()));
  StartChromeFrameInHostBrowser();
}

void CFUrlRequestUnittestRunner::PreStartThread(BrowserThread::ID identifier) {
  fake_chrome_->browser_process()->PreStartThread(identifier);
}

void CFUrlRequestUnittestRunner::PostStartThread(BrowserThread::ID identifier) {
  fake_chrome_->browser_process()->PostStartThread(identifier);
}

void CFUrlRequestUnittestRunner::PreMainMessageLoopRun() {
  fake_chrome_->InitializePostThreadsCreated();
}

bool CFUrlRequestUnittestRunner::MainMessageLoopRun(int* result_code) {
  DCHECK(MessageLoop::current());
  DCHECK(MessageLoop::current()->type() == MessageLoop::TYPE_UI);

  // We need to allow IO on the main thread for these tests.
  base::ThreadRestrictions::SetIOAllowed(true);

  return false;
}

void CFUrlRequestUnittestRunner::PostMainMessageLoopRun() {
  fake_chrome_->browser_process()->StartTearDown();

  // Must do this separately as the mock profile_manager_ is not the
  // same member as BrowserProcessImpl::StartTearDown resets.
  fake_chrome_->browser_process()->DestroyProfileManager();

  if (crash_service_)
    base::KillProcess(crash_service_, 0, false);

  base::KillProcesses(chrome_frame_test::kIEImageName, 0, NULL);
  base::KillProcesses(chrome_frame_test::kIEBrokerImageName, 0, NULL);
}

void CFUrlRequestUnittestRunner::PreStopThread(
    content::BrowserThread::ID identifier) {
  fake_chrome_->browser_process()->PreStopThread(identifier);
}

void CFUrlRequestUnittestRunner::PostStopThread(
    content::BrowserThread::ID identifier) {
  fake_chrome_->browser_process()->PostStopThread(identifier);
}

void CFUrlRequestUnittestRunner::PostDestroyThreads() {
  fake_chrome_->Shutdown();
  fake_chrome_.reset();

#ifndef NDEBUG
  // Avoid CRT cleanup in debug test runs to ensure that webkit ASSERTs which
  // check if globals are created and destroyed on the same thread don't fire.
  // Webkit global objects are created on the inproc renderer thread.
  ExitProcess(test_result());
#endif
}

const char* IEVersionToString(IEVersion version) {
  switch (version) {
    case IE_6:
      return "IE6";
    case IE_7:
      return "IE7";
    case IE_8:
      return "IE8";
    case IE_9:
      return "IE9";
    case IE_10:
      return "IE10";
    case IE_UNSUPPORTED:
      return "Unknown IE Version";
    case NON_IE:
      return "Could not find IE";
    default:
      return "Error.";
  }
}

content::BrowserMainParts* FakeContentBrowserClient::CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) {
  // We never delete this, as the content module takes ownership.
  //
  // We must not construct this earlier, or we will have out-of-order
  // AtExitManager creation/destruction.
  g_test_suite = new CFUrlRequestUnittestRunner(g_argc, g_argv);
  g_test_suite->set_crash_service(chrome_frame_test::StartCrashService());
  return g_test_suite;
}

// We must provide a few functions content/ expects to link with.  They
// are never called for this executable.
int PluginMain(const content::MainFunctionParams& parameters) {
  NOTREACHED();
  return 0;
}

int PpapiBrokerMain(const content::MainFunctionParams& parameters) {
  return PluginMain(parameters);
}

int PpapiPluginMain(const content::MainFunctionParams& parameters) {
  return PluginMain(parameters);
}

int WorkerMain(const content::MainFunctionParams& parameters) {
  return PluginMain(parameters);
}

int main(int argc, char** argv) {
  g_argc = argc;
  g_argv = argv;

  if (chrome_frame_test::GetInstalledIEVersion() >= IE_9) {
    // Adding this here as the command line and the logging stuff gets
    // initialized in the NetTestSuite constructor. Did not want to break that.
    base::AtExitManager at_exit_manager;
    CommandLine::Init(argc, argv);
    CFUrlRequestUnittestRunner::InitializeLogging();
    LOG(INFO) << "Not running ChromeFrame net tests on IE9+";
    return 0;
  }

  google_breakpad::scoped_ptr<google_breakpad::ExceptionHandler> breakpad(
      InitializeCrashReporting(HEADLESS));

  // Display the IE version we run with. This must be done after
  // CFUrlRequestUnittestRunner is constructed since that initializes logging.
  IEVersion ie_version = chrome_frame_test::GetInstalledIEVersion();
  LOG(INFO) << "Running CF net tests with IE version: "
            << IEVersionToString(ie_version);

  // See url_request_unittest.cc for these credentials.
  SupplyProxyCredentials credentials("user", "secret");
  WindowWatchdog watchdog;
  watchdog.AddObserver(&credentials, "Windows Security", "");

  sandbox::SandboxInterfaceInfo sandbox_info = {0};
  // This would normally be done, but is probably not needed for these tests.
  // content::InitializeSandboxInfo(&sandbox_info);
  FakeMainDelegate delegate;
  content::ContentMain(
      reinterpret_cast<HINSTANCE>(GetModuleHandle(NULL)),
      &sandbox_info,
      &delegate);

  // Note:  In debug builds, we ExitProcess during PostDestroyThreads.
  return g_test_suite->test_result();
}
