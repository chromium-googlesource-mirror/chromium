// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/sessions/restore_tab_helper.h"

#include "chrome/browser/ui/tab_contents/tab_contents_wrapper.h"
#include "chrome/common/extensions/extension_messages.h"
#include "content/browser/renderer_host/render_view_host.h"
#include "content/browser/tab_contents/tab_contents.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_types.h"

RestoreTabHelper::RestoreTabHelper(TabContents* contents)
    : content::WebContentsObserver(contents),
      contents_(contents) {
}

RestoreTabHelper::~RestoreTabHelper() {
}

void RestoreTabHelper::SetWindowID(const SessionID& id) {
  window_id_ = id;

  // TODO(mpcomplete): Maybe this notification should send out a TabContents.
  TabContentsWrapper* tab =
      TabContentsWrapper::GetCurrentWrapperForContents(contents_);
  if (tab) {
    content::NotificationService::current()->Notify(
        content::NOTIFICATION_TAB_PARENTED,
        content::Source<TabContentsWrapper>(tab),
        content::NotificationService::NoDetails());
  }

  // Extension code in the renderer holds the ID of the window that hosts it.
  // Notify it that the window ID changed.
  contents_->GetRenderViewHost()->Send(
          new ExtensionMsg_UpdateBrowserWindowId(
          contents_->GetRenderViewHost()->routing_id(), id.id()));
}

void RestoreTabHelper::RenderViewCreated(RenderViewHost* render_view_host) {
  render_view_host->Send(
      new ExtensionMsg_UpdateBrowserWindowId(render_view_host->routing_id(),
                                             window_id_.id()));
}
