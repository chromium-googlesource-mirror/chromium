// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_LOGIN_WEBUI_SCREEN_LOCKER_H_
#define CHROME_BROWSER_CHROMEOS_LOGIN_WEBUI_SCREEN_LOCKER_H_
#pragma once

#include <string>

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "chrome/browser/chromeos/login/lock_window.h"
#include "chrome/browser/chromeos/login/login_display.h"
#include "chrome/browser/chromeos/login/screen_locker_delegate.h"
#include "chrome/browser/chromeos/login/webui_login_view.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "ui/views/widget/widget.h"

namespace chromeos {

class ScreenLocker;
class WebUILoginDisplay;

namespace test {
class WebUIScreenLockerTester;
}

// This version of ScreenLockerDelegate displays a WebUI lock screen based on
// the Oobe account picker screen.
class WebUIScreenLocker : public WebUILoginView,
                          public LoginDisplay::Delegate,
                          public content::NotificationObserver,
                          public ScreenLockerDelegate,
                          public LockWindow::Observer {
 public:
  explicit WebUIScreenLocker(ScreenLocker* screen_locker);

  // ScreenLockerDelegate implementation:
  virtual void LockScreen(bool unlock_on_input) OVERRIDE;
  virtual void ScreenLockReady() OVERRIDE;
  virtual void OnAuthenticate() OVERRIDE;
  virtual void SetInputEnabled(bool enabled) OVERRIDE;
  virtual void SetSignoutEnabled(bool enabled) OVERRIDE;
  virtual void ShowErrorMessage(const string16& message,
                                bool sign_out_only) OVERRIDE;
  virtual void ShowCaptchaAndErrorMessage(const GURL& captcha_url,
                                          const string16& message) OVERRIDE;
  virtual void ClearErrors() OVERRIDE;

  // LoginDisplay::Delegate: implementation
  virtual void CreateAccount() OVERRIDE;
  virtual string16 GetConnectedNetworkName() OVERRIDE;
  virtual void FixCaptivePortal() OVERRIDE;
  virtual void SetDisplayEmail(const std::string& email) OVERRIDE;
  virtual void CompleteLogin(const std::string& username,
                             const std::string& password) OVERRIDE;
  virtual void Login(const std::string& username,
                     const std::string& password) OVERRIDE;
  virtual void LoginAsGuest() OVERRIDE;
  virtual void OnUserSelected(const std::string& username) OVERRIDE;
  virtual void OnStartEnterpriseEnrollment() OVERRIDE;

  // content::NotificationObserver implementation.
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  // LockWindow::Observer implementation.
  virtual void OnLockWindowReady() OVERRIDE;

  // Overridden from WebUILoginView.
  virtual views::Widget::InitParams::Type GetStatusAreaWidgetType() OVERRIDE;

 private:
  friend class test::WebUIScreenLockerTester;

  virtual ~WebUIScreenLocker();

  // The screen locker window.
  views::Widget* lock_window_;

  // Login UI implementation instance.
  scoped_ptr<WebUILoginDisplay> login_display_;

  // Used for user image changed notifications.
  content::NotificationRegistrar registrar_;

  // Tracks when the lock window is displayed and ready.
  bool lock_ready_;

  // Tracks when the WebUI finishes loading.
  bool webui_ready_;

  DISALLOW_COPY_AND_ASSIGN(WebUIScreenLocker);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_LOGIN_WEBUI_SCREEN_LOCKER_H_