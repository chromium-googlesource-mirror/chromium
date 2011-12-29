// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_SOCKET_SOCKET_EVENT_NOTIFIER_H_
#define CHROME_BROWSER_EXTENSIONS_API_SOCKET_SOCKET_EVENT_NOTIFIER_H_
#pragma once

#include <string>

#include "googleurl/src/gurl.h"

class ExtensionEventRouter;
class Profile;

namespace events {
extern const char kOnSocketEvent[];
};  // namespace events

namespace extensions {

enum SocketEventType {
  SOCKET_EVENT_WRITE_COMPLETE
};

extern const char kSrcIdKey[];

// Contains the data that a Socket needs to send an event back to the extension
// that instantiated it.
class SocketEventNotifier {
 public:
  SocketEventNotifier(ExtensionEventRouter* router,
                      Profile* profile,
                      const std::string& src_extension_id, int src_id,
                      const GURL& src_url);
  virtual ~SocketEventNotifier();

  virtual void OnEvent(SocketEventType event_type, int result_code);

  static std::string SocketEventTypeToString(SocketEventType event_type);

 private:
  ExtensionEventRouter* router_;
  Profile* profile_;
  std::string src_extension_id_;
  int src_id_;
  GURL src_url_;

  DISALLOW_COPY_AND_ASSIGN(SocketEventNotifier);
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_SOCKET_SOCKET_EVENT_NOTIFIER_H_
