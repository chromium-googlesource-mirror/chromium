// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_BROWSER_NATIVE_WEB_KEYBOARD_EVENT_H_
#define CONTENT_PUBLIC_BROWSER_NATIVE_WEB_KEYBOARD_EVENT_H_
#pragma once

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "build/build_config.h"
#include "content/common/content_export.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebInputEvent.h"
#include "ui/base/events.h"
#include "ui/gfx/native_widget_types.h"

// Owns a platform specific event; used to pass own and pass event through
// platform independent code.
struct CONTENT_EXPORT NativeWebKeyboardEvent :
  NON_EXPORTED_BASE(public WebKit::WebKeyboardEvent) {
  NativeWebKeyboardEvent();

  explicit NativeWebKeyboardEvent(gfx::NativeEvent native_event);
#if defined(USE_AURA)
  NativeWebKeyboardEvent(ui::EventType type,
                         bool is_char,
                         wchar_t character,
                         int state,
                         double time_stamp_seconds);
#elif defined(OS_MACOSX) || defined(TOOLKIT_USES_GTK)
  // TODO(suzhe): Limit these constructors to Linux native Gtk port.
  // For Linux Views port, after using RenderWidgetHostViewViews to replace
  // RenderWidgetHostViewGtk, we can use constructors for TOOLKIT_VIEWS defined
  // below.
  NativeWebKeyboardEvent(wchar_t character,
                         int state,
                         double time_stamp_seconds);
#endif

  NativeWebKeyboardEvent(const NativeWebKeyboardEvent& event);
  ~NativeWebKeyboardEvent();

  NativeWebKeyboardEvent& operator=(const NativeWebKeyboardEvent& event);

  gfx::NativeEvent os_event;

  // True if the browser should ignore this event if it's not handled by the
  // renderer. This happens for RawKeyDown events that are created while IME is
  // active and is necessary to prevent backspace from doing "history back" if
  // it is hit in ime mode.
  // Currently, it's only used by Linux and Mac ports.
  bool skip_in_browser;

#if defined(TOOLKIT_USES_GTK)
  // True if the key event matches an edit command. In order to ensure the edit
  // command always work in web page, the browser should not pre-handle this key
  // event as a reserved accelerator. See http://crbug.com/54573
  bool match_edit_command;
#endif
};

#endif  // CONTENT_PUBLIC_BROWSER_NATIVE_WEB_KEYBOARD_EVENT_H_
