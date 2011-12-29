/*
 * Copyright 2008 Corbin Simpson <MostAwesomeDude@gmail.com>
 * Copyright 2010 Marek Olšák <maraeo@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef R300_TEXTURE_DESC_H
#define R300_TEXTURE_DESC_H

#include "pipe/p_format.h"
#include "r300_defines.h"

struct pipe_resource;
struct r300_screen;
struct r300_texture_desc;
struct r300_texture;

enum r300_dim {
    DIM_WIDTH  = 0,
    DIM_HEIGHT = 1
};

unsigned r300_get_pixel_alignment(enum pipe_format format,
                                  unsigned num_samples,
                                  enum r300_buffer_tiling microtile,
                                  enum r300_buffer_tiling macrotile,
                                  enum r300_dim dim);

boolean r300_texture_desc_init(struct r300_screen *rscreen,
                               struct r300_texture_desc *desc,
                               const struct pipe_resource *base,
                               enum r300_buffer_tiling microtile,
                               enum r300_buffer_tiling macrotile,
                               unsigned stride_in_bytes_override,
                               unsigned max_buffer_size);

unsigned r300_texture_get_offset(struct r300_texture_desc *desc,
                                 unsigned level, unsigned zslice,
                                 unsigned face);

#endif
