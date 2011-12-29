/**********************************************************
 * Copyright 2009 VMware, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************/

/**
 * @author Jose Fonseca <jfonseca@vmware.com>
 */


#ifndef VMW_CONTEXT_H_
#define VMW_CONTEXT_H_

#include "pipe/p_compiler.h"

struct svga_winsys_screen;
struct svga_winsys_context;
struct pipe_context;
struct pipe_screen;

#define VMW_DEBUG 0

#if VMW_DEBUG
#define vmw_printf debug_printf
#define VMW_FUNC  debug_printf("%s\n", __FUNCTION__)
#else
#define VMW_FUNC
#define vmw_printf(...)
#endif


struct svga_winsys_context *
vmw_svga_winsys_context_create(struct svga_winsys_screen *sws);

void
vmw_svga_context_set_throttling(struct pipe_context *pipe,
				uint32_t throttle_us);

#endif /* VMW_CONTEXT_H_ */
