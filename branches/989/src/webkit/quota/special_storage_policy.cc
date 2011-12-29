// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webkit/quota/special_storage_policy.h"

namespace quota {

SpecialStoragePolicy::Observer::~Observer() {}

SpecialStoragePolicy::SpecialStoragePolicy() {}

SpecialStoragePolicy::~SpecialStoragePolicy() {}

void SpecialStoragePolicy::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void SpecialStoragePolicy::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void SpecialStoragePolicy::NotifyObservers() {
  scoped_refptr<SpecialStoragePolicy> protect(this);
  FOR_EACH_OBSERVER(Observer, observers_, OnSpecialStoragePolicyChanged());
}

}  // namespace quota
