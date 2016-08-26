// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/cros_settings_provider.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/string_util.h"
#include "base/values.h"
#include "chrome/common/chrome_switches.h"

namespace chromeos {

CrosSettingsProvider::CrosSettingsProvider(
    const NotifyObserversCallback& notify_cb)
  : notify_cb_(notify_cb) {
}

CrosSettingsProvider::~CrosSettingsProvider() {
}

void CrosSettingsProvider::Set(const std::string& path,
                               const base::Value& value) {
  // We don't allow changing any of the cros settings without prefix
  // "cros.session." in the guest mode.
  // It should not reach here from UI in the guest mode, but just in case.
  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kGuestSession) &&
      !::StartsWithASCII(path, "cros.session.", true)) {
    LOG(ERROR) << "Ignoring the guest request to change: " << path;
    return;
  }
  DoSet(path, value);
}

void CrosSettingsProvider::NotifyObservers(const std::string& path) {
  notify_cb_.Run(path);
}

};  // namespace chromeos