// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/api/socket/socket_event_notifier.h"

#include "base/json/json_writer.h"
#include "base/values.h"
#include "chrome/browser/extensions/extension_event_router.h"
#include "chrome/browser/profiles/profile.h"

namespace events {
const char kOnSocketEvent[] = "experimental.socket.onEvent";
};  // namespace events

namespace extensions {

const char kEventTypeKey[] = "type";
const char kEventTypeWriteComplete[] = "writeComplete";

const char kSrcIdKey[] = "srcId";
const char kIsFinalEventKey[] = "isFinalEvent";

const char kResultCodeKey[] = "resultCode";

SocketEventNotifier::SocketEventNotifier(ExtensionEventRouter* router,
                                         Profile* profile,
                                         const std::string& src_extension_id,
                                         int src_id,
                                         const GURL& src_url)
    : router_(router),
      profile_(profile),
      src_extension_id_(src_extension_id),
      src_id_(src_id),
      src_url_(src_url) {}

SocketEventNotifier::~SocketEventNotifier() {}

void SocketEventNotifier::OnEvent(SocketEventType event_type,
                                  int result_code) {
  // Do we have a destination for this event? There will be one if a source id
  // was injected by the request handler for socket.create in
  // schema_generated_bindings.js, which will in turn be the case if the caller
  // of socket.create provided an onEvent closure.
  if (src_id_ < 0)
    return;

  std::string event_type_string = SocketEventTypeToString(event_type);

  ListValue args;
  DictionaryValue* event = new DictionaryValue();
  event->SetString(kEventTypeKey, event_type_string);
  event->SetInteger(kSrcIdKey, src_id_);

  // TODO(miket): Signal that it's OK to clean up onEvent listeners. This is
  // the framework we'll use, but we need to start using it.
  event->SetBoolean(kIsFinalEventKey, false);

  if (event_type == SOCKET_EVENT_WRITE_COMPLETE) {
    event->SetInteger(kResultCodeKey, result_code);
  }

  args.Set(0, event);
  std::string json_args;
  base::JSONWriter::Write(&args, false, &json_args);

  router_->DispatchEventToExtension(src_extension_id_, events::kOnSocketEvent,
                                    json_args, profile_, src_url_);
}

// static
std::string SocketEventNotifier::SocketEventTypeToString(
    SocketEventType event_type) {
  switch (event_type) {
    case SOCKET_EVENT_WRITE_COMPLETE:
      return kEventTypeWriteComplete;
  }

  NOTREACHED();
  return std::string();
}

}  // namespace extensions
