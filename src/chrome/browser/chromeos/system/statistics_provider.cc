// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/system/statistics_provider.h"

#include "base/bind.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/synchronization/waitable_event.h"
#include "base/task.h"
#include "base/time.h"
#include "chrome/browser/chromeos/system/name_value_pairs_parser.h"
#include "chrome/browser/chromeos/system/runtime_environment.h"
#include "chrome/common/chrome_version_info.h"
#include "content/public/browser/browser_thread.h"

using content::BrowserThread;

namespace chromeos {
namespace system {
namespace {

// The tools used here should be specified as absolute paths. The
// existence of the tools is checked in GetSingleValueFromTool().

// The system command that returns the hardware class.
const char kHardwareClassKey[] = "hardware_class";
const char* kHardwareClassTool[] = { "/usr/bin/crossystem", "hwid" };
const char kUnknownHardwareClass[] = "unknown";

// Command to get machine hardware info and key/value delimiters.
// /tmp/machine-info is generated by platform/init/chromeos_startup.
const char kMachineHardwareInfoFile[] = "/tmp/machine-info";
const char kMachineHardwareInfoEq[] = "=";
const char kMachineHardwareInfoDelim[] = " \n";

// Command to get machine OS info and key/value delimiters.
const char kMachineOSInfoFile[] = "/etc/lsb-release";
const char kMachineOSInfoEq[] = "=";
const char kMachineOSInfoDelim[] = "\n";

// Command to get VPD info and key/value delimiters.
const char kVpdFile[] = "/var/log/vpd_2.0.txt";
const char kVpdEq[] = "=";
const char kVpdDelim[] = "\n";

// Timeout that we should wait for statistics to get loaded
const int kTimeoutSecs = 3;

}  // namespace

// The StatisticsProvider implementation used in production.
class StatisticsProviderImpl : public StatisticsProvider {
 public:
  // StatisticsProvider implementation:
  virtual bool GetMachineStatistic(const std::string& name,
                                   std::string* result);

  static StatisticsProviderImpl* GetInstance();

 private:
  friend struct DefaultSingletonTraits<StatisticsProviderImpl>;

  StatisticsProviderImpl();

  // Starts loading the machine statistcs.
  void StartLoadingMachineStatistics();

  // Loads the machine statistcs by examining the system.
  void LoadMachineStatistics();

  NameValuePairsParser::NameValueMap machine_info_;
  base::WaitableEvent on_statistics_loaded_;

  DISALLOW_COPY_AND_ASSIGN(StatisticsProviderImpl);
};

bool StatisticsProviderImpl::GetMachineStatistic(
    const std::string& name, std::string* result) {
  VLOG(1) << "Statistic is requested for " << name;
  // Block if the statistics are not loaded yet. Per LOG(WARNING) below,
  // the statistics are loaded before requested as of now. For regular
  // sessions (i.e. not OOBE), statistics are first requested when the
  // user is logging in so we have plenty of time to load the data
  // beforehand.
  //
  // If you see the warning appeared for regular sessions, it probably
  // means that there is new client code that uses the statistics in the
  // very early stage of the browser startup. The statistic name should be
  // helpful to identify the caller.
  if (!on_statistics_loaded_.IsSignaled()) {
    LOG(WARNING) << "Waiting to load statistics. Requested statistic: "
                 << name;
    on_statistics_loaded_.TimedWait(base::TimeDelta::FromSeconds(kTimeoutSecs));
  }

  NameValuePairsParser::NameValueMap::iterator iter = machine_info_.find(name);
  if (iter != machine_info_.end()) {
    *result = iter->second;
    return true;
  }
  return false;
}

// manual_reset needs to be true, as we want to keep the signaled state.
StatisticsProviderImpl::StatisticsProviderImpl()
    : on_statistics_loaded_(true  /* manual_reset */,
                            false /* initially_signaled */) {
  StartLoadingMachineStatistics();
}

void StatisticsProviderImpl::StartLoadingMachineStatistics() {
  VLOG(1) << "Started loading statistics";
  CHECK(BrowserThread::IsMessageLoopValid(BrowserThread::FILE))
      << "StatisticsProvider must not be used before FILE thread is created";
  BrowserThread::PostTask(
      BrowserThread::FILE,
      FROM_HERE,
      base::Bind(&StatisticsProviderImpl::LoadMachineStatistics,
                 base::Unretained(this)));
}

void StatisticsProviderImpl::LoadMachineStatistics() {
  NameValuePairsParser parser(&machine_info_);
  if (!parser.GetSingleValueFromTool(arraysize(kHardwareClassTool),
                                     kHardwareClassTool,
                                     kHardwareClassKey)) {
    // Use kUnknownHardwareClass if the hardware class command fails.
    parser.AddNameValuePair(kHardwareClassKey, kUnknownHardwareClass);
  }
  parser.GetNameValuePairsFromFile(FilePath(kMachineHardwareInfoFile),
                                   kMachineHardwareInfoEq,
                                   kMachineHardwareInfoDelim);
  parser.GetNameValuePairsFromFile(FilePath(kMachineOSInfoFile),
                                   kMachineOSInfoEq,
                                   kMachineOSInfoDelim);
  parser.GetNameValuePairsFromFile(FilePath(kVpdFile), kVpdEq, kVpdDelim);

#if defined(GOOGLE_CHROME_BUILD)
  // TODO(kochi): This is for providing a channel information to
  // chrome::VersionInfo::GetChannel()/GetVersionStringModifier(),
  // but this is still late for some early customers such as
  // prerender::ConfigurePrefetchAndPrerender() and
  // ThreadWatcherList::ParseCommandLine().
  // See http://crbug.com/107333 .
  const char kChromeOSReleaseTrack[] = "CHROMEOS_RELEASE_TRACK";
  std::string channel;
  if (GetMachineStatistic(kChromeOSReleaseTrack, &channel)) {
      chrome::VersionInfo::SetChannel(channel);
  }
#endif

  // Finished loading the statistics.
  on_statistics_loaded_.Signal();
  VLOG(1) << "Finished loading statistics";
}

StatisticsProviderImpl* StatisticsProviderImpl::GetInstance() {
  return Singleton<StatisticsProviderImpl,
                   DefaultSingletonTraits<StatisticsProviderImpl> >::get();
}

// The stub StatisticsProvider implementation used on Linux desktop.
class StatisticsProviderStubImpl : public StatisticsProvider {
 public:
  // StatisticsProvider implementation:
  virtual bool GetMachineStatistic(const std::string& name,
                                   std::string* result) {
    return false;
  }

  static StatisticsProviderStubImpl* GetInstance() {
    return Singleton<StatisticsProviderStubImpl,
        DefaultSingletonTraits<StatisticsProviderStubImpl> >::get();
  }

 private:
  friend struct DefaultSingletonTraits<StatisticsProviderStubImpl>;

  StatisticsProviderStubImpl() {
  }

  DISALLOW_COPY_AND_ASSIGN(StatisticsProviderStubImpl);
};

StatisticsProvider* StatisticsProvider::GetInstance() {
  if (system::runtime_environment::IsRunningOnChromeOS()) {
    return StatisticsProviderImpl::GetInstance();
  } else {
    return StatisticsProviderStubImpl::GetInstance();
  }
}

}  // namespace system
}  // namespace chromeos
