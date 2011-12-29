// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// All of the calls to chrome.* functions should succeed, since this extension
// has requested all required permissions.

var pass = chrome.test.callbackPass;

chrome.test.runTests([
  function experimental() {
    // Test that use of an experimental API works.
    // If/when chrome.experimental.processes is moved out of
    // experimental, this test needs to be updated.
    chrome.tabs.getSelected(null, function(tab) {
      try {
        chrome.experimental.processes.getProcessIdForTab(
          tab.id, pass(function(pid) {}));
      } catch (e) {
        chrome.test.fail();
      }
    });
  },

  function history() {
    try {
      var query = { 'text': '', 'maxResults': 1 };
      chrome.history.search(query, pass(function(results) {}));
    } catch (e) {
      chrome.test.fail();
    }
  },

  function bookmarks() {
    try {
      chrome.bookmarks.get("1", pass(function(results) {}));
    } catch (e) {
      chrome.test.fail();
    }
  },

  function tabs() {
    try {
      chrome.tabs.getSelected(null, pass(function(results) {}));
    } catch (e) {
      chrome.test.fail();
    }
  }
]);
