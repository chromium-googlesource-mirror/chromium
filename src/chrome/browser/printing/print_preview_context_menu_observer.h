// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PRINTING_PRINT_PREVIEW_CONTEXT_MENU_OBSERVER_H_
#define CHROME_BROWSER_PRINTING_PRINT_PREVIEW_CONTEXT_MENU_OBSERVER_H_
#pragma once

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "chrome/browser/tab_contents/render_view_context_menu_observer.h"

class TabContentsWrapper;

class PrintPreviewContextMenuObserver : public RenderViewContextMenuObserver {
 public:
  explicit PrintPreviewContextMenuObserver(TabContentsWrapper* tab);
  virtual ~PrintPreviewContextMenuObserver();

  // RenderViewContextMenuObserver implementation.
  virtual bool IsCommandIdSupported(int command_id) OVERRIDE;
  virtual bool IsCommandIdEnabled(int command_id) OVERRIDE;

 private:
  bool IsPrintPreviewTab();

  TabContentsWrapper* tab_;

  DISALLOW_COPY_AND_ASSIGN(PrintPreviewContextMenuObserver);
};

#endif  // CHROME_BROWSER_PRINTING_PRINT_PREVIEW_CONTEXT_MENU_OBSERVER_H_
