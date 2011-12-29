// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_UNPACKED_INSTALLER_H_
#define CHROME_BROWSER_EXTENSIONS_UNPACKED_INSTALLER_H_
#pragma once

#include <string>

#include "base/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"

class Extension;
class ExtensionService;

namespace extensions {

// Installs and loads an unpacked extension.
// TODO(erikkay): It might be useful to be able to load a packed extension
// (presumably into memory) without installing it.
class UnpackedInstaller
    : public base::RefCountedThreadSafe<UnpackedInstaller> {
 public:
  static scoped_refptr<UnpackedInstaller> Create(
      ExtensionService* extension_service);

  // Loads the extension from the directory |extension_path|, which is
  // the top directory of a specific extension where its manifest file lives.
  // Errors are reported through ExtensionErrorReporter. On success,
  // ExtensionService::AddExtension() is called.
  void Load(const FilePath& extension_path);

  // Loads the extension from the directory |extension_path|;
  // for use with command line switch --load-extension=path.
  // This is equivalent to Load, except that it runs synchronously.
  void LoadFromCommandLine(const FilePath& extension_path);

  // Allows prompting for plugins to be disabled; intended for testing only.
  bool prompt_for_plugins() { return prompt_for_plugins_; }
  void set_prompt_for_plugins(bool val) { prompt_for_plugins_ = val; }

 private:
  friend class base::RefCountedThreadSafe<UnpackedInstaller>;

  explicit UnpackedInstaller(ExtensionService* extension_service);
  virtual ~UnpackedInstaller();

  // We change the input extension path to an absolute path, on the file thread.
  // Then we need to check the file access preference, which needs
  // to happen back on the UI thread, so it posts CheckExtensionFileAccess on
  // the UI thread. In turn, once that gets the pref, it goes back to the
  // file thread with LoadWithFileAccess.
  // TODO(yoz): It would be nice to remove this ping-pong, but we need to know
  // what file access flags to pass to extension_file_util::LoadExtension.
  void GetAbsolutePath();
  void CheckExtensionFileAccess();
  void LoadWithFileAccess(bool allow_file_access);

  // Notify the frontend that there was an error loading an extension.
  void ReportExtensionLoadError(const std::string& error);

  // Called when an unpacked extension has been loaded and installed.
  void OnLoaded(const scoped_refptr<const Extension>& extension);

  base::WeakPtr<ExtensionService> service_weak_;

  // The pathname of the directory to load from, which is an absolute path
  // after GetAbsolutePath has been called.
  FilePath extension_path_;

  // If true and the extension contains plugins, we prompt the user before
  // loading.
  bool prompt_for_plugins_;

  DISALLOW_COPY_AND_ASSIGN(UnpackedInstaller);
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_UNPACKED_INSTALLER_H_
