; Copyright (c) 2011 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.

%include "x86inc.asm"

;
; This file uses MMX and SSE instructions.
;
  SECTION_TEXT
  CPU       MMX, SSE

; Use SSE instruction movntq can write faster.
%define MOVQ movntq

;
; extern "C" void ConvertYUVToRGB32Row_SSE(const uint8* y_buf,
;                                          const uint8* u_buf,
;                                          const uint8* v_buf,
;                                          uint8* rgb_buf,
;                                          int width);
%define SYMBOL ConvertYUVToRGB32Row_SSE
%include "convert_yuv_to_rgb_mmx.inc"
