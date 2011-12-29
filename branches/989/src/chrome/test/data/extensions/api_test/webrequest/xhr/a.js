// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function sendXHR() {
  var req = new XMLHttpRequest();
  req.open("GET", "data.json", true);
  req.send(null);
}

window.onload = function() {
  sendXHR();
};
