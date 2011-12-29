// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// proxy api test
// browser_tests.exe --gtest_filter=ProxySettingsApiTest.ProxyEventsInvalidProxy

var expected_error = {
    error: "net::ERR_PROXY_CONNECTION_FAILED",
    details: "",
    fatal: true
};
var empty_json_url = "";

function test() {
  // Install error handler and get the test server config.
  chrome.proxy.onProxyError.addListener(function (error) {
    chrome.test.assertEq(expected_error, error);
    chrome.test.notifyPass();
  });
  chrome.test.getConfig(readConfigAndSetProxy);
}

function readConfigAndSetProxy(test_config) {
  // Construct the URL used for XHRs and set the proxy settings.
  empty_json_url = "http://127.0.0.1:" +
      test_config.testServer.port +
      "/files/extensions/api_test/proxy/events/empty.json";

  // Set an invalid proxy and fire of a XHR. This should trigger proxy errors.
  // There may be any number of proxy errors, as systems like safe browsing
  // might start network traffic as well.
  var rules = {
    singleProxy: { host: "does.not.exist" }
  };
  var config = { rules: rules, mode: "fixed_servers" };
  chrome.proxy.settings.set({'value': config}, sendFailingXHR);
}

function sendFailingXHR() {
  var req = new XMLHttpRequest();
  req.open("GET", empty_json_url, true);
  req.onload = function () {
    chrome.test.notifyFail("proxy settings should not work");
  }
  req.onerror = testDone;
  req.send(null);
}

function testDone() {
 // Do nothing. The test success/failure is decided in the event handler.
}

test();
