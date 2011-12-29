// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SANDBOX_SRC_INTERCEPTORS_H_
#define SANDBOX_SRC_INTERCEPTORS_H_

#if defined(_WIN64)
#include "sandbox/src/interceptors_64.h"
#endif

namespace sandbox {

enum InterceptorId {
  // Internal use:
  MAP_VIEW_OF_SECTION_ID = 0,
  UNMAP_VIEW_OF_SECTION_ID,
  // Policy broker:
  SET_INFORMATION_THREAD_ID,
  OPEN_THREAD_TOKEN_ID,
  OPEN_THREAD_TOKEN_EX_ID,
  OPEN_TREAD_ID,
  OPEN_PROCESS_ID,
  OPEN_PROCESS_TOKEN_ID,
  OPEN_PROCESS_TOKEN_EX_ID,
  // Filesystem dispatcher:
  CREATE_FILE_ID,
  OPEN_FILE_ID,
  QUERY_ATTRIB_FILE_ID,
  QUERY_FULL_ATTRIB_FILE_ID,
  SET_INFO_FILE_ID,
  // Named pipe dispatcher:
  CREATE_NAMED_PIPE_ID,
  // Process-thread dispatcher:
  CREATE_PROCESSW_ID,
  CREATE_PROCESSA_ID,
  // Registry dispatcher:
  CREATE_KEY_ID,
  OPEN_KEY_ID,
  OPEN_KEY_EX_ID,
  // Sync dispatcher:
  CREATE_EVENT_ID,
  OPEN_EVENT_ID,
  // CSRSS bypasses for HandleCloser:
  CREATE_THREAD_ID,
  GET_USER_DEFAULT_LCID_ID,
  INTERCEPTOR_MAX_ID
};

typedef void* OriginalFunctions[INTERCEPTOR_MAX_ID];

}  // namespace sandbox

#endif  // SANDBOX_SRC_INTERCEPTORS_H_
