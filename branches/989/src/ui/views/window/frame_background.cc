// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/window/frame_background.h"

#include "grit/ui_resources.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/base/theme_provider.h"
#include "ui/gfx/canvas.h"
#include "ui/views/view.h"

namespace views {

FrameBackground::FrameBackground()
  : frame_color_(0),
    theme_bitmap_(NULL),
    theme_overlay_bitmap_(NULL),
    top_area_height_(0),
    left_edge_(NULL),
    top_edge_(NULL),
    right_edge_(NULL),
    bottom_edge_(NULL),
    top_left_corner_(NULL),
    top_right_corner_(NULL),
    bottom_left_corner_(NULL),
    bottom_right_corner_(NULL),
    maximized_top_left_(NULL),
    maximized_top_right_(NULL),
    maximized_top_offset_(0),
    theme_background_y_(0) {
}

FrameBackground::~FrameBackground() {
}

void FrameBackground::SetSideImages(SkBitmap* left,
                                    SkBitmap* top,
                                    SkBitmap* right,
                                    SkBitmap* bottom) {
  left_edge_ = left;
  top_edge_ = top;
  right_edge_ = right;
  bottom_edge_ = bottom;
}

void FrameBackground::SetCornerImages(SkBitmap* top_left,
                                      SkBitmap* top_right,
                                      SkBitmap* bottom_left,
                                      SkBitmap* bottom_right) {
  top_left_corner_ = top_left;
  top_right_corner_ = top_right;
  bottom_left_corner_ = bottom_left;
  bottom_right_corner_ = bottom_right;
}

void FrameBackground::SetMaximizedCorners(SkBitmap* top_left,
                                          SkBitmap* top_right,
                                          int top_offset) {
  maximized_top_left_ = top_left;
  maximized_top_right_ = top_right;
  maximized_top_offset_ = top_offset;
}

void FrameBackground::PaintRestored(gfx::Canvas* canvas, View* view) const {
  // Fill with the frame color first so we have a constant background for
  // areas not covered by the theme image.
  PaintFrameColor(canvas, view);

  // Draw the theme frame.
  canvas->TileImageInt(*theme_bitmap_,
                       0, 0, view->width(), theme_bitmap_->height());

  // Draw the theme frame overlay, if available.
  if (theme_overlay_bitmap_)
    canvas->DrawBitmapInt(*theme_overlay_bitmap_, 0, 0);

  // Draw the top corners and edge, scaling the corner images down if they
  // are too big and relative to the vertical space available.
  int top_left_height =
      std::min(top_left_corner_->height(),
               view->height() - bottom_left_corner_->height());
  canvas->DrawBitmapInt(*top_left_corner_,
                        0, 0, top_left_corner_->width(), top_left_height,
                        0, 0, top_left_corner_->width(), top_left_height,
                        false);
  canvas->TileImageInt(*top_edge_,
      top_left_corner_->width(),
      0,
      view->width() - top_left_corner_->width() - top_right_corner_->width(),
      top_edge_->height());
  int top_right_height =
      std::min(top_right_corner_->height(),
               view->height() - bottom_right_corner_->height());
  canvas->DrawBitmapInt(*top_right_corner_,
                        0, 0,
                        top_right_corner_->width(), top_right_height,
                        view->width() - top_right_corner_->width(), 0,
                        top_right_corner_->width(), top_right_height,
                        false);

  // Right edge.
  int right_edge_height =
      view->height() - top_right_height - bottom_right_corner_->height();
  canvas->TileImageInt(*right_edge_,
                       view->width() - right_edge_->width(),
                       top_right_height,
                       right_edge_->width(),
                       right_edge_height);

  // Bottom corners and edge.
  canvas->DrawBitmapInt(*bottom_right_corner_,
                        view->width() - bottom_right_corner_->width(),
                        view->height() - bottom_right_corner_->height());
  canvas->TileImageInt(
      *bottom_edge_,
      bottom_left_corner_->width(),
      view->height() - bottom_edge_->height(),
      view->width() - bottom_left_corner_->width()
          - bottom_right_corner_->width(),
      bottom_edge_->height());
  canvas->DrawBitmapInt(*bottom_left_corner_, 0,
                        view->height() - bottom_left_corner_->height());

  // Left edge.
  int left_edge_height =
      view->height() - top_left_height - bottom_left_corner_->height();
  canvas->TileImageInt(*left_edge_,
                       0, top_left_height,
                       left_edge_->width(), left_edge_height);
}

void FrameBackground::PaintMaximized(gfx::Canvas* canvas, View* view) const {
  // We will be painting from top_offset to top_offset + theme_frame_height. If
  // this is less than top_area_height_, we need to paint the frame color
  // to fill in the area beneath the image.
  // TODO(jamescook): I'm not sure this is correct, as it doesn't seem to fully
  // account for the top_offset, but this is how it worked before.
  int theme_frame_bottom = maximized_top_offset_ + theme_bitmap_->height();
  if (top_area_height_ > theme_frame_bottom) {
    canvas->FillRect(frame_color_,
                     gfx::Rect(0, 0, view->width(), top_area_height_));
  }

  int left_offset = 0;
  int right_offset = 0;

  // Draw top-left and top-right corners to give maximized ChromeOS windows
  // a rounded appearance.
  if (maximized_top_left_ || maximized_top_right_) {
    // If we have either a left or right we should have both.
    DCHECK(maximized_top_left_ && maximized_top_right_);
    left_offset = maximized_top_left_->width();
    right_offset = maximized_top_right_->width();
    canvas->DrawBitmapInt(*maximized_top_left_, 0, 0);
    canvas->DrawBitmapInt(*maximized_top_right_,
                          view->width() - right_offset, 0);
  }

  // Draw the theme frame.
  canvas->TileImageInt(*theme_bitmap_,
                       left_offset,
                       maximized_top_offset_,
                       view->width() - (left_offset + right_offset),
                       theme_bitmap_->height());
  // Draw the theme frame overlay, if available.
  if (theme_overlay_bitmap_)
    canvas->DrawBitmapInt(*theme_overlay_bitmap_, 0, theme_background_y_);
}

void FrameBackground::PaintFrameColor(gfx::Canvas* canvas, View* view) const {
  // Fill the top area.
  canvas->FillRect(frame_color_,
                   gfx::Rect(0, 0, view->width(), top_area_height_));

  // If the window is very short, we're done.
  int remaining_height = view->height() - top_area_height_;
  if (remaining_height <= 0)
    return;

  // Fill down the sides.
  canvas->FillRect(frame_color_,
                   gfx::Rect(0, top_area_height_,
                             left_edge_->width(), remaining_height));
  canvas->FillRect(frame_color_,
                   gfx::Rect(view->width() - right_edge_->width(),
                             top_area_height_,
                             right_edge_->width(),
                             remaining_height));

  // If the window is very narrow, we're done.
  int center_width =
      view->width() - left_edge_->width() - right_edge_->width();
  if (center_width <= 0)
    return;

  // Fill the bottom area.
  canvas->FillRect(frame_color_,
                   gfx::Rect(left_edge_->width(),
                             view->height() - bottom_edge_->height(),
                             center_width,
                             bottom_edge_->height()));
}

}  // namespace views
