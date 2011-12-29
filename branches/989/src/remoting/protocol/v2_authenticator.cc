// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "remoting/protocol/v2_authenticator.h"

#include "base/base64.h"
#include "base/logging.h"
#include "crypto/rsa_private_key.h"
#include "remoting/base/constants.h"
#include "remoting/protocol/ssl_hmac_channel_authenticator.h"
#include "third_party/libjingle/source/talk/xmllite/xmlelement.h"

using crypto::P224EncryptedKeyExchange;

#if defined(_WIN32) && defined(GetMessage)
#undef GetMessage
#endif

namespace remoting {
namespace protocol {

namespace {

const buzz::StaticQName kEkeTag = { kChromotingXmlNamespace,
                                    "eke-message" };
const buzz::StaticQName kCertificateTag = { kChromotingXmlNamespace,
                                            "certificate" };

}  // namespace

// static
bool V2Authenticator::IsEkeMessage(const buzz::XmlElement* message) {
  return message->FirstNamed(kEkeTag) != NULL;
}

// static
V2Authenticator* V2Authenticator::CreateForClient(
      const std::string& shared_secret) {
  return new V2Authenticator(
      P224EncryptedKeyExchange::kPeerTypeClient, shared_secret);
}

// static
V2Authenticator* V2Authenticator::CreateForHost(
    const std::string& local_cert,
    crypto::RSAPrivateKey* local_private_key,
    const std::string& shared_secret) {
  V2Authenticator* result = new V2Authenticator(
      P224EncryptedKeyExchange::kPeerTypeServer, shared_secret);
  result->local_cert_ = local_cert;
  result->local_private_key_.reset(local_private_key->Copy());
  result->state_ = WAITING_MESSAGE;
  return result;
}

V2Authenticator::V2Authenticator(
    crypto::P224EncryptedKeyExchange::PeerType type,
    const std::string& shared_secret)
    : certificate_sent_(false),
      key_exchange_impl_(type, shared_secret),
      state_(MESSAGE_READY) {
  pending_messages_.push(key_exchange_impl_.GetMessage());
}

V2Authenticator::~V2Authenticator() {
}

Authenticator::State V2Authenticator::state() const {
  if (state_ == ACCEPTED && !pending_messages_.empty())
    return MESSAGE_READY;
  return state_;
}

void V2Authenticator::ProcessMessage(const buzz::XmlElement* message) {
  DCHECK_EQ(state(), WAITING_MESSAGE);

  // Parse the certificate.
  std::string base64_cert = message->TextNamed(kCertificateTag);
  if (!base64_cert.empty()) {
    if (!base::Base64Decode(base64_cert, &remote_cert_)) {
      LOG(WARNING) << "Failed to decode certificate received from the peer.";
      remote_cert_.clear();
    }
  }

  // Client always expect certificate in the first message.
  if (!is_host_side() && remote_cert_.empty()) {
    LOG(WARNING) << "No valid host certificate.";
    state_ = REJECTED;
    return;
  }

  const buzz::XmlElement* eke_element = message->FirstNamed(kEkeTag);
  if (!eke_element) {
    LOG(WARNING) << "No eke-message found.";
    state_ = REJECTED;
    return;
  }

  for (; eke_element; eke_element = eke_element->NextNamed(kEkeTag)) {
    std::string base64_message = eke_element->BodyText();
    std::string spake_message;
    if (base64_message.empty() ||
        !base::Base64Decode(base64_message, &spake_message)) {
      LOG(WARNING) << "Failed to decode auth message received from the peer.";
      state_ = REJECTED;
      return;
    }

    P224EncryptedKeyExchange::Result result =
        key_exchange_impl_.ProcessMessage(spake_message);
    switch (result) {
      case P224EncryptedKeyExchange::kResultPending:
        pending_messages_.push(key_exchange_impl_.GetMessage());
        break;

      case P224EncryptedKeyExchange::kResultFailed:
        state_ = REJECTED;
        return;

      case P224EncryptedKeyExchange::kResultSuccess:
        auth_key_ = key_exchange_impl_.GetKey();
        state_ = ACCEPTED;
        return;
    }
  }

  state_ = MESSAGE_READY;
}

buzz::XmlElement* V2Authenticator::GetNextMessage() {
  DCHECK_EQ(state(), MESSAGE_READY);

  buzz::XmlElement* message = CreateEmptyAuthenticatorMessage();

  DCHECK(!pending_messages_.empty());
  while (!pending_messages_.empty()) {
    const std::string& spake_message = pending_messages_.front();
    std::string base64_message;
    if (!base::Base64Encode(spake_message, &base64_message)) {
      LOG(DFATAL) << "Cannot perform base64 encode on certificate";
      continue;
    }

    buzz::XmlElement* eke_tag = new buzz::XmlElement(kEkeTag);
    eke_tag->SetBodyText(base64_message);
    message->AddElement(eke_tag);

    pending_messages_.pop();
  }

  if (!local_cert_.empty() && !certificate_sent_) {
    buzz::XmlElement* certificate_tag = new buzz::XmlElement(kCertificateTag);
    std::string base64_cert;
    if (!base::Base64Encode(local_cert_, &base64_cert)) {
      LOG(DFATAL) << "Cannot perform base64 encode on certificate";
    }
    certificate_tag->SetBodyText(base64_cert);
    message->AddElement(certificate_tag);
  }

  if (state_ != ACCEPTED) {
    state_ = WAITING_MESSAGE;
  }
  return message;
}

ChannelAuthenticator* V2Authenticator::CreateChannelAuthenticator() const {
  DCHECK_EQ(state(), ACCEPTED);
  CHECK(!auth_key_.empty());

  if (is_host_side()) {
    return SslHmacChannelAuthenticator::CreateForHost(
        local_cert_, local_private_key_.get(), auth_key_);
  } else {
    return SslHmacChannelAuthenticator::CreateForClient(
        remote_cert_, auth_key_);
  }
}

bool V2Authenticator::is_host_side() const {
  return local_private_key_.get() != NULL;
}

V2HostAuthenticatorFactory::V2HostAuthenticatorFactory(
    const std::string& local_cert,
    const crypto::RSAPrivateKey* local_private_key,
    const std::string& shared_secret)
    : local_cert_(local_cert),
      local_private_key_(local_private_key->Copy()),
      shared_secret_(shared_secret) {
  CHECK(local_private_key_.get());
}

V2HostAuthenticatorFactory::~V2HostAuthenticatorFactory() {
}

Authenticator* V2HostAuthenticatorFactory::CreateAuthenticator(
    const std::string& remote_jid,
    const buzz::XmlElement* first_message) {
  if (!V2Authenticator::IsEkeMessage(first_message))
    return NULL;
  return V2Authenticator::CreateForHost(
      local_cert_, local_private_key_.get(), shared_secret_);
}

}  // namespace protocol
}  // namespace remoting
