// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_BROWSER_CONTENT_IPC_LOGGING_H_
#define CONTENT_PUBLIC_BROWSER_CONTENT_IPC_LOGGING_H_
#pragma once

#include "content/common/content_export.h"
#include "ipc/ipc_message.h"  // For IPC_MESSAGE_LOG_ENABLED.

namespace content {

#if defined(IPC_MESSAGE_LOG_ENABLED)

// Enable or disable IPC logging for the browser, all processes
// derived from ChildProcess (plugin etc), and all
// renderers.
CONTENT_EXPORT void EnableIPCLogging(bool enable);

#endif

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_CONTENT_IPC_LOGGING_H_
