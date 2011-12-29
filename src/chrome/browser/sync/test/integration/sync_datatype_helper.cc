// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/sync/test/integration/sync_datatype_helper.h"

#include "chrome/browser/sync/test/integration/sync_test.h"

namespace {

// The SyncTest instance associated with sync_datatype_helper.
static SyncTest* test = NULL;

}  // namespace

namespace sync_datatype_helper {

void AssociateWithTest(SyncTest* test) {
  ASSERT_TRUE(test != NULL) << "Cannot associate with null test.";
  ASSERT_TRUE(::test == NULL) << "Already associated with a test.";
  ::test = test;
}

SyncTest* test() {
  EXPECT_TRUE(::test != NULL) << "Must call AssociateWithTest first.";
  return ::test;
}

}  // namespace sync_datatype_helper
