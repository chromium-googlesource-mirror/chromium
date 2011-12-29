// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "remoting/protocol/it2me_host_authenticator_factory.h"

#include "base/logging.h"
#include "crypto/rsa_private_key.h"
#include "remoting/protocol/v1_authenticator.h"
#include "remoting/protocol/v2_authenticator.h"

namespace remoting {
namespace protocol {

It2MeHostAuthenticatorFactory::It2MeHostAuthenticatorFactory(
    const std::string& local_cert,
    const crypto::RSAPrivateKey* local_private_key,
    const std::string& shared_secret)
    : local_cert_(local_cert),
      local_private_key_(local_private_key->Copy()),
      shared_secret_(shared_secret) {
}

It2MeHostAuthenticatorFactory::~It2MeHostAuthenticatorFactory() {
}

Authenticator* It2MeHostAuthenticatorFactory::CreateAuthenticator(
    const std::string& remote_jid,
    const buzz::XmlElement* first_message) {
  if (V2Authenticator::IsEkeMessage(first_message)) {
    return V2Authenticator::CreateForHost(
        local_cert_, local_private_key_.get(), shared_secret_);
  }

  return new V1HostAuthenticator(local_cert_, local_private_key_.get(),
                                 shared_secret_, remote_jid);
}

}  // namespace protocol
}  // namespace remoting
