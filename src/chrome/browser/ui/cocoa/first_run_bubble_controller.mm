// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "chrome/browser/ui/cocoa/first_run_bubble_controller.h"

#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "chrome/browser/search_engines/util.h"
#import "chrome/browser/ui/cocoa/l10n_util.h"
#import "chrome/browser/ui/cocoa/info_bubble_view.h"
#include "grit/generated_resources.h"
#include "ui/base/l10n/l10n_util.h"

@interface FirstRunBubbleController(Private)
- (id)initRelativeToView:(NSView*)view
                  offset:(NSPoint)offset
                 profile:(Profile*)profile;
- (void)closeIfNotKey;
@end

@implementation FirstRunBubbleController

+ (FirstRunBubbleController*) showForView:(NSView*)view
                                   offset:(NSPoint)offset
                                  profile:(Profile*)profile {
  // Autoreleases itself on bubble close.
  return [[FirstRunBubbleController alloc] initRelativeToView:view
                                                       offset:offset
                                                      profile:profile];
}

- (id)initRelativeToView:(NSView*)view
                  offset:(NSPoint)offset
                 profile:(Profile*)profile {
  if ((self = [super initWithWindowNibPath:@"FirstRunBubble"
                            relativeToView:view
                                    offset:offset])) {
    profile_ = profile;
    [self showWindow:nil];

    // On 10.5, the first run bubble sometimes does not disappear when clicking
    // the omnibox. This happens if the bubble never became key, due to it
    // showing up so early in the startup sequence. As a workaround, close it
    // automatically after a few seconds if it doesn't become key.
    // http://crbug.com/52726
    [self performSelector:@selector(closeIfNotKey) withObject:nil afterDelay:3];
  }
  return self;
}

- (void)awakeFromNib {
  DCHECK(header_);
  [header_ setStringValue:cocoa_l10n_util::ReplaceNSStringPlaceholders(
      [header_ stringValue], GetDefaultSearchEngineName(profile_), NULL)];

  // Adapt window size to bottom buttons. Do this before all other layouting.
  CGFloat dy = cocoa_l10n_util::VerticallyReflowGroup([[self bubble] subviews]);
  NSSize ds = NSMakeSize(0, dy);
  ds = [[self bubble] convertSize:ds toView:nil];

  NSRect frame = [[self window] frame];
  frame.origin.y -= ds.height;
  frame.size.height += ds.height;
  [[self window] setFrame:frame display:YES];
}

- (void)close {
  // If the window is closed before the timer is fired, cancel the timer, since
  // it retains the controller.
  [NSObject cancelPreviousPerformRequestsWithTarget:self
                                           selector:@selector(closeIfNotKey)
                                             object:nil];
  [super close];
}

- (void)closeIfNotKey {
  if (![[self window] isKeyWindow])
    [self close];
}

@end  // FirstRunBubbleController
