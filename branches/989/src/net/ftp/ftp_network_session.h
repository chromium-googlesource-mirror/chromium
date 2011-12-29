// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_FTP_FTP_NETWORK_SESSION_H_
#define NET_FTP_FTP_NETWORK_SESSION_H_
#pragma once

#include "base/memory/ref_counted.h"
#include "net/base/net_export.h"
#include "net/ftp/ftp_auth_cache.h"

namespace net {

class HostResolver;

// This class holds session objects used by FtpNetworkTransaction objects.
class NET_EXPORT_PRIVATE FtpNetworkSession
    : public base::RefCounted<FtpNetworkSession> {
 public:
  explicit FtpNetworkSession(HostResolver* host_resolver);

  HostResolver* host_resolver() { return host_resolver_; }
  FtpAuthCache* auth_cache() { return &auth_cache_; }

 private:
  friend class base::RefCounted<FtpNetworkSession>;

  virtual ~FtpNetworkSession();

  HostResolver* const host_resolver_;
  FtpAuthCache auth_cache_;
};

}  // namespace net

#endif  // NET_FTP_FTP_NETWORK_SESSION_H_
