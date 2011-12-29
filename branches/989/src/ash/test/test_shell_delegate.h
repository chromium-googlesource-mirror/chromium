// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_TEST_TEST_SHELL_DELEGATE_H_
#define ASH_TEST_TEST_SHELL_DELEGATE_H_
#pragma once

#include "ash/shell_delegate.h"
#include "base/compiler_specific.h"

namespace ash {
namespace test {

class TestShellDelegate : public ShellDelegate {
 public:
  TestShellDelegate();
  virtual ~TestShellDelegate();

  // Overridden from ShellDelegate:
  virtual void CreateNewWindow() OVERRIDE;
  virtual views::Widget* CreateStatusArea() OVERRIDE;
  virtual void RequestAppListWidget(
      const gfx::Rect& bounds,
      const SetWidgetCallback& callback) OVERRIDE;
  virtual void BuildAppListModel(AppListModel* model) OVERRIDE;
  virtual AppListViewDelegate* CreateAppListViewDelegate() OVERRIDE;
  virtual void LauncherItemClicked(const LauncherItem& item) OVERRIDE;
  virtual bool ConfigureLauncherItem(LauncherItem* item) OVERRIDE;
};

}  // namespace test
}  // namespace ash

#endif  // ASH_TEST_TEST_SHELL_DELEGATE_H_
