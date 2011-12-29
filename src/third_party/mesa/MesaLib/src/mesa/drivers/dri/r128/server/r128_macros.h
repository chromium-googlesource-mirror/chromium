/**
 * \file server/R128_macros.h
 * \brief Macros for R128 MMIO operation.
 *
 * \authors Kevin E. Martin <martin@xfree86.org>
 * \authors Rickard E. Faith <faith@valinux.com>
 * \authors Alan Hourihane <alanh@fairlite.demon.co.uk>
 */

/*
 * Copyright 2000 ATI Technologies Inc., Markham, Ontario, and
 *                VA Linux Systems Inc., Fremont, California.
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation on the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL ATI, VA LINUX SYSTEMS AND/OR
 * THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#ifndef _R128_MACROS_H_
#define _R128_MACROS_H_



#  define MMIO_IN8(base, offset) \
	*(volatile unsigned char *)(((unsigned char*)(base)) + (offset))
#  define MMIO_IN16(base, offset) \
	*(volatile unsigned short *)(void *)(((unsigned char*)(base)) + (offset))
#  define MMIO_IN32(base, offset) \
	*(volatile unsigned int *)(void *)(((unsigned char*)(base)) + (offset))
#  define MMIO_OUT8(base, offset, val) \
	*(volatile unsigned char *)(((unsigned char*)(base)) + (offset)) = (val)
#  define MMIO_OUT16(base, offset, val) \
	*(volatile unsigned short *)(void *)(((unsigned char*)(base)) + (offset)) = (val)
#  define MMIO_OUT32(base, offset, val) \
	*(volatile unsigned int *)(void *)(((unsigned char*)(base)) + (offset)) = (val)


				/* Memory mapped register access macros */
#define INREG8(addr)        MMIO_IN8(R128MMIO, addr)
#define INREG16(addr)       MMIO_IN16(R128MMIO, addr)
#define INREG(addr)         MMIO_IN32(R128MMIO, addr)
#define OUTREG8(addr, val)  MMIO_OUT8(R128MMIO, addr, val)
#define OUTREG16(addr, val) MMIO_OUT16(R128MMIO, addr, val)
#define OUTREG(addr, val)   MMIO_OUT32(R128MMIO, addr, val)

#define ADDRREG(addr)       ((volatile GLuint *)(pointer)(R128MMIO + (addr)))


#define OUTREGP(addr, val, mask)					\
do {									\
    GLuint tmp = INREG(addr);						\
    tmp &= (mask);							\
    tmp |= (val);							\
    OUTREG(addr, tmp);							\
} while (0)

#define INPLL(dpy, addr) r128INPLL(dpy, addr)

#define OUTPLL(addr, val)						\
do {									\
    OUTREG8(R128_CLOCK_CNTL_INDEX, (((addr) & 0x3f) |			\
				      R128_PLL_WR_EN));		\
    OUTREG(R128_CLOCK_CNTL_DATA, val);				\
} while (0)

#define OUTPLLP(dpy, addr, val, mask)					\
do {									\
    GLuint tmp = INPLL(dpy, addr);					\
    tmp &= (mask);							\
    tmp |= (val);							\
    OUTPLL(addr, tmp);							\
} while (0)

#define OUTPAL_START(idx)						\
do {									\
    OUTREG8(R128_PALETTE_INDEX, (idx));				\
} while (0)

#define OUTPAL_NEXT(r, g, b)						\
do {									\
    OUTREG(R128_PALETTE_DATA, ((r) << 16) | ((g) << 8) | (b));	\
} while (0)

#define OUTPAL_NEXT_CARD32(v)						\
do {									\
    OUTREG(R128_PALETTE_DATA, (v & 0x00ffffff));			\
} while (0)

#define OUTPAL(idx, r, g, b)						\
do {									\
    OUTPAL_START((idx));						\
    OUTPAL_NEXT((r), (g), (b));						\
} while (0)

#define INPAL_START(idx)						\
do {									\
    OUTREG(R128_PALETTE_INDEX, (idx) << 16);				\
} while (0)

#define INPAL_NEXT() INREG(R128_PALETTE_DATA)

#define PAL_SELECT(idx)							\
do {									\
    if (!idx) {								\
	OUTREG(R128_DAC_CNTL2, INREG(R128_DAC_CNTL2) &		\
	       (GLuint)~R128_DAC2_PALETTE_ACC_CTL);			\
    } else {								\
	OUTREG(R128_DAC_CNTL2, INREG(R128_DAC_CNTL2) |		\
	       R128_DAC2_PALETTE_ACC_CTL);				\
    }									\
} while (0)


#endif
