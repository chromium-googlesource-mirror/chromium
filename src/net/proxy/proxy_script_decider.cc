// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/proxy/proxy_script_decider.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/compiler_specific.h"
#include "base/format_macros.h"
#include "base/logging.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "net/base/net_log.h"
#include "net/base/net_errors.h"
#include "net/proxy/dhcp_proxy_script_fetcher.h"
#include "net/proxy/dhcp_proxy_script_fetcher_factory.h"
#include "net/proxy/proxy_script_fetcher.h"

namespace net {

namespace {
bool LooksLikePacScript(const string16& script) {
  // Note: this is only an approximation! It may not always work correctly,
  // however it is very likely that legitimate scripts have this exact string,
  // since they must minimally define a function of this name. Conversely, a
  // file not containing the string is not likely to be a PAC script.
  //
  // An exact test would have to load the script in a javascript evaluator.
  return script.find(ASCIIToUTF16("FindProxyForURL")) != string16::npos;
}
}

// This is the hard-coded location used by the DNS portion of web proxy
// auto-discovery.
//
// Note that we not use DNS devolution to find the WPAD host, since that could
// be dangerous should our top level domain registry  become out of date.
//
// Instead we directly resolve "wpad", and let the operating system apply the
// DNS suffix search paths. This is the same approach taken by Firefox, and
// compatibility hasn't been an issue.
//
// For more details, also check out this comment:
// http://code.google.com/p/chromium/issues/detail?id=18575#c20
static const char kWpadUrl[] = "http://wpad/wpad.dat";

ProxyScriptDecider::ProxyScriptDecider(
    ProxyScriptFetcher* proxy_script_fetcher,
    DhcpProxyScriptFetcher* dhcp_proxy_script_fetcher,
    NetLog* net_log)
    : proxy_script_fetcher_(proxy_script_fetcher),
      dhcp_proxy_script_fetcher_(dhcp_proxy_script_fetcher),
      current_pac_source_index_(0u),
      pac_mandatory_(false),
      next_state_(STATE_NONE),
      net_log_(BoundNetLog::Make(
          net_log, NetLog::SOURCE_PROXY_SCRIPT_DECIDER)),
      fetch_pac_bytes_(false) {
}

ProxyScriptDecider::~ProxyScriptDecider() {
  if (next_state_ != STATE_NONE)
    Cancel();
}

int ProxyScriptDecider::Start(
    const ProxyConfig& config, const base::TimeDelta wait_delay,
    bool fetch_pac_bytes, const CompletionCallback& callback) {
  DCHECK_EQ(STATE_NONE, next_state_);
  DCHECK(!callback.is_null());
  DCHECK(config.HasAutomaticSettings());

  net_log_.BeginEvent(NetLog::TYPE_PROXY_SCRIPT_DECIDER, NULL);

  fetch_pac_bytes_ = fetch_pac_bytes;

  // Save the |wait_delay| as a non-negative value.
  wait_delay_ = wait_delay;
  if (wait_delay_ < base::TimeDelta())
    wait_delay_ = base::TimeDelta();

  pac_mandatory_ = config.pac_mandatory();

  pac_sources_ = BuildPacSourcesFallbackList(config);
  DCHECK(!pac_sources_.empty());

  next_state_ = STATE_WAIT;

  int rv = DoLoop(OK);
  if (rv == ERR_IO_PENDING)
    callback_ = callback;
  else
    DidComplete();

  return rv;
}

const ProxyConfig& ProxyScriptDecider::effective_config() const {
  DCHECK_EQ(STATE_NONE, next_state_);
  return effective_config_;
}

// TODO(eroman): Return a const-pointer.
ProxyResolverScriptData* ProxyScriptDecider::script_data() const {
  DCHECK_EQ(STATE_NONE, next_state_);
  return script_data_.get();
}

// Initialize the fallback rules.
// (1) WPAD (DHCP).
// (2) WPAD (DNS).
// (3) Custom PAC URL.
ProxyScriptDecider::PacSourceList ProxyScriptDecider::
    BuildPacSourcesFallbackList(
    const ProxyConfig& config) const {
  PacSourceList pac_sources;
  if (config.auto_detect()) {
    pac_sources.push_back(PacSource(PacSource::WPAD_DHCP, GURL()));
    pac_sources.push_back(PacSource(PacSource::WPAD_DNS, GURL()));
  }
  if (config.has_pac_url())
    pac_sources.push_back(PacSource(PacSource::CUSTOM, config.pac_url()));
  return pac_sources;
}

void ProxyScriptDecider::OnIOCompletion(int result) {
  DCHECK_NE(STATE_NONE, next_state_);
  int rv = DoLoop(result);
  if (rv != ERR_IO_PENDING) {
    DidComplete();
    DoCallback(rv);
  }
}

int ProxyScriptDecider::DoLoop(int result) {
  DCHECK_NE(next_state_, STATE_NONE);
  int rv = result;
  do {
    State state = next_state_;
    next_state_ = STATE_NONE;
    switch (state) {
      case STATE_WAIT:
        DCHECK_EQ(OK, rv);
        rv = DoWait();
        break;
      case STATE_WAIT_COMPLETE:
        rv = DoWaitComplete(rv);
        break;
      case STATE_FETCH_PAC_SCRIPT:
        DCHECK_EQ(OK, rv);
        rv = DoFetchPacScript();
        break;
      case STATE_FETCH_PAC_SCRIPT_COMPLETE:
        rv = DoFetchPacScriptComplete(rv);
        break;
      case STATE_VERIFY_PAC_SCRIPT:
        DCHECK_EQ(OK, rv);
        rv = DoVerifyPacScript();
        break;
      case STATE_VERIFY_PAC_SCRIPT_COMPLETE:
        rv = DoVerifyPacScriptComplete(rv);
        break;
      default:
        NOTREACHED() << "bad state";
        rv = ERR_UNEXPECTED;
        break;
    }
  } while (rv != ERR_IO_PENDING && next_state_ != STATE_NONE);
  return rv;
}

void ProxyScriptDecider::DoCallback(int result) {
  DCHECK_NE(ERR_IO_PENDING, result);
  DCHECK(!callback_.is_null());
  callback_.Run(result);
}

int ProxyScriptDecider::DoWait() {
  next_state_ = STATE_WAIT_COMPLETE;

  // If no waiting is required, continue on to the next state.
  if (wait_delay_.ToInternalValue() == 0)
    return OK;

  // Otherwise wait the specified amount of time.
  wait_timer_.Start(FROM_HERE, wait_delay_, this,
                    &ProxyScriptDecider::OnWaitTimerFired);
  net_log_.BeginEvent(NetLog::TYPE_PROXY_SCRIPT_DECIDER_WAIT, NULL);
  return ERR_IO_PENDING;
}

int ProxyScriptDecider::DoWaitComplete(int result) {
  DCHECK_EQ(OK, result);
  if (wait_delay_.ToInternalValue() != 0) {
    net_log_.EndEventWithNetErrorCode(NetLog::TYPE_PROXY_SCRIPT_DECIDER_WAIT,
                                      result);
  }
  next_state_ = GetStartState();
  return OK;
}

int ProxyScriptDecider::DoFetchPacScript() {
  DCHECK(fetch_pac_bytes_);

  next_state_ = STATE_FETCH_PAC_SCRIPT_COMPLETE;

  const PacSource& pac_source = current_pac_source();

  GURL effective_pac_url;
  NetLogStringParameter* log_parameter =
      CreateNetLogParameterAndDetermineURL(pac_source, &effective_pac_url);

  net_log_.BeginEvent(
      NetLog::TYPE_PROXY_SCRIPT_DECIDER_FETCH_PAC_SCRIPT,
      make_scoped_refptr(log_parameter));

  if (pac_source.type == PacSource::WPAD_DHCP) {
    if (!dhcp_proxy_script_fetcher_) {
      net_log_.AddEvent(NetLog::TYPE_PROXY_SCRIPT_DECIDER_HAS_NO_FETCHER, NULL);
      return ERR_UNEXPECTED;
    }

    return dhcp_proxy_script_fetcher_->Fetch(
        &pac_script_, base::Bind(&ProxyScriptDecider::OnIOCompletion,
                                 base::Unretained(this)));
  }

  if (!proxy_script_fetcher_) {
    net_log_.AddEvent(NetLog::TYPE_PROXY_SCRIPT_DECIDER_HAS_NO_FETCHER, NULL);
    return ERR_UNEXPECTED;
  }

  return proxy_script_fetcher_->Fetch(
      effective_pac_url, &pac_script_,
      base::Bind(&ProxyScriptDecider::OnIOCompletion, base::Unretained(this)));
}

int ProxyScriptDecider::DoFetchPacScriptComplete(int result) {
  DCHECK(fetch_pac_bytes_);

  net_log_.EndEventWithNetErrorCode(
      NetLog::TYPE_PROXY_SCRIPT_DECIDER_FETCH_PAC_SCRIPT, result);
  if (result != OK)
    return TryToFallbackPacSource(result);

  next_state_ = STATE_VERIFY_PAC_SCRIPT;
  return result;
}

int ProxyScriptDecider::DoVerifyPacScript() {
  next_state_ = STATE_VERIFY_PAC_SCRIPT_COMPLETE;

  // This is just a heuristic. Ideally we would try to parse the script.
  if (fetch_pac_bytes_ && !LooksLikePacScript(pac_script_))
    return ERR_PAC_SCRIPT_FAILED;

  return OK;
}

int ProxyScriptDecider::DoVerifyPacScriptComplete(int result) {
  if (result != OK)
    return TryToFallbackPacSource(result);

  const PacSource& pac_source = current_pac_source();

  // Extract the current script data.
  if (fetch_pac_bytes_) {
    script_data_ = ProxyResolverScriptData::FromUTF16(pac_script_);
  } else {
    script_data_ = pac_source.type == PacSource::CUSTOM ?
        ProxyResolverScriptData::FromURL(pac_source.url) :
        ProxyResolverScriptData::ForAutoDetect();
  }

  // Let the caller know which automatic setting we ended up initializing the
  // resolver for (there may have been multiple fallbacks to choose from.)
  if (current_pac_source().type == PacSource::CUSTOM) {
    effective_config_ =
        ProxyConfig::CreateFromCustomPacURL(current_pac_source().url);
    effective_config_.set_pac_mandatory(pac_mandatory_);
  } else {
    if (fetch_pac_bytes_) {
      GURL auto_detected_url;

      switch (current_pac_source().type) {
        case PacSource::WPAD_DHCP:
          auto_detected_url = dhcp_proxy_script_fetcher_->GetPacURL();
          break;

        case PacSource::WPAD_DNS:
          auto_detected_url = GURL(kWpadUrl);
          break;

        default:
          NOTREACHED();
      }

      effective_config_ =
          ProxyConfig::CreateFromCustomPacURL(auto_detected_url);
    } else {
      // The resolver does its own resolution so we cannot know the
      // URL. Just do the best we can and state that the configuration
      // is to auto-detect proxy settings.
      effective_config_ = ProxyConfig::CreateAutoDetect();
    }
  }

  return OK;
}

int ProxyScriptDecider::TryToFallbackPacSource(int error) {
  DCHECK_LT(error, 0);

  if (current_pac_source_index_ + 1 >= pac_sources_.size()) {
    // Nothing left to fall back to.
    return error;
  }

  // Advance to next URL in our list.
  ++current_pac_source_index_;

  net_log_.AddEvent(
      NetLog::TYPE_PROXY_SCRIPT_DECIDER_FALLING_BACK_TO_NEXT_PAC_SOURCE, NULL);

  next_state_ = GetStartState();

  return OK;
}

ProxyScriptDecider::State ProxyScriptDecider::GetStartState() const {
  return fetch_pac_bytes_ ?  STATE_FETCH_PAC_SCRIPT : STATE_VERIFY_PAC_SCRIPT;
}

NetLogStringParameter* ProxyScriptDecider::CreateNetLogParameterAndDetermineURL(
    const PacSource& pac_source,
    GURL* effective_pac_url) {
  DCHECK(effective_pac_url);

  std::string source_field;
  switch (pac_source.type) {
    case PacSource::WPAD_DHCP:
      source_field = "WPAD DHCP";
      break;
    case PacSource::WPAD_DNS:
      *effective_pac_url = GURL(kWpadUrl);
      source_field = "WPAD DNS: ";
      source_field += effective_pac_url->possibly_invalid_spec();
      break;
    case PacSource::CUSTOM:
      *effective_pac_url = pac_source.url;
      source_field = "Custom PAC URL: ";
      source_field += effective_pac_url->possibly_invalid_spec();
      break;
  }
  return new NetLogStringParameter("source", source_field);
}

const ProxyScriptDecider::PacSource&
    ProxyScriptDecider::current_pac_source() const {
  DCHECK_LT(current_pac_source_index_, pac_sources_.size());
  return pac_sources_[current_pac_source_index_];
}

void ProxyScriptDecider::OnWaitTimerFired() {
  OnIOCompletion(OK);
}

void ProxyScriptDecider::DidComplete() {
  net_log_.EndEvent(NetLog::TYPE_PROXY_SCRIPT_DECIDER, NULL);
}

void ProxyScriptDecider::Cancel() {
  DCHECK_NE(STATE_NONE, next_state_);

  net_log_.AddEvent(NetLog::TYPE_CANCELLED, NULL);

  switch (next_state_) {
    case STATE_WAIT_COMPLETE:
      wait_timer_.Stop();
      break;
    case STATE_FETCH_PAC_SCRIPT_COMPLETE:
      proxy_script_fetcher_->Cancel();
      break;
    default:
      NOTREACHED();
      break;
  }

  // This is safe to call in any state.
  if (dhcp_proxy_script_fetcher_)
    dhcp_proxy_script_fetcher_->Cancel();

  DidComplete();
}

}  // namespace net
