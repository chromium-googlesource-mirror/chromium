// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/watchdog.h"

#include "base/compiler_specific.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/threading/platform_thread.h"

namespace base {

namespace {

// When the debugger breaks (when we alarm), all the other alarms that are
// armed will expire (also alarm).  To diminish this effect, we track any
// delay due to debugger breaks, and we *try* to adjust the effective start
// time of other alarms to step past the debugging break.
// Without this safety net, any alarm will typically trigger a host of follow
// on alarms from callers that specify old times.

// Lock for access of static data...
LazyInstance<Lock, LeakyLazyInstanceTraits<Lock> > g_static_lock =
    LAZY_INSTANCE_INITIALIZER;

// When did we last alarm and get stuck (for a while) in a debugger?
TimeTicks g_last_debugged_alarm_time;

// How long did we sit on a break in the debugger?
TimeDelta g_last_debugged_alarm_delay;

}  // namespace

// Start thread running in a Disarmed state.
Watchdog::Watchdog(const TimeDelta& duration,
                   const std::string& thread_watched_name,
                   bool enabled)
  : init_successful_(false),
    lock_(),
    condition_variable_(&lock_),
    state_(DISARMED),
    duration_(duration),
    thread_watched_name_(thread_watched_name),
    ALLOW_THIS_IN_INITIALIZER_LIST(delegate_(this)) {
  if (!enabled)
    return;  // Don't start thread, or doing anything really.
  init_successful_ = PlatformThread::Create(0,  // Default stack size.
                                            &delegate_,
                                            &handle_);
  DCHECK(init_successful_);
}

// Notify watchdog thread, and wait for it to finish up.
Watchdog::~Watchdog() {
  if (!init_successful_)
    return;
  {
    AutoLock lock(lock_);
    state_ = SHUTDOWN;
  }
  condition_variable_.Signal();
  PlatformThread::Join(handle_);
}

void Watchdog::Arm() {
  ArmAtStartTime(TimeTicks::Now());
}

void Watchdog::ArmSomeTimeDeltaAgo(const TimeDelta& time_delta) {
  ArmAtStartTime(TimeTicks::Now() - time_delta);
}

// Start clock for watchdog.
void Watchdog::ArmAtStartTime(const TimeTicks start_time) {
  {
    AutoLock lock(lock_);
    start_time_ = start_time;
    state_ = ARMED;
  }
  // Force watchdog to wake up, and go to sleep with the timer ticking with the
  // proper duration.
  condition_variable_.Signal();
}

// Disable watchdog so that it won't do anything when time expires.
void Watchdog::Disarm() {
  AutoLock lock(lock_);
  state_ = DISARMED;
  // We don't need to signal, as the watchdog will eventually wake up, and it
  // will check its state and time, and act accordingly.
}

void Watchdog::Alarm() {
  DVLOG(1) << "Watchdog alarmed for " << thread_watched_name_;
}

//------------------------------------------------------------------------------
// Internal private methods that the watchdog thread uses.

void Watchdog::ThreadDelegate::ThreadMain() {
  SetThreadName();
  TimeDelta remaining_duration;
  while (1) {
    AutoLock lock(watchdog_->lock_);
    while (DISARMED == watchdog_->state_)
      watchdog_->condition_variable_.Wait();
    if (SHUTDOWN == watchdog_->state_)
      return;
    DCHECK(ARMED == watchdog_->state_);
    remaining_duration = watchdog_->duration_ -
        (TimeTicks::Now() - watchdog_->start_time_);
    if (remaining_duration.InMilliseconds() > 0) {
      // Spurios wake?  Timer drifts?  Go back to sleep for remaining time.
      watchdog_->condition_variable_.TimedWait(remaining_duration);
      continue;
    }
    // We overslept, so this seems like a real alarm.
    // Watch out for a user that stopped the debugger on a different alarm!
    {
      AutoLock static_lock(*g_static_lock.Pointer());
      if (g_last_debugged_alarm_time > watchdog_->start_time_) {
        // False alarm: we started our clock before the debugger break (last
        // alarm time).
        watchdog_->start_time_ += g_last_debugged_alarm_delay;
        if (g_last_debugged_alarm_time > watchdog_->start_time_)
          // Too many alarms must have taken place.
          watchdog_->state_ = DISARMED;
        continue;
      }
    }
    watchdog_->state_ = DISARMED;  // Only alarm at most once.
    TimeTicks last_alarm_time = TimeTicks::Now();
    {
      AutoUnlock lock(watchdog_->lock_);
      watchdog_->Alarm();  // Set a break point here to debug on alarms.
    }
    TimeDelta last_alarm_delay = TimeTicks::Now() - last_alarm_time;
    if (last_alarm_delay <= TimeDelta::FromMilliseconds(2))
      continue;
    // Ignore race of two alarms/breaks going off at roughly the same time.
    AutoLock static_lock(*g_static_lock.Pointer());
    // This was a real debugger break.
    g_last_debugged_alarm_time = last_alarm_time;
    g_last_debugged_alarm_delay = last_alarm_delay;
  }
}

void Watchdog::ThreadDelegate::SetThreadName() const {
  std::string name = watchdog_->thread_watched_name_ + " Watchdog";
  PlatformThread::SetName(name.c_str());
  DVLOG(1) << "Watchdog active: " << name;
}

// static
void Watchdog::ResetStaticData() {
  AutoLock lock(*g_static_lock.Pointer());
  g_last_debugged_alarm_time = TimeTicks();
  g_last_debugged_alarm_delay = TimeDelta();
}

}  // namespace base
