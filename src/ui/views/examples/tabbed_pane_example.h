// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_EXAMPLES_TABBED_PANE_EXAMPLE_H_
#define UI_VIEWS_EXAMPLES_TABBED_PANE_EXAMPLE_H_
#pragma once

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane_listener.h"
#include "ui/views/examples/example_base.h"

namespace views {
class TabbedPane;

namespace examples {

// A TabbedPane example tests adding/removing/selecting tabs.
class TabbedPaneExample : public ExampleBase,
                          public ButtonListener,
                          public TabbedPaneListener {
 public:
  TabbedPaneExample();
  virtual ~TabbedPaneExample();

  // ExampleBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

 private:
  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const Event& event) OVERRIDE;

  // TabbedPaneListener:
  virtual void TabSelectedAt(int index) OVERRIDE;

  // Print the status of the tab in the status area.
  void PrintStatus();

  void AddButton(const std::string& label);

  // The tabbed pane to be tested.
  TabbedPane* tabbed_pane_;

  // Control buttons to add, remove or select tabs.
  Button* add_;
  Button* add_at_;
  Button* remove_at_;
  Button* select_at_;

  DISALLOW_COPY_AND_ASSIGN(TabbedPaneExample);
};

}  // namespace examples
}  // namespace views

#endif  // UI_VIEWS_EXAMPLES_TABBED_PANE_EXAMPLE_H_
