// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PPAPI_CPP_PRIVATE_UDP_SOCKET_PRIVATE_H_
#define PPAPI_CPP_PRIVATE_UDP_SOCKET_PRIVATE_H_

#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/private/ppb_udp_socket_private.h"
#include "ppapi/cpp/resource.h"

namespace pp {

class CompletionCallback;
class Instance;

class UDPSocketPrivate : public Resource {
 public:
  explicit UDPSocketPrivate(Instance* instance);

  int32_t Bind(const PP_NetAddress_Private* addr,
               const CompletionCallback& callback);
  int32_t RecvFrom(char* buffer,
                   int32_t num_bytes,
                   const CompletionCallback& callback);
  bool GetRecvFromAddress(PP_NetAddress_Private* addr);
  int32_t SendTo(const char* buffer,
                 int32_t num_bytes,
                 const PP_NetAddress_Private* addr,
                 const CompletionCallback& callback);
  void Close();
};

}  // namespace pp

#endif  // PPAPI_CPP_PRIVATE_UDP_SOCKET_PRIVATE_H_

