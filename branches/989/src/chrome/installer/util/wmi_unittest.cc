// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <windows.h>

#include "chrome/installer/util/wmi.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace installer {

TEST(WMITest, TestLocalConnectionSecurityBlanket) {
  ::CoInitialize(NULL);
  IWbemServices* services = NULL;
  EXPECT_TRUE(WMI::CreateLocalConnection(true, &services));
  ASSERT_TRUE(NULL != services);
  ULONG refs = services->Release();
  EXPECT_EQ(refs, 0);
  ::CoUninitialize();
}

TEST(WMITest, TestLocalConnectionNoSecurityBlanket) {
  ::CoInitialize(NULL);
  IWbemServices* services = NULL;
  EXPECT_TRUE(WMI::CreateLocalConnection(false, &services));
  ASSERT_TRUE(NULL != services);
  ULONG refs = services->Release();
  EXPECT_EQ(refs, 0);
  ::CoUninitialize();
}

TEST(WMITest, TestCreateClassMethod) {
  ::CoInitialize(NULL);
  IWbemServices* wmi_services = NULL;
  EXPECT_TRUE(WMI::CreateLocalConnection(true, &wmi_services));
  ASSERT_TRUE(NULL != wmi_services);
  IWbemClassObject* class_method = NULL;
  EXPECT_TRUE(WMI::CreateClassMethodObject(wmi_services,
                                               L"Win32_ShortcutFile",
                                               L"Rename", &class_method));
  ASSERT_TRUE(NULL != class_method);
  ULONG refs = class_method->Release();
  EXPECT_EQ(refs, 0);
  refs = wmi_services->Release();
  EXPECT_EQ(refs, 0);
  ::CoUninitialize();
}

// Creates an instance of cmd which executes 'echo' and exits immediately.
TEST(WMITest, TestLaunchProcess) {
  ::CoInitialize(NULL);
  int pid = 0;
  bool result = WMIProcess::Launch(L"cmd.exe /c echo excelent!", &pid);
  EXPECT_TRUE(result);
  EXPECT_GT(pid, 0);
  ::CoUninitialize();
}

}  // namespace installer
