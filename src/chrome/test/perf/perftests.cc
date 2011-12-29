// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/message_loop.h"
#include "base/test/perf_test_suite.h"
#include "chrome/common/chrome_paths.cc"

int main(int argc, char **argv) {
  base::PerfTestSuite suite(argc, argv);
  chrome::RegisterPathProvider();
  MessageLoop main_message_loop;

  return suite.Run();
}
