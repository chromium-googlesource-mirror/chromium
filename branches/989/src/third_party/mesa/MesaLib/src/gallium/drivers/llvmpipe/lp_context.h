/**************************************************************************
 * 
 * Copyright 2007 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 **************************************************************************/

/* Authors:  Keith Whitwell <keith@tungstengraphics.com>
 */

#ifndef LP_CONTEXT_H
#define LP_CONTEXT_H

#include "pipe/p_context.h"

#include "draw/draw_vertex.h"

#include "lp_tex_sample.h"
#include "lp_jit.h"
#include "lp_setup.h"
#include "lp_state_fs.h"


struct llvmpipe_vbuf_render;
struct draw_context;
struct draw_stage;
struct lp_fragment_shader;
struct lp_vertex_shader;
struct lp_blend_state;
struct lp_setup_context;
struct lp_velems_state;

struct llvmpipe_context {
   struct pipe_context pipe;  /**< base class */

   /** Constant state objects */
   const struct pipe_blend_state *blend;
   const struct pipe_sampler_state *sampler[PIPE_MAX_SAMPLERS];
   struct pipe_sampler_state *vertex_samplers[PIPE_MAX_VERTEX_SAMPLERS];
   const struct pipe_depth_stencil_alpha_state *depth_stencil;
   const struct pipe_rasterizer_state *rasterizer;
   struct lp_fragment_shader *fs;
   const struct lp_vertex_shader *vs;
   const struct lp_geometry_shader *gs;
   const struct lp_velems_state *velems;
   const struct lp_so_state *so;

   /** Other rendering state */
   struct pipe_blend_color blend_color;
   struct pipe_stencil_ref stencil_ref;
   struct pipe_clip_state clip;
   struct pipe_resource *constants[PIPE_SHADER_TYPES][PIPE_MAX_CONSTANT_BUFFERS];
   struct pipe_framebuffer_state framebuffer;
   struct pipe_poly_stipple poly_stipple;
   struct pipe_scissor_state scissor;
   struct pipe_sampler_view *fragment_sampler_views[PIPE_MAX_SAMPLERS];
   struct pipe_sampler_view *vertex_sampler_views[PIPE_MAX_VERTEX_SAMPLERS];
   struct pipe_viewport_state viewport;
   struct pipe_vertex_buffer vertex_buffer[PIPE_MAX_ATTRIBS];
   struct pipe_index_buffer index_buffer;
   struct {
      struct llvmpipe_resource *buffer[PIPE_MAX_SO_BUFFERS];
      int offset[PIPE_MAX_SO_BUFFERS];
      int so_count[PIPE_MAX_SO_BUFFERS];
      int num_buffers;
   } so_target;
   struct pipe_resource *mapped_vs_tex[PIPE_MAX_VERTEX_SAMPLERS];

   unsigned num_samplers;
   unsigned num_fragment_sampler_views;
   unsigned num_vertex_samplers;
   unsigned num_vertex_sampler_views;
   unsigned num_vertex_buffers;

   unsigned dirty; /**< Mask of LP_NEW_x flags */

   int active_query_count;

   /** Mapped vertex buffers */
   ubyte *mapped_vbuffer[PIPE_MAX_ATTRIBS];
   
   /** Vertex format */
   struct vertex_info vertex_info;
   
   /** Which vertex shader output slot contains point size */
   int psize_slot;

   /** Fragment shader input interpolation info */
   unsigned num_inputs;
   struct lp_shader_input inputs[PIPE_MAX_SHADER_INPUTS];

   /** The tiling engine */
   struct lp_setup_context *setup;

   /** The primitive drawing context */
   struct draw_context *draw;

   unsigned tex_timestamp;
   boolean no_rast;

   struct lp_fs_variant_list_item fs_variants_list;
   unsigned nr_fs_variants;
};


struct pipe_context *
llvmpipe_create_context( struct pipe_screen *screen, void *priv );


static INLINE struct llvmpipe_context *
llvmpipe_context( struct pipe_context *pipe )
{
   return (struct llvmpipe_context *)pipe;
}

#endif /* LP_CONTEXT_H */

