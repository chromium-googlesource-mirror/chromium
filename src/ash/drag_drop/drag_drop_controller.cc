// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/drag_drop/drag_drop_controller.h"

#include "ash/drag_drop/drag_image_view.h"
#include "ash/shell.h"
#include "base/message_loop.h"
#include "ui/aura/client/drag_drop_delegate.h"
#include "ui/aura/root_window.h"
#include "ui/aura/window.h"
#include "ui/base/dragdrop/drag_drop_types.h"
#include "ui/base/dragdrop/os_exchange_data_provider_aura.h"
#include "ui/gfx/point.h"
#include "ui/gfx/rect.h"
#include "ui/views/widget/native_widget_aura.h"

namespace ash {
namespace internal {

using aura::RootWindow;

namespace {
const gfx::Point kDragDropWidgetOffset(0, 0);
}

////////////////////////////////////////////////////////////////////////////////
// DragDropController, public:

DragDropController::DragDropController()
    : aura::EventFilter(RootWindow::GetInstance()),
      drag_image_(NULL),
      drag_data_(NULL),
      drag_operation_(0),
      dragged_window_(NULL),
      drag_drop_in_progress_(false),
      should_block_during_drag_drop_(true) {
  Shell::GetInstance()->AddRootWindowEventFilter(this);
  aura::client::SetDragDropClient(this);
}

DragDropController::~DragDropController() {
  Shell::GetInstance()->RemoveRootWindowEventFilter(this);
  Cleanup();
}

int DragDropController::StartDragAndDrop(const ui::OSExchangeData& data,
                                          int operation) {
  DCHECK(!drag_drop_in_progress_);
  aura::Window* capture_window = RootWindow::GetInstance()->capture_window();
  if (capture_window)
    RootWindow::GetInstance()->ReleaseCapture(capture_window);
  drag_drop_in_progress_ = true;

  drag_data_ = &data;
  drag_operation_ = operation;
  gfx::Point location = RootWindow::GetInstance()->last_mouse_location();
  const ui::OSExchangeDataProviderAura& provider =
      static_cast<const ui::OSExchangeDataProviderAura&>(data.provider());

  drag_image_.reset(new DragImageView);
  drag_image_->SetImage(provider.drag_image());
  drag_image_->SetScreenBounds(gfx::Rect(location.Add(kDragDropWidgetOffset),
      drag_image_->GetPreferredSize()));
  drag_image_->SetWidgetVisible(true);

  dragged_window_ = NULL;

  if (should_block_during_drag_drop_) {
    MessageLoopForUI::current()->RunWithDispatcher(
        RootWindow::GetInstance()->GetDispatcher());
  }
  return drag_operation_;
}

void DragDropController::DragUpdate(aura::Window* target,
                                    const aura::MouseEvent& event) {
  aura::client::DragDropDelegate* delegate = NULL;
  if (target != dragged_window_) {
    if (dragged_window_ &&
        (delegate = aura::client::GetDragDropDelegate(dragged_window_))) {
      delegate->OnDragExited();
    }
    dragged_window_ = target;
    if ((delegate = aura::client::GetDragDropDelegate(dragged_window_))) {
      aura::DropTargetEvent e(*drag_data_, event.location(), drag_operation_);
      delegate->OnDragEntered(e);
    }
  } else {
    if ((delegate = aura::client::GetDragDropDelegate(dragged_window_))) {
      aura::DropTargetEvent e(*drag_data_, event.location(), drag_operation_);
      int op = delegate->OnDragUpdated(e);
       gfx::NativeCursor cursor = (op == ui::DragDropTypes::DRAG_NONE)?
           aura::kCursorMove : aura::kCursorHand;
       RootWindow::GetInstance()->SetCursor(cursor);
    }
  }

  DCHECK(drag_image_.get());
  if (drag_image_->visible()) {
    drag_image_->SetScreenPosition(RootWindow::GetInstance()->
        last_mouse_location().Add(kDragDropWidgetOffset));
  }
}

void DragDropController::Drop(aura::Window* target,
                              const aura::MouseEvent& event) {
  aura::client::DragDropDelegate* delegate = NULL;
  DCHECK(target == dragged_window_);
  if ((delegate = aura::client::GetDragDropDelegate(dragged_window_))) {
    aura::DropTargetEvent e(*drag_data_, event.location(), drag_operation_);
    drag_operation_ = delegate->OnPerformDrop(e);
    // TODO(varunjain): if drag_op is 0, do drag widget flying back animation
  }

  Cleanup();
  if (should_block_during_drag_drop_)
    MessageLoop::current()->Quit();
}

void DragDropController::DragCancel() {
  // TODO(varunjain): Do drag widget flying back animation
  Cleanup();
  drag_operation_ = 0;
  if (should_block_during_drag_drop_)
    MessageLoop::current()->Quit();
}

bool DragDropController::IsDragDropInProgress() {
  return drag_drop_in_progress_;
}

bool DragDropController::PreHandleKeyEvent(aura::Window* target,
                                           aura::KeyEvent* event) {
  return false;
}

bool DragDropController::PreHandleMouseEvent(aura::Window* target,
                                             aura::MouseEvent* event) {
  if (!drag_drop_in_progress_)
    return false;
  switch (event->type()) {
    case ui::ET_MOUSE_DRAGGED:
      DragUpdate(target, *event);
      break;
    case ui::ET_MOUSE_RELEASED:
      Drop(target, *event);
      break;
    case ui::ET_MOUSE_EXITED:
      DragCancel();
      break;
    default:
      NOTREACHED();
      break;
  }
  return true;
}

ui::TouchStatus DragDropController::PreHandleTouchEvent(
    aura::Window* target,
    aura::TouchEvent* event) {
  return ui::TOUCH_STATUS_UNKNOWN;
}

////////////////////////////////////////////////////////////////////////////////
// DragDropController, private:

void DragDropController::Cleanup() {
  drag_image_.reset();
  drag_data_ = NULL;
  drag_drop_in_progress_ = false;
}

}  // namespace internal
}  // namespace ash
