// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_TABS_HOVER_TAB_SELECTOR_H_
#define CHROME_BROWSER_UI_TABS_HOVER_TAB_SELECTOR_H_
#pragma once

#include "base/memory/weak_ptr.h"

class TabStripModel;

// Helper class to perform "spring-loaded" tab transitions. Manages
// the lifecycle of delayed tab transition tasks.
class HoverTabSelector {
 public:
  explicit HoverTabSelector(TabStripModel* tab_strip_model);
  ~HoverTabSelector();

  // Begin a delayed tab transition to the tab at |index|. Only starts
  // the transition if the specified tab is not active and there isn't
  // already a transition to it scheduled. Cancels the pending transition
  // to any other tab, if there is one.
  void StartTabTransition(int index);

  // Cancel a pending tab transition. No-op if there is no pending transition.
  void CancelTabTransition();

 private:
  // Performs the tab transition.
  void PerformTabTransition();

  // Model of the tab strip on which this class operates.
  TabStripModel* tab_strip_model_;

  // The model index of the tab to transition to.
  int tab_transition_tab_index_;

  // Factory for creating tab transition tasks.
  base::WeakPtrFactory<HoverTabSelector> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(HoverTabSelector);
};

#endif  // CHROME_BROWSER_UI_TABS_HOVER_TAB_SELECTOR_H_

