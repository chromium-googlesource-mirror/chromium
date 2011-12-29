// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_SOCKET_SSL_SERVER_SOCKET_H_
#define NET_SOCKET_SSL_SERVER_SOCKET_H_

#include "base/basictypes.h"
#include "net/base/completion_callback.h"
#include "net/base/net_export.h"
#include "net/socket/ssl_socket.h"
#include "net/socket/stream_socket.h"

namespace crypto {
class RSAPrivateKey;
}  // namespace crypto

namespace net {

struct SSLConfig;
class X509Certificate;

class SSLServerSocket : public SSLSocket {
 public:
  virtual ~SSLServerSocket() {}

  // Perform the SSL server handshake, and notify the supplied callback
  // if the process completes asynchronously.  If Disconnect is called before
  // completion then the callback will be silently, as for other StreamSocket
  // calls.
  virtual int Handshake(const CompletionCallback& callback) = 0;
};

// Creates an SSL server socket over an already-connected transport socket.
// The caller must provide the server certificate and private key to use.
//
// The returned SSLServerSocket takes ownership of |socket|.  Stubbed versions
// of CreateSSLServerSocket will delete |socket| and return NULL.
// It takes a reference to |certificate|.
// The |key| and |ssl_config| parameters are copied.  |key| cannot be const
// because the methods used to copy its contents are non-const.
//
// The caller starts the SSL server handshake by calling Handshake on the
// returned socket.
NET_EXPORT SSLServerSocket* CreateSSLServerSocket(
    StreamSocket* socket,
    X509Certificate* certificate,
    crypto::RSAPrivateKey* key,
    const SSLConfig& ssl_config);

}  // namespace net

#endif  // NET_SOCKET_SSL_SERVER_SOCKET_H_
