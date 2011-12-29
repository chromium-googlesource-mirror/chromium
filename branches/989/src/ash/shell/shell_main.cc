// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/launcher/launcher_types.h"
#include "ash/shell.h"
#include "ash/shell_delegate.h"
#include "ash/shell_factory.h"
#include "ash/shell/example_factory.h"
#include "ash/shell/toplevel_window.h"
#include "ash/wm/window_util.h"
#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"
#include "ui/aura/root_window.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/compositor/test/compositor_test_support.h"
#include "ui/views/test/test_views_delegate.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"

namespace {

class ShellDelegateImpl : public ash::ShellDelegate {
 public:
  ShellDelegateImpl() {
  }

  virtual void CreateNewWindow() OVERRIDE {
    ash::shell::ToplevelWindow::CreateParams create_params;
    create_params.can_resize = true;
    create_params.can_maximize = true;
    ash::shell::ToplevelWindow::CreateToplevelWindow(create_params);
  }

  virtual views::Widget* CreateStatusArea() OVERRIDE {
    return ash::internal::CreateStatusArea();
  }

  virtual void RequestAppListWidget(
      const gfx::Rect& bounds,
      const SetWidgetCallback& callback) OVERRIDE {
    // TODO(xiyuan): Clean this up.
    // The code below here is because we don't want to use
    // --aura-views-applist. This function is deprecated and all code
    // here will be removed when we clean it up.
    ash::shell::CreateAppList(bounds, callback);
  }

  virtual void BuildAppListModel(ash::AppListModel* model) {
    ash::shell::BuildAppListModel(model);
  }

  virtual ash::AppListViewDelegate* CreateAppListViewDelegate() {
    return ash::shell::CreateAppListViewDelegate();
  }

  virtual void LauncherItemClicked(
      const ash::LauncherItem& item) OVERRIDE {
    ash::ActivateWindow(item.window);
  }

  virtual bool ConfigureLauncherItem(ash::LauncherItem* item) OVERRIDE {
    static int image_count = 0;
    item->tab_images.resize(image_count + 1);
    for (int i = 0; i < image_count + 1; ++i) {
      item->tab_images[i].image.setConfig(SkBitmap::kARGB_8888_Config, 16, 16);
      item->tab_images[i].image.allocPixels();
      item->tab_images[i].image.eraseARGB(255,
                                          i == 0 ? 255 : 0,
                                          i == 1 ? 255 : 0,
                                          i == 2 ? 255 : 0);
    }
    image_count = (image_count + 1) % 3;
    return true;  // Makes the entry show up in the launcher.
  }
};

}  // namespace

namespace ash {
namespace shell {

void InitWindowTypeLauncher();

}  // namespace shell
}  // namespace ash

int main(int argc, char** argv) {
  CommandLine::Init(argc, argv);

  // The exit manager is in charge of calling the dtors of singleton objects.
  base::AtExitManager exit_manager;

  ui::RegisterPathProvider();
  icu_util::Initialize();
  ResourceBundle::InitSharedInstance("en-US");

  // Create the message-loop here before creating the root window.
  MessageLoop message_loop(MessageLoop::TYPE_UI);
  ui::CompositorTestSupport::Initialize();

  // A ViewsDelegate is required.
  if (!views::ViewsDelegate::views_delegate)
    views::ViewsDelegate::views_delegate = new views::TestViewsDelegate;

  ash::Shell::CreateInstance(new ShellDelegateImpl);

  ash::shell::InitWindowTypeLauncher();

  aura::RootWindow::GetInstance()->Run();

  ash::Shell::DeleteInstance();

  aura::RootWindow::DeleteInstance();

  ui::CompositorTestSupport::Terminate();

  return 0;
}
