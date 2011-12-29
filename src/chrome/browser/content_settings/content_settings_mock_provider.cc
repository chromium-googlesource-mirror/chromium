// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/content_settings/content_settings_mock_provider.h"

namespace content_settings {

MockProvider::MockProvider()
    : read_only_(false) {}

MockProvider::MockProvider(bool read_only)
    : read_only_(read_only) {
}

MockProvider::~MockProvider() {}

RuleIterator* MockProvider::GetRuleIterator(
    ContentSettingsType content_type,
    const ResourceIdentifier& resource_identifier,
    bool incognito) const {
  return value_map_.GetRuleIterator(content_type, resource_identifier, NULL);
}

bool MockProvider::SetWebsiteSetting(
    const ContentSettingsPattern& requesting_url_pattern,
    const ContentSettingsPattern& embedding_url_pattern,
    ContentSettingsType content_type,
    const ResourceIdentifier& resource_identifier,
    base::Value* value) {
  if (read_only_)
    return false;
  value_map_.clear();
  value_map_.SetValue(requesting_url_pattern,
                      embedding_url_pattern,
                      content_type,
                      resource_identifier,
                      value);
  return true;
}

void MockProvider::ShutdownOnUIThread() {
  RemoveAllObservers();
}

}  // namespace content_settings
