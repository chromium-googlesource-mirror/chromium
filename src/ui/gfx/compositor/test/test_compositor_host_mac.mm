// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/compositor/test/test_compositor_host.h"

#import <AppKit/NSApplication.h>
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>
#import <Foundation/NSAutoreleasePool.h>

#include "base/compiler_specific.h"
#include "ui/gfx/compositor/compositor.h"
#include "ui/gfx/rect.h"

// AcceleratedTestView provides an NSView class that delegates drawing to a
// ui::Compositor delegate, setting up the NSOpenGLContext as required.
@interface AcceleratedTestView : NSView {
  scoped_refptr<ui::Compositor> compositor_;
}
// Designated initializer.
-(id)init;
-(void)setCompositor:(scoped_refptr<ui::Compositor>)compositor;
@end

@implementation AcceleratedTestView
-(id)init {
  // The frame will be resized when reparented into the window's view hierarchy.
  self = [super initWithFrame:NSZeroRect];
  return self;
}

-(void)setCompositor:(scoped_refptr<ui::Compositor>)compositor {
  compositor_ = compositor;
}

- (void)drawRect:(NSRect)rect {
  DCHECK(compositor_) << "Drawing with no compositor set.";
  compositor_->Draw(false);
}
@end

namespace ui {

// Tests that use Objective-C memory semantics need to have a top-level
// NSAutoreleasePool set up and initialized prior to execution and drained upon
// exit.  The tests will leak otherwise.
class FoundationHost {
 protected:
  FoundationHost() {
    pool_ = [[NSAutoreleasePool alloc] init];
  }
  virtual ~FoundationHost() {
    [pool_ drain];
  }

 private:
  NSAutoreleasePool* pool_;
  DISALLOW_COPY_AND_ASSIGN(FoundationHost);
};

// Tests that use the AppKit framework need to have the NSApplication
// initialized prior to doing anything with display objects such as windows,
// views, or controls.
class AppKitHost : public FoundationHost {
 protected:
  AppKitHost() {
    [NSApplication sharedApplication];
  }
  virtual ~AppKitHost() {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(AppKitHost);
};

// TestCompositorHostMac provides a window surface and a coordinated compositor
// for use in the compositor unit tests.
class TestCompositorHostMac : public TestCompositorHost,
                              public CompositorDelegate,
                              public AppKitHost {
 public:
  TestCompositorHostMac(const gfx::Rect& bounds);
  virtual ~TestCompositorHostMac();

 private:
  // TestCompositorHost:
  virtual void Show() OVERRIDE;
  virtual ui::Compositor* GetCompositor() OVERRIDE;

  // CompositorDelegate:
  virtual void ScheduleDraw() OVERRIDE;

  gfx::Rect bounds_;
  scoped_refptr<ui::Compositor> compositor_;

  // Owned.  Released when window is closed.
  NSWindow* window_;

  DISALLOW_COPY_AND_ASSIGN(TestCompositorHostMac);
};

TestCompositorHostMac::TestCompositorHostMac(const gfx::Rect& bounds)
    : bounds_(bounds), window_(nil) {
}

TestCompositorHostMac::~TestCompositorHostMac() {
  [window_ orderOut:nil];
  [window_ close];
}

void TestCompositorHostMac::Show() {
  DCHECK(!window_);
  window_ = [[NSWindow alloc]
                initWithContentRect:NSMakeRect(bounds_.x(),
                                               bounds_.y(),
                                               bounds_.width(),
                                               bounds_.height())
                          styleMask:NSBorderlessWindowMask
                            backing:NSBackingStoreBuffered
                              defer:NO];
  AcceleratedTestView* view = [[[AcceleratedTestView alloc] init] autorelease];
  compositor_ = ui::Compositor::Create(this, view, bounds_.size());
  [view setCompositor:compositor_];
  [window_ setContentView:view];
  [window_ orderFront:nil];
}

ui::Compositor* TestCompositorHostMac::GetCompositor() {
  return compositor_;
}

void TestCompositorHostMac::ScheduleDraw() {
  if (!compositor_)
    return;

  // Force display now.
  [window_ display];
}

// static
TestCompositorHost* TestCompositorHost::Create(const gfx::Rect& bounds) {
  return new TestCompositorHostMac(bounds);
}

}  // namespace ui
