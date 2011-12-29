// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_REPEAT_CONTROLLER_H_
#define UI_VIEWS_REPEAT_CONTROLLER_H_
#pragma once

#include "base/callback.h"
#include "base/timer.h"

namespace views {

///////////////////////////////////////////////////////////////////////////////
//
// RepeatController
//
//  An object that handles auto-repeating UI actions. There is a longer initial
//  delay after which point repeats become constant. Users provide a callback
//  that is notified when each repeat occurs so that they can perform the
//  associated action.
//
///////////////////////////////////////////////////////////////////////////////
class RepeatController {
 public:
  // The RepeatController takes ownership of this callback object.
  explicit RepeatController(const base::Closure& callback);
  virtual ~RepeatController();

  // Start repeating.
  void Start();

  // Stop repeating.
  void Stop();

 private:
  // Called when the timer expires.
  void Run();

  // The current timer.
  base::OneShotTimer<RepeatController> timer_;

  base::Closure callback_;

  DISALLOW_COPY_AND_ASSIGN(RepeatController);
};

}  // namespace views

#endif  // UI_VIEWS_REPEAT_CONTROLLER_H_
