// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/controls/scrollbar/base_scroll_bar_thumb.h"

#include "ui/gfx/canvas.h"
#include "ui/gfx/rect.h"
#include "ui/views/controls/scrollbar/base_scroll_bar.h"

namespace {
// The distance the mouse can be dragged outside the bounds of the thumb during
// dragging before the scrollbar will snap back to its regular position.
static const int kScrollThumbDragOutSnap = 100;
}

namespace views {

BaseScrollBarThumb::BaseScrollBarThumb(BaseScrollBar* scroll_bar)
    : scroll_bar_(scroll_bar),
      drag_start_position_(-1),
      mouse_offset_(-1),
      state_(CustomButton::BS_NORMAL) {
}

BaseScrollBarThumb::~BaseScrollBarThumb() {
}

void BaseScrollBarThumb::SetSize(int size) {
  // Make sure the thumb is never sized smaller than its minimum possible
  // display size.
  gfx::Size prefsize = GetPreferredSize();
  size = std::max(size, scroll_bar_->IsHorizontal() ? prefsize.width() :
                                                      prefsize.height());
  gfx::Rect thumb_bounds = bounds();
  if (scroll_bar_->IsHorizontal()) {
    thumb_bounds.set_width(size);
  } else {
    thumb_bounds.set_height(size);
  }
  SetBoundsRect(thumb_bounds);
}

int BaseScrollBarThumb::GetSize() const {
  if (scroll_bar_->IsHorizontal())
    return width();
  return height();
}

void BaseScrollBarThumb::SetPosition(int position) {
  gfx::Rect thumb_bounds = bounds();
  gfx::Rect track_bounds = scroll_bar_->GetTrackBounds();
  if (scroll_bar_->IsHorizontal()) {
    thumb_bounds.set_x(track_bounds.x() + position);
  } else {
    thumb_bounds.set_y(track_bounds.y() + position);
  }
  SetBoundsRect(thumb_bounds);
}

int BaseScrollBarThumb::GetPosition() const {
  gfx::Rect track_bounds = scroll_bar_->GetTrackBounds();
  if (scroll_bar_->IsHorizontal())
    return x() - track_bounds.x();
  return y() - track_bounds.y();
}

void BaseScrollBarThumb::OnMouseEntered(const MouseEvent& event) {
  SetState(CustomButton::BS_HOT);
}

void BaseScrollBarThumb::OnMouseExited(const MouseEvent& event) {
  SetState(CustomButton::BS_NORMAL);
}

bool BaseScrollBarThumb::OnMousePressed(const MouseEvent& event) {
  mouse_offset_ = scroll_bar_->IsHorizontal() ? event.x() : event.y();
  drag_start_position_ = GetPosition();
  SetState(CustomButton::BS_PUSHED);
  return true;
}

bool BaseScrollBarThumb::OnMouseDragged(const MouseEvent& event) {
  // If the user moves the mouse more than |kScrollThumbDragOutSnap| outside
  // the bounds of the thumb, the scrollbar will snap the scroll back to the
  // point it was at before the drag began.
  if (scroll_bar_->IsHorizontal()) {
    if ((event.y() < y() - kScrollThumbDragOutSnap) ||
        (event.y() > (y() + height() + kScrollThumbDragOutSnap))) {
      scroll_bar_->ScrollToThumbPosition(drag_start_position_, false);
      return true;
    }
  } else {
    if ((event.x() < x() - kScrollThumbDragOutSnap) ||
        (event.x() > (x() + width() + kScrollThumbDragOutSnap))) {
      scroll_bar_->ScrollToThumbPosition(drag_start_position_, false);
      return true;
    }
  }
  if (scroll_bar_->IsHorizontal()) {
    int thumb_x = event.x() - mouse_offset_;
    scroll_bar_->ScrollToThumbPosition(GetPosition() + thumb_x, false);
  } else {
    int thumb_y = event.y() - mouse_offset_;
    scroll_bar_->ScrollToThumbPosition(GetPosition() + thumb_y, false);
  }
  return true;
}

void BaseScrollBarThumb::OnMouseReleased(const MouseEvent& event) {
  OnMouseCaptureLost();
}

void BaseScrollBarThumb::OnMouseCaptureLost() {
  SetState(CustomButton::BS_HOT);
}

CustomButton::ButtonState BaseScrollBarThumb::GetState() const {
  return state_;
}

void BaseScrollBarThumb::SetState(CustomButton::ButtonState state) {
  state_ = state;
  SchedulePaint();
}

}  // namespace views
