// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_INSTALLER_UTIL_PRODUCT_UNITTEST_H_
#define CHROME_INSTALLER_UTIL_PRODUCT_UNITTEST_H_
#pragma once

#include <windows.h>

#include "base/file_path.h"
#include "base/file_util.h"
#include "base/path_service.h"
#include "base/scoped_temp_dir.h"
#include "testing/gtest/include/gtest/gtest.h"

class TestWithTempDir : public testing::Test {
 protected:
  virtual void SetUp();
  virtual void TearDown();

  ScopedTempDir test_dir_;
};

class TestWithTempDirAndDeleteTempOverrideKeys : public TestWithTempDir {
 protected:
  virtual void SetUp();
  virtual void TearDown();
};

#endif  // CHROME_INSTALLER_UTIL_PRODUCT_UNITTEST_H_
