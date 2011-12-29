// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TAB_CONTENTS_BACKGROUND_CONTENTS_H_
#define CHROME_BROWSER_TAB_CONTENTS_BACKGROUND_CONTENTS_H_
#pragma once

#include <string>

#include "base/memory/scoped_ptr.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "webkit/glue/window_open_disposition.h"

class Profile;
class SiteInstance;

// This class consumes TabContents. It can host a renderer, but does not
// have any visible display.
class BackgroundContents : public content::WebContentsDelegate,
                           public content::WebContentsObserver,
                           public content::NotificationObserver {
 public:
  class Delegate {
   public:
    // Called by AddNewContents(). Asks the delegate to attach the opened
    // WebContents to a suitable container (e.g. browser) or to show it if it's
    // a popup window.
    virtual void AddWebContents(content::WebContents* new_contents,
                                WindowOpenDisposition disposition,
                                const gfx::Rect& initial_pos,
                                bool user_gesture) = 0;

   protected:
    virtual ~Delegate() {}
  };

  BackgroundContents(SiteInstance* site_instance,
                     int routing_id,
                     Delegate* delegate);
  virtual ~BackgroundContents();

  TabContents* tab_contents() { return tab_contents_.get(); }
  virtual const GURL& GetURL() const;

  // content::WebContentsDelegate implementation:
  virtual void CloseContents(content::WebContents* source) OVERRIDE;
  virtual bool ShouldSuppressDialogs() OVERRIDE;
  virtual void DidNavigateMainFramePostCommit(
      content::WebContents* tab) OVERRIDE;
  virtual void AddNewContents(content::WebContents* source,
                              content::WebContents* new_contents,
                              WindowOpenDisposition disposition,
                              const gfx::Rect& initial_pos,
                              bool user_gesture) OVERRIDE;

  // content::WebContentsObserver implementation:
  virtual void RenderViewGone(base::TerminationStatus status) OVERRIDE;

  // content::NotificationObserver
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

 protected:
  // Exposed for testing.
  BackgroundContents();

 private:
  // The delegate for this BackgroundContents.
  Delegate* delegate_;

  Profile* profile_;
  scoped_ptr<TabContents> tab_contents_;
  content::NotificationRegistrar registrar_;

  DISALLOW_COPY_AND_ASSIGN(BackgroundContents);
};

// This is the data sent out as the details with BACKGROUND_CONTENTS_OPENED.
struct BackgroundContentsOpenedDetails {
  // The BackgroundContents object that has just been opened.
  BackgroundContents* contents;

  // The name of the parent frame for these contents.
  const string16& frame_name;

  // The ID of the parent application (if any).
  const string16& application_id;
};

#endif  // CHROME_BROWSER_TAB_CONTENTS_BACKGROUND_CONTENTS_H_
