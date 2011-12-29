// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBKIT_PLUGINS_NPAPI_TEST_PLUGIN_JAVASCRIPT_OPEN_POPUP_H_
#define WEBKIT_PLUGINS_NPAPI_TEST_PLUGIN_JAVASCRIPT_OPEN_POPUP_H_

#include "base/compiler_specific.h"
#include "webkit/plugins/npapi/test/plugin_test.h"

namespace NPAPIClient {

// This class tests the case where a windowed plugin instance is
// instantiated in a popup window. The plugin instance needs to
// have a valid parent window.
class ExecuteJavascriptOpenPopupWithPluginTest : public PluginTest {
 public:
  // Constructor.
  ExecuteJavascriptOpenPopupWithPluginTest(
      NPP id, NPNetscapeFuncs *host_functions);
  // NPAPI SetWindow handler.
  virtual NPError SetWindow(NPWindow* window) OVERRIDE;

 private:
  bool popup_window_test_started_;
};

// This class represents a windowed plugin instance instantiated within a
// popup window. It verifies that the plugin instance has a valid parent.
class ExecuteJavascriptPopupWindowTargetPluginTest : public PluginTest {
 public:
  ExecuteJavascriptPopupWindowTargetPluginTest(
      NPP id, NPNetscapeFuncs *host_functions);
  // NPAPI SetWindow handler.
  virtual NPError SetWindow(NPWindow* window) OVERRIDE;

 private:
  // Do a platform-specific validation of the passed-in |window|.
  // E.g. on Windows, verifies window->window is a reasonable HWND.
  // Returns true if the test should be marked complete.
  bool CheckWindow(NPWindow* window);

  bool test_completed_;
};

}  // namespace NPAPIClient

#endif // WEBKIT_PLUGINS_NPAPI_TEST_PLUGIN_JAVASCRIPT_OPEN_POPUP_H_
