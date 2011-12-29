// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_TAB_MODAL_CONFIRM_DIALOG_DELEGATE_H_
#define CHROME_BROWSER_UI_TAB_MODAL_CONFIRM_DIALOG_DELEGATE_H_
#pragma once

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/string16.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"

class ConstrainedWindow;

namespace content {
class WebContents;
}

namespace gfx {
class Image;
}

// This class acts as the delegate for a simple tab-modal dialog confirming
// whether the user wants to execute a certain action.
class TabModalConfirmDialogDelegate : public content::NotificationObserver {
 public:
  explicit TabModalConfirmDialogDelegate(content::WebContents* web_contents);
  virtual ~TabModalConfirmDialogDelegate();

  void set_window(ConstrainedWindow* window) { window_ = window; }

  void Accept();
  void Cancel();

  // The title of the dialog. Note that the title is not shown on all platforms.
  virtual string16 GetTitle() = 0;
  virtual string16 GetMessage() = 0;

  // Icon to show for the dialog. If this method is not overridden, a default
  // icon (like the application icon) is shown.
  virtual gfx::Image* GetIcon();

  // Title for the accept and the cancel buttons.
  // The default implementation uses IDS_OK and IDS_CANCEL.
  virtual string16 GetAcceptButtonTitle();
  virtual string16 GetCancelButtonTitle();

  // GTK stock icon names for the accept and cancel buttons, respectively.
  // The icons are only used on GTK. If these methods are not overriden,
  // the buttons have no stock icons.
  virtual const char* GetAcceptButtonIcon();
  virtual const char* GetCancelButtonIcon();

 protected:
  ConstrainedWindow* window() { return window_; }

  // content::NotificationObserver implementation.
  // Watch for a new load or a closed tab and dismiss the dialog if they occur.
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  content::NotificationRegistrar registrar_;

 private:
  // Called when the user accepts or cancels the dialog, respectively.
  virtual void OnAccepted();
  virtual void OnCanceled();

  // Close the dialog.
  void CloseDialog();

  ConstrainedWindow* window_;
  // True iff we are in the process of closing, to avoid running callbacks
  // multiple times.
  bool closing_;

  DISALLOW_COPY_AND_ASSIGN(TabModalConfirmDialogDelegate);
};

#endif  // CHROME_BROWSER_UI_TAB_MODAL_CONFIRM_DIALOG_DELEGATE_H_
