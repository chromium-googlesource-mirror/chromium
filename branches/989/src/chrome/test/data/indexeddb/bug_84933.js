// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function test()
{
  var n = 25;
  for (var i = 0; i < n; i++) {
    webkitIndexedDB.open("bug_84933_" + i.toString()).onsuccess = function() {
      window.close();
    }
  }
}
