// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_CONTROLS_MENU_VIEW_MENU_DELEGATE_H_
#define UI_VIEWS_CONTROLS_MENU_VIEW_MENU_DELEGATE_H_
#pragma once

#include "ui/gfx/native_widget_types.h"

namespace gfx {
class Point;
}

namespace views {

class View;

////////////////////////////////////////////////////////////////////////////////
//
// ViewMenuDelegate
//
// An interface that allows a component to tell a View about a menu that it
// has constructed that the view can show (e.g. for MenuButton views, or as a
// context menu.)
//
////////////////////////////////////////////////////////////////////////////////
class ViewMenuDelegate {
 public:
  // Create and show a menu at the specified position. Source is the view the
  // ViewMenuDelegate was set on.
  virtual void RunMenu(View* source, const gfx::Point& pt) = 0;

 protected:
  virtual ~ViewMenuDelegate() {}
};

}  // namespace views

#endif  // UI_VIEWS_CONTROLS_MENU_VIEW_MENU_DELEGATE_H_
