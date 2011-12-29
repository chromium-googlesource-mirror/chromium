/**************************************************************************

Copyright 1998-1999 Precision Insight, Inc., Cedar Park, Texas.
All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sub license, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

/*
 * Authors:
 *   Keith Whitwell <keith@tungstengraphics.com>
 *
 */


#include "main/glheader.h"
#include "main/imports.h"
#include "main/context.h"
#include "main/framebuffer.h"
#include "main/renderbuffer.h"
#include "main/simple_list.h"
#include "utils.h"

#include "i810screen.h"
#include "i810_dri.h"

#include "i810state.h"
#include "i810tex.h"
#include "i810span.h"

#include "GL/internal/dri_interface.h"

static const __DRIconfig **
i810FillInModes( __DRIscreen *psp,
		 unsigned pixel_bits, unsigned depth_bits,
		 unsigned stencil_bits, GLboolean have_back_buffer )
{
    __DRIconfig **configs;
    __GLcontextModes * m;
    unsigned depth_buffer_factor;
    unsigned back_buffer_factor;
    unsigned i;

    /* Right now GLX_SWAP_COPY_OML isn't supported, but it would be easy
     * enough to add support.  Basically, if a context is created with an
     * fbconfig where the swap method is GLX_SWAP_COPY_OML, pageflipping
     * will never be used.
     */
    static const GLenum back_buffer_modes[] = {
	GLX_NONE, GLX_SWAP_UNDEFINED_OML /*, GLX_SWAP_COPY_OML */
    };

    uint8_t depth_bits_array[2];
    uint8_t stencil_bits_array[2];
    uint8_t msaa_samples_array[1];

    depth_bits_array[0] = depth_bits;
    depth_bits_array[1] = depth_bits;

    /* Just like with the accumulation buffer, always provide some modes
     * with a stencil buffer.  It will be a sw fallback, but some apps won't
     * care about that.
     */
    stencil_bits_array[0] = 0;
    stencil_bits_array[1] = (stencil_bits == 0) ? 8 : stencil_bits;

    msaa_samples_array[0] = 0;

    depth_buffer_factor = ((depth_bits != 0) || (stencil_bits != 0)) ? 2 : 1;
    back_buffer_factor  = (have_back_buffer) ? 2 : 1;

    configs = driCreateConfigs(GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
			       depth_bits_array, stencil_bits_array,
			       depth_buffer_factor,
			       back_buffer_modes, back_buffer_factor,
                               msaa_samples_array, 1, GL_TRUE);
    if (configs == NULL) {
	fprintf( stderr, "[%s:%u] Error creating FBConfig!\n",
		 __func__, __LINE__ );
	return NULL;
    }

    /* Mark the visual as slow if there are "fake" stencil bits.
     */
    for (i = 0; configs[i]; i++) {
	m = &configs[i]->modes;
	if ((m->stencilBits != 0) && (m->stencilBits != stencil_bits)) {
	    m->visualRating = GLX_SLOW_CONFIG;
	}
    }

    return (const __DRIconfig **) configs;
}

     
/*  static int i810_malloc_proxy_buf(drmBufMapPtr buffers) */
/*  { */
/*     char *buffer; */
/*     drmBufPtr buf; */
/*     int i; */

/*     buffer = CALLOC(I810_DMA_BUF_SZ); */
/*     if(buffer == NULL) return -1; */
/*     for(i = 0; i < I810_DMA_BUF_NR; i++) { */
/*        buf = &(buffers->list[i]); */
/*        buf->address = (drmAddress)buffer; */
/*     } */
/*     return 0; */
/*  } */

static drmBufMapPtr i810_create_empty_buffers(void)
{
   drmBufMapPtr retval;

   retval = (drmBufMapPtr)_mesa_align_malloc(sizeof(drmBufMap), 32);
   if(retval == NULL) return NULL;
   memset(retval, 0, sizeof(drmBufMap));
   retval->list = (drmBufPtr)_mesa_align_malloc(sizeof(drmBuf) * I810_DMA_BUF_NR, 32);
   if(retval->list == NULL) {
      _mesa_align_free(retval);
      return NULL;
   }
   memset(retval->list, 0, sizeof(drmBuf) * I810_DMA_BUF_NR);
   return retval;
}


static const __DRIconfig **
i810InitScreen(__DRIscreen *sPriv)
{
   static const __DRIversion ddx_expected = { 1, 0, 0 };
   static const __DRIversion dri_expected = { 4, 0, 0 };
   static const __DRIversion drm_expected = { 1, 2, 0 };
   i810ScreenPrivate *i810Screen;
   I810DRIPtr         gDRIPriv = (I810DRIPtr)sPriv->pDevPriv;

   if ( ! driCheckDriDdxDrmVersions2( "i810",
				      &sPriv->dri_version, & dri_expected,
				      &sPriv->ddx_version, & ddx_expected,
				      &sPriv->drm_version, & drm_expected ) ) {
      return NULL;
   }

   if (sPriv->devPrivSize != sizeof(I810DRIRec)) {
      fprintf(stderr,"\nERROR!  sizeof(I810DRIRec) does not match passed size from device driver\n");
      return GL_FALSE;
   }

   /* Allocate the private area */
   i810Screen = (i810ScreenPrivate *)CALLOC(sizeof(i810ScreenPrivate));
   if (!i810Screen) {
      __driUtilMessage("i810InitDriver: alloc i810ScreenPrivate struct failed");
      return GL_FALSE;
   }

   i810Screen->driScrnPriv = sPriv;
   sPriv->private = (void *)i810Screen;

   i810Screen->deviceID=gDRIPriv->deviceID;
   i810Screen->width=gDRIPriv->width;
   i810Screen->height=gDRIPriv->height;
   i810Screen->mem=gDRIPriv->mem;
   i810Screen->cpp=gDRIPriv->cpp;
   i810Screen->fbStride=gDRIPriv->fbStride;
   i810Screen->fbOffset=gDRIPriv->fbOffset;

   if (gDRIPriv->bitsPerPixel == 15)
      i810Screen->fbFormat = DV_PF_555;
   else
      i810Screen->fbFormat = DV_PF_565;

   i810Screen->backOffset=gDRIPriv->backOffset;
   i810Screen->depthOffset=gDRIPriv->depthOffset;
   i810Screen->backPitch = gDRIPriv->auxPitch;
   i810Screen->backPitchBits = gDRIPriv->auxPitchBits;
   i810Screen->textureOffset=gDRIPriv->textureOffset;
   i810Screen->textureSize=gDRIPriv->textureSize;
   i810Screen->logTextureGranularity = gDRIPriv->logTextureGranularity;

   i810Screen->bufs = i810_create_empty_buffers();
   if (i810Screen->bufs == NULL) {
      __driUtilMessage("i810InitDriver: i810_create_empty_buffers() failed");
      FREE(i810Screen);
      return GL_FALSE;
   }

   i810Screen->back.handle = gDRIPriv->backbuffer;
   i810Screen->back.size = gDRIPriv->backbufferSize;

   if (drmMap(sPriv->fd,
	      i810Screen->back.handle,
	      i810Screen->back.size,
	      (drmAddress *)&i810Screen->back.map) != 0) {
      FREE(i810Screen);
      sPriv->private = NULL;
      __driUtilMessage("i810InitDriver: drmMap failed");
      return GL_FALSE;
   }

   i810Screen->depth.handle = gDRIPriv->depthbuffer;
   i810Screen->depth.size = gDRIPriv->depthbufferSize;

   if (drmMap(sPriv->fd,
	      i810Screen->depth.handle,
	      i810Screen->depth.size,
	      (drmAddress *)&i810Screen->depth.map) != 0) {
      drmUnmap(i810Screen->back.map, i810Screen->back.size);
      FREE(i810Screen);
      sPriv->private = NULL;
      __driUtilMessage("i810InitDriver: drmMap (2) failed");
      return GL_FALSE;
   }

   i810Screen->tex.handle = gDRIPriv->textures;
   i810Screen->tex.size = gDRIPriv->textureSize;

   if (drmMap(sPriv->fd,
	      i810Screen->tex.handle,
	      i810Screen->tex.size,
	      (drmAddress *)&i810Screen->tex.map) != 0) {
      drmUnmap(i810Screen->back.map, i810Screen->back.size);
      drmUnmap(i810Screen->depth.map, i810Screen->depth.size);
      FREE(i810Screen);
      sPriv->private = NULL;
      __driUtilMessage("i810InitDriver: drmMap (3) failed");
      return GL_FALSE;
   }

   i810Screen->sarea_priv_offset = gDRIPriv->sarea_priv_offset;

   return i810FillInModes(sPriv, 16, 16, 0, 1);
}

static void
i810DestroyScreen(__DRIscreen *sPriv)
{
   i810ScreenPrivate *i810Screen = (i810ScreenPrivate *)sPriv->private;

   /* Need to unmap all the bufs and maps here:
    */
   drmUnmap(i810Screen->back.map, i810Screen->back.size);
   drmUnmap(i810Screen->depth.map, i810Screen->depth.size);
   drmUnmap(i810Screen->tex.map, i810Screen->tex.size);

   FREE(i810Screen);
   sPriv->private = NULL;
}


/**
 * Create a buffer which corresponds to the window.
 */
static GLboolean
i810CreateBuffer( __DRIscreen *driScrnPriv,
                  __DRIdrawable *driDrawPriv,
                  const __GLcontextModes *mesaVis,
                  GLboolean isPixmap )
{
   i810ScreenPrivate *screen = (i810ScreenPrivate *) driScrnPriv->private;

   if (isPixmap) {
      return GL_FALSE; /* not implemented */
   }
   else {
      struct gl_framebuffer *fb = _mesa_create_framebuffer(mesaVis);

      {
         driRenderbuffer *frontRb
            = driNewRenderbuffer(MESA_FORMAT_ARGB8888,
                                 driScrnPriv->pFB,
                                 screen->cpp,
                                 /*screen->frontOffset*/0, screen->backPitch,
                                 driDrawPriv);
         i810SetSpanFunctions(frontRb, mesaVis);
         _mesa_add_renderbuffer(fb, BUFFER_FRONT_LEFT, &frontRb->Base);
      }

      if (mesaVis->doubleBufferMode) {
         driRenderbuffer *backRb
            = driNewRenderbuffer(MESA_FORMAT_ARGB8888,
                                 screen->back.map,
                                 screen->cpp,
                                 screen->backOffset, screen->backPitch,
                                 driDrawPriv);
         i810SetSpanFunctions(backRb, mesaVis);
         _mesa_add_renderbuffer(fb, BUFFER_BACK_LEFT, &backRb->Base);
      }

      if (mesaVis->depthBits == 16) {
         driRenderbuffer *depthRb
            = driNewRenderbuffer(MESA_FORMAT_Z16,
                                 screen->depth.map,
                                 screen->cpp,
                                 screen->depthOffset, screen->backPitch,
                                 driDrawPriv);
         i810SetSpanFunctions(depthRb, mesaVis);
         _mesa_add_renderbuffer(fb, BUFFER_DEPTH, &depthRb->Base);
      }

      _mesa_add_soft_renderbuffers(fb,
                                   GL_FALSE, /* color */
                                   GL_FALSE, /* depth */
                                   mesaVis->stencilBits > 0,
                                   mesaVis->accumRedBits > 0,
                                   GL_FALSE, /* alpha */
                                   GL_FALSE /* aux */);
      driDrawPriv->driverPrivate = (void *) fb;

      return (driDrawPriv->driverPrivate != NULL);
   }
}


static void
i810DestroyBuffer(__DRIdrawable *driDrawPriv)
{
   _mesa_reference_framebuffer((GLframebuffer **)(&(driDrawPriv->driverPrivate)), NULL);
}

const struct __DriverAPIRec driDriverAPI = {
   .InitScreen      = i810InitScreen,
   .DestroyScreen   = i810DestroyScreen,
   .CreateContext   = i810CreateContext,
   .DestroyContext  = i810DestroyContext,
   .CreateBuffer    = i810CreateBuffer,
   .DestroyBuffer   = i810DestroyBuffer,
   .SwapBuffers     = i810SwapBuffers,
   .MakeCurrent     = i810MakeCurrent,
   .UnbindContext   = i810UnbindContext,
   .GetSwapInfo     = NULL,
   .GetDrawableMSC  = NULL,
   .WaitForMSC      = NULL,
   .WaitForSBC      = NULL,
   .SwapBuffersMSC  = NULL
};

/* This is the table of extensions that the loader will dlsym() for. */
PUBLIC const __DRIextension *__driDriverExtensions[] = {
    &driCoreExtension.base,
    &driLegacyExtension.base,
    NULL
};
