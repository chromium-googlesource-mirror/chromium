// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function makeAbsoluteUrl(path) {
  var parts = location.href.split("/");
  parts.pop();
  parts.push(path);
  return parts.join("/");
}

var tests = [
  function IconUrlFailure() {
    var manifest = getManifest();
    var loadFailureUrl = makeAbsoluteUrl("does_not_exist.png");
    chrome.webstorePrivate.beginInstallWithManifest3(
        {'id': extensionId,'iconUrl': loadFailureUrl, 'manifest': manifest },
        callbackFail("Image decode failed", function(result) {
      assertEq(result, "icon_error");
    }));
  },

  function IconUrlSuccess() {
    var manifest = getManifest();

    // The |absoluteIconUrl| parameter controls whether to use a relative or
    // absolute url for the test.
    function runSuccessTest(absoluteIconUrl) {
      var iconPath = "extension/icon.png";
      var iconUrl = absoluteIconUrl ? makeAbsoluteUrl(iconPath) : iconPath;
      chrome.webstorePrivate.beginInstallWithManifest3(
          {'id': extensionId,'iconUrl': iconUrl, 'manifest': manifest },
          callbackPass());
    }
    runSuccessTest(true);
    runSuccessTest(false);
  }
];

runTests(tests);
