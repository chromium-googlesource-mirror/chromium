// This file is a cut'n'paste from WebKit/WebCore/platform/gtk/CursorGtk.h,
// with slight modifications to fit within Chrome.  Its original
// copyright is below.

// This file intentionally doesn't have a header guard, since it is just
// data to be used within a C++ file.

/*
 * Copyright (C) 2001 Tim Copperfield <timecop@network.email.ne.jp>
 * Copyright (C) 2007 Christian Dywan <christian@twotoasts.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

/*
 These cursors are copied from Mozilla code:
  http://lxr.mozilla.org/mozilla1.8/source/widget/src/gtk2/nsGtkCursors.h
*/

/* MOZ_CURSOR_VERTICAL_TEXT */
static const char moz_vertical_text_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00,
  0x06, 0x60, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00,
  0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_vertical_text_mask_bits[] = {
  0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00,
  0xff, 0xff, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x0f, 0xf0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* MOZ_CURSOR_CONTEXT_MENU */
static const char moz_menu_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
  0x7c, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0xfc, 0xfd, 0x00, 0x00,
  0xfc, 0xff, 0x00, 0x00, 0x7c, 0x84, 0x00, 0x00, 0x6c, 0xfc, 0x00, 0x00,
  0xc4, 0x84, 0x00, 0x00, 0xc0, 0xfc, 0x00, 0x00, 0x80, 0x85, 0x00, 0x00,
  0x80, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_menu_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x1e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
  0xfe, 0x00, 0x00, 0x00, 0xfe, 0xfd, 0x00, 0x00, 0xfe, 0xff, 0x01, 0x00,
  0xfe, 0xff, 0x01, 0x00, 0xfe, 0xff, 0x01, 0x00, 0xfe, 0xfe, 0x01, 0x00,
  0xee, 0xff, 0x01, 0x00, 0xe4, 0xff, 0x01, 0x00, 0xc0, 0xff, 0x01, 0x00,
  0xc0, 0xff, 0x01, 0x00, 0x80, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* MOZ_CURSOR_COPY */
static const char moz_copy_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
  0x7c, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00,
  0xfc, 0x03, 0x00, 0x00, 0x7c, 0x30, 0x00, 0x00, 0x6c, 0x30, 0x00, 0x00,
  0xc4, 0xfc, 0x00, 0x00, 0xc0, 0xfc, 0x00, 0x00, 0x80, 0x31, 0x00, 0x00,
  0x80, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_copy_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x1e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
  0xfe, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x00, 0xfe, 0x03, 0x00, 0x00,
  0xfe, 0x37, 0x00, 0x00, 0xfe, 0x7b, 0x00, 0x00, 0xfe, 0xfc, 0x00, 0x00,
  0xee, 0xff, 0x01, 0x00, 0xe4, 0xff, 0x01, 0x00, 0xc0, 0xff, 0x00, 0x00,
  0xc0, 0x7b, 0x00, 0x00, 0x80, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* MOZ_CURSOR_ALIAS */
static const char moz_alias_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
  0x7c, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00,
  0xfc, 0x03, 0x00, 0x00, 0x7c, 0xf0, 0x00, 0x00, 0x6c, 0xe0, 0x00, 0x00,
  0xc4, 0xf0, 0x00, 0x00, 0xc0, 0xb0, 0x00, 0x00, 0x80, 0x19, 0x00, 0x00,
  0x80, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_alias_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x1e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
  0xfe, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x00, 0xfe, 0x03, 0x00, 0x00,
  0xfe, 0xf7, 0x00, 0x00, 0xfe, 0xfb, 0x01, 0x00, 0xfe, 0xf0, 0x01, 0x00,
  0xee, 0xf9, 0x01, 0x00, 0xe4, 0xf9, 0x01, 0x00, 0xc0, 0xbf, 0x00, 0x00,
  0xc0, 0x3f, 0x00, 0x00, 0x80, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* MOZ_CURSOR_ZOOM_IN */
static const char moz_zoom_in_bits[] = {
  0xf0, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00,
  0x62, 0x04, 0x00, 0x00, 0x61, 0x08, 0x00, 0x00, 0xf9, 0x09, 0x00, 0x00,
  0xf9, 0x09, 0x00, 0x00, 0x61, 0x08, 0x00, 0x00, 0x62, 0x04, 0x00, 0x00,
  0x02, 0x04, 0x00, 0x00, 0x0c, 0x0f, 0x00, 0x00, 0xf0, 0x1c, 0x00, 0x00,
  0x00, 0x38, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_zoom_in_mask_bits[] = {
  0xf0, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x00,
  0xfe, 0x07, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00,
  0xff, 0x0f, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x00,
  0xfe, 0x07, 0x00, 0x00, 0xfc, 0x0f, 0x00, 0x00, 0xf0, 0x1c, 0x00, 0x00,
  0x00, 0x38, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* MOZ_CURSOR_ZOOM_OUT */
static const char moz_zoom_out_bits[] = {
  0xf0, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00,
  0x02, 0x04, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0xf9, 0x09, 0x00, 0x00,
  0xf9, 0x09, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00,
  0x02, 0x04, 0x00, 0x00, 0x0c, 0x0f, 0x00, 0x00, 0xf0, 0x1c, 0x00, 0x00,
  0x00, 0x38, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_zoom_out_mask_bits[] = {
  0xf0, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x00,
  0xfe, 0x07, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00,
  0xff, 0x0f, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x00,
  0xfe, 0x07, 0x00, 0x00, 0xfc, 0x0f, 0x00, 0x00, 0xf0, 0x1c, 0x00, 0x00,
  0x00, 0x38, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* MOZ_CURSOR_HAND_GRAB */
static const char moz_hand_grab_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
  0x60, 0x39, 0x00, 0x00, 0x90, 0x49, 0x00, 0x00, 0x90, 0x49, 0x01, 0x00,
  0x20, 0xc9, 0x02, 0x00, 0x20, 0x49, 0x02, 0x00, 0x58, 0x40, 0x02, 0x00,
  0x64, 0x00, 0x02, 0x00, 0x44, 0x00, 0x01, 0x00, 0x08, 0x00, 0x01, 0x00,
  0x10, 0x00, 0x01, 0x00, 0x10, 0x80, 0x00, 0x00, 0x20, 0x80, 0x00, 0x00,
  0x40, 0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_hand_grab_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x60, 0x3f, 0x00, 0x00,
  0xf0, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0x01, 0x00, 0xf8, 0xff, 0x03, 0x00,
  0xf0, 0xff, 0x07, 0x00, 0xf8, 0xff, 0x07, 0x00, 0xfc, 0xff, 0x07, 0x00,
  0xfe, 0xff, 0x07, 0x00, 0xfe, 0xff, 0x03, 0x00, 0xfc, 0xff, 0x03, 0x00,
  0xf8, 0xff, 0x03, 0x00, 0xf8, 0xff, 0x01, 0x00, 0xf0, 0xff, 0x01, 0x00,
  0xe0, 0xff, 0x00, 0x00, 0xc0, 0xff, 0x00, 0x00, 0xc0, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* MOZ_CURSOR_HAND_GRABBING */
static const char moz_hand_grabbing_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xc0, 0x36, 0x00, 0x00, 0x20, 0xc9, 0x00, 0x00, 0x20, 0x40, 0x01, 0x00,
  0x40, 0x00, 0x01, 0x00, 0x60, 0x00, 0x01, 0x00, 0x10, 0x00, 0x01, 0x00,
  0x10, 0x00, 0x01, 0x00, 0x10, 0x80, 0x00, 0x00, 0x20, 0x80, 0x00, 0x00,
  0x40, 0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_hand_grabbing_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x36, 0x00, 0x00,
  0xe0, 0xff, 0x00, 0x00, 0xf0, 0xff, 0x01, 0x00, 0xf0, 0xff, 0x03, 0x00,
  0xe0, 0xff, 0x03, 0x00, 0xf0, 0xff, 0x03, 0x00, 0xf8, 0xff, 0x03, 0x00,
  0xf8, 0xff, 0x03, 0x00, 0xf8, 0xff, 0x01, 0x00, 0xf0, 0xff, 0x01, 0x00,
  0xe0, 0xff, 0x00, 0x00, 0xc0, 0xff, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

enum CustomCursorType {
  CustomCursorCopy = 0,
  CustomCursorAlias,
  CustomCursorContextMenu,
  CustomCursorZoomIn,
  CustomCursorZoomOut,
  CustomCursorVerticalText,
  CustomCursorGrab,
  CustomCursorGrabbing,
};

typedef struct {
  const char* name;
  const char* bits;
  const char* mask_bits;
  int hot_x;
  int hot_y;
} CustomCursor;

// create custom pixmap cursor from cursors in nsGTKCursorData.h
static const CustomCursor CustomCursors[] = {
  { "copy", moz_copy_bits, moz_copy_mask_bits, 2, 2 },
  { "alias", moz_alias_bits, moz_alias_mask_bits, 2, 2 },
  { "context-menu", moz_menu_bits, moz_menu_mask_bits, 2, 2 },
  { "zoom-in", moz_zoom_in_bits, moz_zoom_in_mask_bits, 6, 6 },
  { "zoom-out", moz_zoom_out_bits, moz_zoom_out_mask_bits, 6, 6 },
  { "vertical-text", moz_vertical_text_bits, moz_vertical_text_mask_bits, 8, 4 },
  { "grab", moz_hand_grab_bits, moz_hand_grab_mask_bits, 10, 10 },
  { "grabbing", moz_hand_grabbing_bits, moz_hand_grabbing_mask_bits, 10, 10 }
};

// This cursor intentionally left out of above structs. It is only used by
// RenderWidgetHostViewGtk. For an explanation see
// http://vektor-sigma.livejournal.com/1137.html (where it is referred
// to as left_ptr_watch).

/* MOZ_CURSOR_SPINNING */
static const char moz_spinning_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
  0x7c, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00,
  0xfc, 0x3b, 0x00, 0x00, 0x7c, 0x38, 0x00, 0x00, 0x6c, 0x54, 0x00, 0x00,
  0xc4, 0xdc, 0x00, 0x00, 0xc0, 0x44, 0x00, 0x00, 0x80, 0x39, 0x00, 0x00,
  0x80, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char moz_spinning_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x1e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
  0xfe, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x00, 0xfe, 0x3b, 0x00, 0x00,
  0xfe, 0x7f, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00,
  0xee, 0xff, 0x01, 0x00, 0xe4, 0xff, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00,
  0xc0, 0x7f, 0x00, 0x00, 0x80, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
