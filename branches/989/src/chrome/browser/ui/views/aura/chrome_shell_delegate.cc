// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/aura/chrome_shell_delegate.h"

#include "ash/launcher/launcher_types.h"
#include "ash/wm/window_util.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/aura/app_list_window.h"
#include "chrome/browser/ui/views/aura/app_list/app_list_model_builder.h"
#include "chrome/browser/ui/views/aura/app_list/app_list_view_delegate.h"
#include "chrome/browser/ui/views/aura/status_area_host_aura.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/aura/window.h"

// static
ChromeShellDelegate* ChromeShellDelegate::instance_ = NULL;

ChromeShellDelegate::ChromeShellDelegate() {
  instance_ = this;
}

ChromeShellDelegate::~ChromeShellDelegate() {
  if (instance_ == this)
    instance_ = NULL;
}

StatusAreaView* ChromeShellDelegate::GetStatusArea() {
  return status_area_host_->GetStatusArea();
}

// static
bool ChromeShellDelegate::ShouldCreateLauncherItemForBrowser(
    Browser* browser,
    ash::LauncherItemType* type) {
  if (browser->type() == Browser::TYPE_TABBED) {
    *type = ash::TYPE_TABBED;
    return true;
  }
  if (browser->is_app()) {
    *type = ash::TYPE_APP;
    return true;
  }
  return false;
}

void ChromeShellDelegate::CreateNewWindow() {
  Browser* browser = Browser::Create(ProfileManager::GetDefaultProfile());
  browser->AddSelectedTabWithURL(GURL(), content::PAGE_TRANSITION_START_PAGE);
  browser->window()->Show();
}

views::Widget* ChromeShellDelegate::CreateStatusArea() {
  status_area_host_.reset(new StatusAreaHostAura());
  views::Widget* status_area_widget =
      status_area_host_.get()->CreateStatusArea();
  return status_area_widget;
}

void ChromeShellDelegate::RequestAppListWidget(
    const gfx::Rect& bounds,
    const SetWidgetCallback& callback) {
  // AppListWindow deletes itself when closed.
  new AppListWindow(bounds, callback);
}

void ChromeShellDelegate::BuildAppListModel(ash::AppListModel* model) {
  AppListModelBuilder builder(ProfileManager::GetDefaultProfile(),
                              model);
  builder.Build();
}

ash::AppListViewDelegate*
ChromeShellDelegate::CreateAppListViewDelegate() {
  // Shell will own the created delegate.
  return new AppListViewDelegate;
}

void ChromeShellDelegate::LauncherItemClicked(
    const ash::LauncherItem& item) {
  ash::ActivateWindow(item.window);
}

bool ChromeShellDelegate::ConfigureLauncherItem(
    ash::LauncherItem* item) {
  BrowserView* view = BrowserView::GetBrowserViewForNativeWindow(item->window);
  return view &&
      ShouldCreateLauncherItemForBrowser(view->browser(), &(item->type));
}
