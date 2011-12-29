// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_INSTALLER_UTIL_APP_COMMAND_H_
#define CHROME_INSTALLER_UTIL_APP_COMMAND_H_
#pragma once

#include <windows.h>

#include <string>

class WorkItemList;

namespace base {
namespace win {
class RegKey;
}
}

namespace installer {

// A description of a command registered by setup.exe that can be invoked by
// Google Update.  This class is CopyConstructible and Assignable for use in
// STL containers.
class AppCommand {
 public:
  AppCommand();
  AppCommand(const std::wstring& command_line, bool send_pings,
             bool is_web_accessible);
  // The implicit dtor, copy ctor and assignment operator are desired.

  // Initializes an instance from the command in |key|.
  bool Initialize(const base::win::RegKey& key);

  // Adds to |item_list| work items to write this object to the key named
  // |command_path| under |predefined_root|.
  void AddWorkItems(HKEY predefined_root,
                    const std::wstring& command_path,
                    WorkItemList* item_list) const;

  // Returns the command-line for the app command as it is represented in the
  // registry.  Use CommandLine::FromString() on this value to check arguments
  // or to launch the command.
  const std::wstring& command_line() const { return command_line_; }
  void set_command_line(const std::wstring& command_line) {
    command_line_ = command_line;
  }

  bool sends_pings() const { return sends_pings_; }
  void set_sends_pings(bool sends_pings) { sends_pings_ = sends_pings; }

  bool is_web_accessible() const { return is_web_accessible_; }
  void set_is_web_accessible(bool is_web_accessible) {
    is_web_accessible_ = is_web_accessible;
  }

 protected:
  std::wstring command_line_;
  bool sends_pings_;
  bool is_web_accessible_;
};

}  // namespace installer

#endif  // CHROME_INSTALLER_UTIL_APP_COMMAND_H_
