/**************************************************************************
 * 
 * Copyright 2005 Tungsten Graphics, Inc., Cedar Park, Texas.
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

#include "main/context.h"
#include "main/glheader.h"
#include "main/enums.h"
#include "main/imports.h"
#include "main/mtypes.h"
#include "main/dispatch.h"
#include "glapi/glapi.h"

#include "vbo_context.h"


#if FEATURE_dlist


typedef void (*attr_func)( GLcontext *ctx, GLint target, const GLfloat * );


/* This file makes heavy use of the aliasing of NV vertex attributes
 * with the legacy attributes, and also with ARB and Material
 * attributes as currently implemented.
 */
static void VertexAttrib1fvNV(GLcontext *ctx, GLint target, const GLfloat *v)
{
   CALL_VertexAttrib1fvNV(ctx->Exec, (target, v));
}

static void VertexAttrib2fvNV(GLcontext *ctx, GLint target, const GLfloat *v)
{
   CALL_VertexAttrib2fvNV(ctx->Exec, (target, v));
}

static void VertexAttrib3fvNV(GLcontext *ctx, GLint target, const GLfloat *v)
{
   CALL_VertexAttrib3fvNV(ctx->Exec, (target, v));
}

static void VertexAttrib4fvNV(GLcontext *ctx, GLint target, const GLfloat *v)
{
   CALL_VertexAttrib4fvNV(ctx->Exec, (target, v));
}

static attr_func vert_attrfunc[4] = {
   VertexAttrib1fvNV,
   VertexAttrib2fvNV,
   VertexAttrib3fvNV,
   VertexAttrib4fvNV
};

struct loopback_attr {
   GLint target;
   GLint sz;
   attr_func func;
};

/* Don't emit ends and begins on wrapped primitives.  Don't replay
 * wrapped vertices.  If we get here, it's probably because the
 * precalculated wrapping is wrong.
 */
static void loopback_prim( GLcontext *ctx,
			   const GLfloat *buffer,
			   const struct _mesa_prim *prim,
			   GLuint wrap_count,
			   GLuint vertex_size,
			   const struct loopback_attr *la, GLuint nr )
{
   GLint start = prim->start;
   GLint end = start + prim->count;
   const GLfloat *data;
   GLint j;
   GLuint k;

   if (0)
      printf("loopback prim %s(%s,%s) verts %d..%d\n",
	     _mesa_lookup_prim_by_nr(prim->mode),
	     prim->begin ? "begin" : "..",
	     prim->end ? "end" : "..",
	     start, 
	     end);

   if (prim->begin) {
      CALL_Begin(GET_DISPATCH(), ( prim->mode ));
   }
   else {
      assert(start == 0);
      start += wrap_count;
   }

   data = buffer + start * vertex_size;

   for (j = start ; j < end ; j++) {
      const GLfloat *tmp = data + la[0].sz;

      for (k = 1 ; k < nr ; k++) {
	 la[k].func( ctx, la[k].target, tmp );
	 tmp += la[k].sz;
      }
	 
      /* Fire the vertex
       */
      la[0].func( ctx, VBO_ATTRIB_POS, data );
      data = tmp;
   }

   if (prim->end) {
      CALL_End(GET_DISPATCH(), ());
   }
}

/* Primitives generated by DrawArrays/DrawElements/Rectf may be
 * caught here.  If there is no primitive in progress, execute them
 * normally, otherwise need to track and discard the generated
 * primitives.
 */
static void loopback_weak_prim( GLcontext *ctx,
				const struct _mesa_prim *prim )
{
   /* Use the prim_weak flag to ensure that if this primitive
    * wraps, we don't mistake future vertex_lists for part of the
    * surrounding primitive.
    *
    * While this flag is set, we are simply disposing of data
    * generated by an operation now known to be a noop.
    */
   if (prim->begin)
      ctx->Driver.CurrentExecPrimitive |= VBO_SAVE_PRIM_WEAK;
   if (prim->end)
      ctx->Driver.CurrentExecPrimitive &= ~VBO_SAVE_PRIM_WEAK;
}


void vbo_loopback_vertex_list( GLcontext *ctx,
			       const GLfloat *buffer,
			       const GLubyte *attrsz,
			       const struct _mesa_prim *prim,
			       GLuint prim_count,
			       GLuint wrap_count,
			       GLuint vertex_size)
{
   struct loopback_attr la[VBO_ATTRIB_MAX];
   GLuint i, nr = 0;

   /* All Legacy, NV, ARB and Material attributes are routed through
    * the NV attributes entrypoints:
    */
   for (i = 0 ; i < VBO_ATTRIB_MAX ; i++) {
      if (attrsz[i]) {
	 la[nr].target = i;
	 la[nr].sz = attrsz[i];
	 la[nr].func = vert_attrfunc[attrsz[i]-1];
	 nr++;
      }
   }

   for (i = 0 ; i < prim_count ; i++) {
      if ((prim[i].mode & VBO_SAVE_PRIM_WEAK) &&
	  (ctx->Driver.CurrentExecPrimitive != PRIM_OUTSIDE_BEGIN_END))
      {
	 loopback_weak_prim( ctx, &prim[i] );
      }
      else
      {
	 loopback_prim( ctx, buffer, &prim[i], wrap_count, vertex_size, la, nr );
      }
   }
}


#endif /* FEATURE_dlist */
