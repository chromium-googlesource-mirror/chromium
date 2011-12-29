// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_BROWSER_DIALOGS_H_
#define CHROME_BROWSER_UI_BROWSER_DIALOGS_H_
#pragma once

#include "chrome/browser/ui/dialog_style.h"
#include "ipc/ipc_message.h"
#include "ui/gfx/native_widget_types.h"

class Browser;
class Extension;
class HtmlDialogUIDelegate;
class Profile;
class SkBitmap;
class TabContents;
class TabContentsWrapper;
class TabModalConfirmDialogDelegate;
class TemplateURL;

namespace content {
class WebContents;
}

namespace browser {

#if defined(IPC_MESSAGE_LOG_ENABLED)

// The dialog is a singleton. If the dialog is already opened, it won't do
// anything, so you can just blindly call this function all you want.
// This is Called from chrome/browser/browser_about_handler.cc
void ShowAboutIPCDialog();

#endif  // IPC_MESSAGE_LOG_ENABLED

// Creates and shows an HTML dialog with the given delegate and profile.
// The window is automatically destroyed when it is closed.
// Returns the created window.
//
// Make sure to use the returned window only when you know it is safe
// to do so, i.e. before OnDialogClosed() is called on the delegate.
gfx::NativeWindow ShowHtmlDialog(gfx::NativeWindow parent, Profile* profile,
                                 HtmlDialogUIDelegate* delegate,
                                 DialogStyle style);

// Creates the ExtensionInstalledBubble and schedules it to be shown once
// the extension has loaded. |extension| is the installed extension. |browser|
// is the browser window which will host the bubble. |icon| is the install
// icon of the extension.
void ShowExtensionInstalledBubble(const Extension* extension,
                                  Browser* browser,
                                  const SkBitmap& icon,
                                  Profile* profile);

// Shows or hide the hung renderer dialog for the given WebContents.
// We need to pass the WebContents to the dialog, because multiple tabs can hang
// and it needs to keep track of which tabs are currently hung.
void ShowHungRendererDialog(content::WebContents* contents);
void HideHungRendererDialog(content::WebContents* contents);

// Native implementations of hung renderer dialogs.
void ShowNativeHungRendererDialog(content::WebContents* contents);
void HideNativeHungRendererDialog(content::WebContents* contents);

// Show the edit search engine dialog.
void ConfirmAddSearchProvider(const TemplateURL* template_url,
                              Profile* profile);

// Shows a tab-modal dialog box.
void ShowTabModalConfirmDialog(TabModalConfirmDialogDelegate* delegate,
                               TabContentsWrapper* wrapper);

}  // namespace browser

#endif  // CHROME_BROWSER_UI_BROWSER_DIALOGS_H_
