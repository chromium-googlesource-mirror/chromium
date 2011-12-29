// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_EXAMPLES_TABLE_EXAMPLE_H_
#define UI_VIEWS_EXAMPLES_TABLE_EXAMPLE_H_
#pragma once

#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/base/models/table_model.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/table/table_view.h"
#include "ui/views/controls/table/table_view_observer.h"
#include "ui/views/examples/example_base.h"

namespace views {
class Checkbox;
class Event;
class TableView;

namespace examples {

class TableExample : public ExampleBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener {
 public:
  TableExample();
  virtual ~TableExample();

  // ExampleBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

  // ui::TableModel:
  virtual int RowCount() OVERRIDE;
  virtual string16 GetText(int row, int column_id) OVERRIDE;
  virtual SkBitmap GetIcon(int row) OVERRIDE;
  virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE;

  // TableViewObserver:
  virtual void OnSelectionChanged() OVERRIDE;
  virtual void OnDoubleClick() OVERRIDE;
  virtual void OnMiddleClick() OVERRIDE;
  virtual void OnKeyDown(ui::KeyboardCode virtual_keycode) OVERRIDE;
  virtual void OnTableViewDelete(TableView* table_view) OVERRIDE;
  virtual void OnTableView2Delete(TableView2* table_view) OVERRIDE;

  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const Event& event) OVERRIDE;

 private:
  // The table to be tested.
  TableView* table_;

  Checkbox* column1_visible_checkbox_;
  Checkbox* column2_visible_checkbox_;
  Checkbox* column3_visible_checkbox_;
  Checkbox* column4_visible_checkbox_;

  SkBitmap icon1_;
  SkBitmap icon2_;

  DISALLOW_COPY_AND_ASSIGN(TableExample);
};

}  // namespace examples
}  // namespace views

#endif  // UI_VIEWS_EXAMPLES_TABLE_EXAMPLE_H_
