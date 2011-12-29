// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/common/process_type.h"

#include "base/logging.h"

namespace content {

std::string GetProcessTypeNameInEnglish(ProcessType type) {
  switch (type) {
    case PROCESS_TYPE_BROWSER:
      return "Browser";
    case PROCESS_TYPE_RENDERER:
      return "Tab";
    case PROCESS_TYPE_PLUGIN:
      return "Plug-in";
    case PROCESS_TYPE_WORKER:
      return "Web Worker";
    case PROCESS_TYPE_UTILITY:
      return "Utility";
    case PROCESS_TYPE_PROFILE_IMPORT:
      return "Profile Import helper";
    case PROCESS_TYPE_ZYGOTE:
      return "Zygote";
    case PROCESS_TYPE_SANDBOX_HELPER:
      return "Sandbox helper";
    case PROCESS_TYPE_NACL_LOADER:
      return "Native Client module";
    case PROCESS_TYPE_NACL_BROKER:
      return "Native Client broker";
    case PROCESS_TYPE_GPU:
      return "GPU";
    case PROCESS_TYPE_PPAPI_PLUGIN:
      return "Pepper Plugin";
    case PROCESS_TYPE_PPAPI_BROKER:
      return "Pepper Plugin Broker";
    case PROCESS_TYPE_UNKNOWN:
    default:
      DCHECK(false) << "Unknown child process type!";
      return "Unknown";
  }
}

}  // namespace content
