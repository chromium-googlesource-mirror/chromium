// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_WM_TOPLEVEL_LAYOUT_MANAGER_H_
#define ASH_WM_TOPLEVEL_LAYOUT_MANAGER_H_
#pragma once

#include <set>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ui/aura/layout_manager.h"
#include "ui/aura/window_observer.h"
#include "ash/ash_export.h"

namespace ash {
namespace internal {

class ShelfLayoutManager;

// ToplevelLayoutManager is the LayoutManager installed on a container that
// hosts what the shell considers to be top-level windows. It is used if the
// WorkspaceManager is not enabled. ToplevelLayoutManager listens for changes to
// kShowStateKey and resizes the window appropriately.
class ASH_EXPORT ToplevelLayoutManager : public aura::LayoutManager,
                                                public aura::WindowObserver {
 public:
  ToplevelLayoutManager();
  virtual ~ToplevelLayoutManager();

  void set_shelf(ShelfLayoutManager* shelf) { shelf_ = shelf; }

  // LayoutManager overrides:
  virtual void OnWindowResized() OVERRIDE;
  virtual void OnWindowAddedToLayout(aura::Window* child) OVERRIDE;
  virtual void OnWillRemoveWindowFromLayout(aura::Window* child) OVERRIDE;
  virtual void OnChildWindowVisibilityChanged(aura::Window* child,
                                              bool visibile) OVERRIDE;
  virtual void SetChildBounds(aura::Window* child,
                              const gfx::Rect& requested_bounds) OVERRIDE;

  // WindowObserver overrides:
  virtual void OnWindowPropertyChanged(aura::Window* window,
                                       const char* name,
                                       void* old) OVERRIDE;

 private:
  typedef std::set<aura::Window*> Windows;

  // Updates the visibility of the shelf based on if there are any full screen
  // windows.
  void UpdateShelfVisibility();

  // Set of windows we're listening to.
  Windows windows_;

  // May be NULL if we're not using a shelf.
  ShelfLayoutManager* shelf_;

  DISALLOW_COPY_AND_ASSIGN(ToplevelLayoutManager);
};

}  // namespace ash
}  // namespace internal

#endif  // ASH_WM_TOPLEVEL_LAYOUT_MANAGER_H_
