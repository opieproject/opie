/*
 * yuv2rgb_mmx.c
 * Copyright (C) 2000-2001 Silicon Integrated System Corp.
 * All Rights Reserved.
 *
 * Author: Olie Lho <ollie@sis.com.tw>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifdef ARCH_X86

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "yuv2rgb.h"
#include "xineutils.h"

#define CPU_MMXEXT 0
#define CPU_MMX 1

/* CPU_MMXEXT/CPU_MMX adaptation layer */

#define movntq(src,dest)	\
do {				\
    if (cpu == CPU_MMXEXT)	\
	movntq_r2m (src, dest);	\
    else			\
	movq_r2m (src, dest);	\
} while (0)

static mmx_t mmx_subYw = {0x1010101010101010};
static mmx_t mmx_addYw = {0x0000000000000000};

void mmx_yuv2rgb_set_gamma(int gamma) 
{
int a,s,i;

  if( gamma <= 16 ) {
    a = 0;
    s = 16 - gamma;
  } else {
    a = gamma - 16;
    s = 0;
  }
  
  for( i = 0; i < 8; i++ ) {
    *((unsigned char *)&mmx_subYw + i) = s;
    *((unsigned char *)&mmx_addYw + i) = a;
  }
}

static inline void mmx_yuv2rgb (uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    static mmx_t mmx_80w = {0x0080008000800080};
    static mmx_t mmx_U_green = {0xf37df37df37df37d};
    static mmx_t mmx_U_blue = {0x4093409340934093};
    static mmx_t mmx_V_red = {0x3312331233123312};
    static mmx_t mmx_V_green = {0xe5fce5fce5fce5fc};
    static mmx_t mmx_00ffw = {0x00ff00ff00ff00ff};
    static mmx_t mmx_Y_coeff = {0x253f253f253f253f};

    movq_m2r (*py, mm6);		// mm6 = Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
    pxor_r2r (mm4, mm4);		// mm4 = 0

    psubusb_m2r (mmx_subYw, mm6);	// Y -= 16
    paddusb_m2r (mmx_addYw, mm6);

    movd_m2r (*pu, mm0);		// mm0 = 00 00 00 00 u3 u2 u1 u0
    movq_r2r (mm6, mm7);		// mm7 = Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0

    pand_m2r (mmx_00ffw, mm6);		// mm6 =    Y6    Y4    Y2    Y0
    psrlw_i2r (8, mm7);			// mm7 =    Y7    Y5    Y3    Y1

    movd_m2r (*pv, mm1);		// mm1 = 00 00 00 00 v3 v2 v1 v0
    psllw_i2r (3, mm6);			// promote precision

    pmulhw_m2r (mmx_Y_coeff, mm6);	// mm6 = luma_rgb even
    psllw_i2r (3, mm7);			// promote precision

    punpcklbw_r2r (mm4, mm0);		// mm0 = u3 u2 u1 u0

    psubsw_m2r (mmx_80w, mm0);		// u -= 128
    punpcklbw_r2r (mm4, mm1);		// mm1 = v3 v2 v1 v0

    pmulhw_m2r (mmx_Y_coeff, mm7);	// mm7 = luma_rgb odd
    psllw_i2r (3, mm0);			// promote precision

    psubsw_m2r (mmx_80w, mm1);		// v -= 128
    movq_r2r (mm0, mm2);		// mm2 = u3 u2 u1 u0

    psllw_i2r (3, mm1);			// promote precision

    movq_r2r (mm1, mm4);		// mm4 = v3 v2 v1 v0

    pmulhw_m2r (mmx_U_blue, mm0);	// mm0 = chroma_b


    // slot


    // slot


    pmulhw_m2r (mmx_V_red, mm1);	// mm1 = chroma_r
    movq_r2r (mm0, mm3);		// mm3 = chroma_b

    paddsw_r2r (mm6, mm0);		// mm0 = B6 B4 B2 B0
    paddsw_r2r (mm7, mm3);		// mm3 = B7 B5 B3 B1

    packuswb_r2r (mm0, mm0);		// saturate to 0-255


    pmulhw_m2r (mmx_U_green, mm2);	// mm2 = u * u_green


    packuswb_r2r (mm3, mm3);		// saturate to 0-255


    punpcklbw_r2r (mm3, mm0);		// mm0 = B7 B6 B5 B4 B3 B2 B1 B0


    pmulhw_m2r (mmx_V_green, mm4);	// mm4 = v * v_green

    
    // slot


    // slot


    paddsw_r2r (mm4, mm2);		// mm2 = chroma_g
    movq_r2r (mm2, mm5);		// mm5 = chroma_g


    movq_r2r (mm1, mm4);		// mm4 = chroma_r
    paddsw_r2r (mm6, mm2);		// mm2 = G6 G4 G2 G0


    packuswb_r2r (mm2, mm2);		// saturate to 0-255
    paddsw_r2r (mm6, mm1);		// mm1 = R6 R4 R2 R0

    packuswb_r2r (mm1, mm1);		// saturate to 0-255
    paddsw_r2r (mm7, mm4);		// mm4 = R7 R5 R3 R1

    packuswb_r2r (mm4, mm4);		// saturate to 0-255
    paddsw_r2r (mm7, mm5);		// mm5 = G7 G5 G3 G1


    packuswb_r2r (mm5, mm5);		// saturate to 0-255


    punpcklbw_r2r (mm4, mm1);		// mm1 = R7 R6 R5 R4 R3 R2 R1 R0


    punpcklbw_r2r (mm5, mm2);		// mm2 = G7 G6 G5 G4 G3 G2 G1 G0
}

// basic opt
static inline void mmx_unpack_16rgb (uint8_t * image, int cpu)
{
    static mmx_t mmx_bluemask = {0xf8f8f8f8f8f8f8f8};
    static mmx_t mmx_greenmask = {0xfcfcfcfcfcfcfcfc};
    static mmx_t mmx_redmask = {0xf8f8f8f8f8f8f8f8};

    /*
     * convert RGB plane to RGB 16 bits
     * mm0 -> B, mm1 -> R, mm2 -> G
     * mm4 -> GB, mm5 -> AR pixel 4-7
     * mm6 -> GB, mm7 -> AR pixel 0-3
     */

    pand_m2r (mmx_bluemask, mm0);	// mm0 = b7b6b5b4b3______
    pxor_r2r (mm4, mm4);		// mm4 = 0

    pand_m2r (mmx_greenmask, mm2);	// mm2 = g7g6g5g4g3g2____
    psrlq_i2r (3, mm0);			// mm0 = ______b7b6b5b4b3

    movq_r2r (mm2, mm7);		// mm7 = g7g6g5g4g3g2____
    movq_r2r (mm0, mm5);		// mm5 = ______b7b6b5b4b3

    pand_m2r (mmx_redmask, mm1);	// mm1 = r7r6r5r4r3______
    punpcklbw_r2r (mm4, mm2);

    punpcklbw_r2r (mm1, mm0);

    psllq_i2r (3, mm2);

    punpckhbw_r2r (mm4, mm7);
    por_r2r (mm2, mm0);

    psllq_i2r (3, mm7);

    movntq (mm0, *image);
    punpckhbw_r2r (mm1, mm5);

    por_r2r (mm7, mm5);

    // U
    // V

    movntq (mm5, *(image+8));
}

static inline void mmx_unpack_15rgb (uint8_t * image, int cpu)
{
    static mmx_t mmx_bluemask = {0xf8f8f8f8f8f8f8f8};
    static mmx_t mmx_greenmask = {0xf8f8f8f8f8f8f8f8};
    static mmx_t mmx_redmask = {0xf8f8f8f8f8f8f8f8};

    /*
     * convert RGB plane to RGB 15 bits
     * mm0 -> B, mm1 -> R, mm2 -> G
     * mm4 -> GB, mm5 -> AR pixel 4-7
     * mm6 -> GB, mm7 -> AR pixel 0-3
     */

    pand_m2r (mmx_bluemask, mm0);	// mm0 = b7b6b5b4b3______
    pxor_r2r (mm4, mm4);		// mm4 = 0

    pand_m2r (mmx_greenmask, mm2);	// mm2 = g7g6g5g4g3g2____
    psrlq_i2r (3, mm0);			// mm0 = ______b7b6b5b4b3

    movq_r2r (mm2, mm7);		// mm7 = g7g6g5g4g3g2____
    movq_r2r (mm0, mm5);		// mm5 = ______b7b6b5b4b3

    pand_m2r (mmx_redmask, mm1);	// mm1 = r7r6r5r4r3______
    punpcklbw_r2r (mm4, mm2);

    psrlq_i2r (1, mm1);
    punpcklbw_r2r (mm1, mm0);

    psllq_i2r (2, mm2);

    punpckhbw_r2r (mm4, mm7);
    por_r2r (mm2, mm0);

    psllq_i2r (2, mm7);

    movntq (mm0, *image);
    punpckhbw_r2r (mm1, mm5);

    por_r2r (mm7, mm5);

    // U
    // V

    movntq (mm5, *(image+8));
}

static inline void mmx_unpack_32rgb (uint8_t * image, int cpu)
{
    /*
     * convert RGB plane to RGB packed format,
     * mm0 -> B, mm1 -> R, mm2 -> G, mm3 -> 0,
     * mm4 -> GB, mm5 -> AR pixel 4-7,
     * mm6 -> GB, mm7 -> AR pixel 0-3
     */

    pxor_r2r (mm3, mm3);
    movq_r2r (mm0, mm6);

    punpcklbw_r2r (mm2, mm6);
    movq_r2r (mm1, mm7);

    punpcklbw_r2r (mm3, mm7);
    movq_r2r (mm0, mm4);

    punpcklwd_r2r (mm7, mm6);
    movq_r2r (mm1, mm5);

    /* scheduling: this is hopeless */
    movntq (mm6, *image);
    movq_r2r (mm0, mm6);
    punpcklbw_r2r (mm2, mm6);
    punpckhwd_r2r (mm7, mm6);
    movntq (mm6, *(image+8));
    punpckhbw_r2r (mm2, mm4);
    punpckhbw_r2r (mm3, mm5);
    punpcklwd_r2r (mm5, mm4);
    movntq (mm4, *(image+16));
    movq_r2r (mm0, mm4);
    punpckhbw_r2r (mm2, mm4);
    punpckhwd_r2r (mm5, mm4);
    movntq (mm4, *(image+24));
}

static inline void mmx_unpack_32bgr (uint8_t * image, int cpu)
{
    /*
     * convert RGB plane to RGB packed format,
     * mm0 -> B, mm1 -> R, mm2 -> G, mm3 -> 0,
     * mm4 -> GB, mm5 -> AR pixel 4-7,
     * mm6 -> GB, mm7 -> AR pixel 0-3
     */

    pxor_r2r (mm3, mm3);
    movq_r2r (mm1, mm6);

    punpcklbw_r2r (mm2, mm6);
    movq_r2r (mm0, mm7);

    punpcklbw_r2r (mm3, mm7);
    movq_r2r (mm1, mm4);

    punpcklwd_r2r (mm7, mm6);
    movq_r2r (mm0, mm5);

    /* scheduling: this is hopeless */
    movntq (mm6, *image);
    movq_r2r (mm0, mm6);
    punpcklbw_r2r (mm2, mm6);
    punpckhwd_r2r (mm7, mm6);
    movntq (mm6, *(image+8));
    punpckhbw_r2r (mm2, mm4);
    punpckhbw_r2r (mm3, mm5);
    punpcklwd_r2r (mm5, mm4);
    movntq (mm4, *(image+16));
    movq_r2r (mm0, mm4);
    punpckhbw_r2r (mm2, mm4);
    punpckhwd_r2r (mm5, mm4);
    movntq (mm4, *(image+24));
}

static inline void mmx_unpack_24rgb (uint8_t * image, int cpu)
{
    /*
     * convert RGB plane to RGB packed format,
     * mm0 -> B, mm1 -> R, mm2 -> G, mm3 -> 0,
     * mm4 -> GB, mm5 -> AR pixel 4-7,
     * mm6 -> GB, mm7 -> AR pixel 0-3
     */

    pxor_r2r (mm3, mm3);
    movq_r2r (mm0, mm6);

    punpcklbw_r2r (mm2, mm6);
    movq_r2r (mm1, mm7);

    punpcklbw_r2r (mm3, mm7);
    movq_r2r (mm0, mm4);

    punpcklwd_r2r (mm7, mm6);
    movq_r2r (mm1, mm5);

    /* scheduling: this is hopeless */
    movntq (mm6, *image);
    movq_r2r (mm0, mm6);
    punpcklbw_r2r (mm2, mm6);
    punpckhwd_r2r (mm7, mm6);
    movntq (mm6, *(image+8));
    punpckhbw_r2r (mm2, mm4);
    punpckhbw_r2r (mm3, mm5);
    punpcklwd_r2r (mm5, mm4);
    movntq (mm4, *(image+16));
}

static inline void yuv420_rgb16 (yuv2rgb_t *this,
				 uint8_t * image,
				 uint8_t * py, uint8_t * pu, uint8_t * pv,
				 int cpu)
{
    int i;
    int rgb_stride = this->rgb_stride;
    int y_stride   = this->y_stride;
    int uv_stride  = this->uv_stride;
    int width      = this->source_width;
    int height     = this->source_height;
    int dst_height = this->dest_height;
    uint8_t *img;

    width >>= 3;

    if (!this->do_scale) {
      y_stride -= 8 * width;
      uv_stride -= 4 * width;

      do {

	i = width; img = image;
	do {
	  mmx_yuv2rgb (py, pu, pv); 
	  mmx_unpack_16rgb (img, cpu); 
	  py += 8;
	  pu += 4;
	  pv += 4;
	  img += 16;
	} while (--i);
	
	py += y_stride;
	image += rgb_stride;
	if (height & 1) {
	  pu += uv_stride;
	  pv += uv_stride;
	} else {
	  pu -= 4 * width;
	  pv -= 4 * width;
	}
      } while (--height);

    } else {

      scale_line_func_t scale_line = this->scale_line;
      uint8_t *y_buf, *u_buf, *v_buf;
      int      dy = 0;

      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);
      for (height = 0;; ) {

	y_buf = this->y_buffer;
	u_buf = this->u_buffer;
	v_buf = this->v_buffer;

	i = this->dest_width >> 3; img = image;
	do {
	  /* printf ("i : %d\n",i); */

	  mmx_yuv2rgb (y_buf, u_buf, v_buf); 
	  mmx_unpack_16rgb (img, cpu); 
	  y_buf += 8;
	  u_buf += 4;
	  v_buf += 4;
	  img += 16;
	} while (--i);
	
	dy += this->step_dy;
	image += rgb_stride;

	while (--dst_height > 0 && dy < 32768) {

	  xine_fast_memcpy (image, image-rgb_stride, this->dest_width*2); 

	  dy += this->step_dy;
	  image += rgb_stride;
	}

	if (dst_height <= 0)
	  break;

        do {
            dy -= 32768;

            py += y_stride;

            scale_line (py, this->y_buffer, 
                        this->dest_width, this->step_dx);

            if (height & 1) {
                pu += uv_stride;
                pv += uv_stride;
	  
                scale_line (pu, this->u_buffer,
                            this->dest_width >> 1, this->step_dx);
                scale_line (pv, this->v_buffer,
                            this->dest_width >> 1, this->step_dx);
	  
            }
            height++;
        } while( dy>=32768);
      }
    } 
}

static inline void yuv420_rgb15 (yuv2rgb_t *this,
				 uint8_t * image,
				 uint8_t * py, uint8_t * pu, uint8_t * pv,
				 int cpu)
{
    int i;
    int rgb_stride = this->rgb_stride;
    int y_stride   = this->y_stride;
    int uv_stride  = this->uv_stride;
    int width      = this->source_width;
    int height     = this->source_height;
    int dst_height = this->dest_height;
    uint8_t *img;

    width >>= 3;

    if (!this->do_scale) {
      y_stride -= 8 * width;
      uv_stride -= 4 * width;

      do {

	i = width; img = image;
	do {
	  mmx_yuv2rgb (py, pu, pv); 
	  mmx_unpack_15rgb (img, cpu); 
	  py += 8;
	  pu += 4;
	  pv += 4;
	  img += 16;
	} while (--i);
	
	py += y_stride;
	image += rgb_stride;
	if (height & 1) {
	  pu += uv_stride;
	  pv += uv_stride;
	} else {
	  pu -= 4 * width;
	  pv -= 4 * width;
	}
      } while (--height);

    } else {

      scale_line_func_t scale_line = this->scale_line;
      uint8_t *y_buf, *u_buf, *v_buf;
      int      dy = 0;

      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);
      for (height = 0;; ) {

	y_buf = this->y_buffer;
	u_buf = this->u_buffer;
	v_buf = this->v_buffer;

	i = this->dest_width >> 3; img = image;
	do {
	  /* printf ("i : %d\n",i); */

	  mmx_yuv2rgb (y_buf, u_buf, v_buf); 
	  mmx_unpack_15rgb (img, cpu); 
	  y_buf += 8;
	  u_buf += 4;
	  v_buf += 4;
	  img += 16;
	} while (--i);
	
	dy += this->step_dy;
	image += rgb_stride;

	while (--dst_height > 0 && dy < 32768) {

	  xine_fast_memcpy (image, image-rgb_stride, this->dest_width*2); 

	  dy += this->step_dy;
	  image += rgb_stride;
	}

	if (dst_height <= 0)
	  break;

        do {
            dy -= 32768;
            py += y_stride;

            scale_line (py, this->y_buffer, 
                        this->dest_width, this->step_dx);

            if (height & 1) {
                pu += uv_stride;
                pv += uv_stride;
	  
                scale_line (pu, this->u_buffer,
                            this->dest_width >> 1, this->step_dx);
                scale_line (pv, this->v_buffer,
                            this->dest_width >> 1, this->step_dx);
	  
            }
            height++;
        } while( dy>=32768 );
      }
    } 
}

static inline void yuv420_rgb24 (yuv2rgb_t *this,
				 uint8_t * image, uint8_t * py,
				 uint8_t * pu, uint8_t * pv, int cpu)
{
    int i;
    int rgb_stride = this->rgb_stride;
    int y_stride   = this->y_stride;
    int uv_stride  = this->uv_stride;
    int width      = this->source_width;
    int height     = this->source_height;
    int dst_height = this->dest_height;
    uint8_t *img;

    /* rgb_stride -= 4 * this->dest_width; */
    width >>= 3;

    if (!this->do_scale) {
      y_stride -= 8 * width;
      uv_stride -= 4 * width;

      do {
	i = width; img = image;
	do {
	  mmx_yuv2rgb (py, pu, pv);
	  mmx_unpack_24rgb (img, cpu);
	  py += 8;
	  pu += 4;
	  pv += 4;
	  img += 24;
	} while (--i);

	py += y_stride;
	image += rgb_stride;
	if (height & 1) {
	  pu += uv_stride;
	  pv += uv_stride;
	} else {
	  pu -= 4 * width;
	  pv -= 4 * width;
	}
      } while (--height);
    } else {

      scale_line_func_t scale_line = this->scale_line;
      uint8_t *y_buf, *u_buf, *v_buf;
      int      dy = 0;

      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);

      for (height = 0;; ) {

	y_buf = this->y_buffer;
	u_buf = this->u_buffer;
	v_buf = this->v_buffer;


	i = this->dest_width >> 3; img=image;
	do {
	  /* printf ("i : %d\n",i); */

	  mmx_yuv2rgb (y_buf, u_buf, v_buf); 
	  mmx_unpack_24rgb (img, cpu); 
	  y_buf += 8;
	  u_buf += 4;
	  v_buf += 4;
	  img += 24;
	} while (--i);
	
	dy += this->step_dy;
	image += rgb_stride;

	while (--dst_height > 0 && dy < 32768) {

	  xine_fast_memcpy (image, image-rgb_stride, this->dest_width*3);

	  dy += this->step_dy;
	  image += rgb_stride;
	}

	if (dst_height <= 0)
	  break;

        do {
            dy -= 32768;
            py += y_stride;
	
            scale_line (py, this->y_buffer, 
                        this->dest_width, this->step_dx);

            if (height & 1) {
                pu += uv_stride;
                pv += uv_stride;
	  
                scale_line (pu, this->u_buffer,
                            this->dest_width >> 1, this->step_dx);
                scale_line (pv, this->v_buffer,
                            this->dest_width >> 1, this->step_dx);
            }
            height++;
        } while( dy>=32768 );

      }
      
    }
}

static inline void yuv420_argb32 (yuv2rgb_t *this,
				  uint8_t * image, uint8_t * py,
				  uint8_t * pu, uint8_t * pv, int cpu)
{
    int i;
    int rgb_stride = this->rgb_stride;
    int y_stride   = this->y_stride;
    int uv_stride  = this->uv_stride;
    int width      = this->source_width;
    int height     = this->source_height;
    int dst_height = this->dest_height;
    uint8_t *img;

    /* rgb_stride -= 4 * this->dest_width; */
    width >>= 3;

    if (!this->do_scale) {
      y_stride -= 8 * width;
      uv_stride -= 4 * width;

      do {
	i = width; img = image;
	do {
	  mmx_yuv2rgb (py, pu, pv);
	  mmx_unpack_32rgb (img, cpu);
	  py += 8;
	  pu += 4;
	  pv += 4;
	  img += 32;
	} while (--i);

	py += y_stride;
	image += rgb_stride;
	if (height & 1) {
	  pu += uv_stride;
	  pv += uv_stride;
	} else {
	  pu -= 4 * width;
	  pv -= 4 * width;
	}
      } while (--height);
    } else {

      scale_line_func_t scale_line = this->scale_line;
      uint8_t *y_buf, *u_buf, *v_buf;
      int      dy = 0;

      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);

      for (height = 0;; ) {

	y_buf = this->y_buffer;
	u_buf = this->u_buffer;
	v_buf = this->v_buffer;


	i = this->dest_width >> 3; img=image;
	do {
	  /* printf ("i : %d\n",i); */

	  mmx_yuv2rgb (y_buf, u_buf, v_buf); 
	  mmx_unpack_32rgb (img, cpu); 
	  y_buf += 8;
	  u_buf += 4;
	  v_buf += 4;
	  img += 32;
	} while (--i);
	
	dy += this->step_dy;
	image += rgb_stride;

	while (--dst_height > 0 && dy < 32768) {

	  xine_fast_memcpy (image, image-rgb_stride, this->dest_width*4); 

	  dy += this->step_dy;
	  image += rgb_stride;
	}

	if (dst_height <= 0)
	  break;

        do {
            dy -= 32768;
            py += y_stride;
	
            scale_line (py, this->y_buffer, 
                        this->dest_width, this->step_dx);

            if (height & 1) {
                pu += uv_stride;
                pv += uv_stride;
	  
                scale_line (pu, this->u_buffer,
                            this->dest_width >> 1, this->step_dx);
                scale_line (pv, this->v_buffer,
                            this->dest_width >> 1, this->step_dx);
            }
            height++;
        } while( dy>=32768 );
      }
      
    }
}

static inline void yuv420_abgr32 (yuv2rgb_t *this,
				  uint8_t * image, uint8_t * py,
				  uint8_t * pu, uint8_t * pv, int cpu)
{
    int i;
    int rgb_stride = this->rgb_stride;
    int y_stride   = this->y_stride;
    int uv_stride  = this->uv_stride;
    int width      = this->source_width;
    int height     = this->source_height;
    int dst_height = this->dest_height;
    uint8_t *img;

    /* rgb_stride -= 4 * this->dest_width; */
    width >>= 3;

    if (!this->do_scale) {
      y_stride -= 8 * width;
      uv_stride -= 4 * width;

      do {
	i = width; img = image;
	do {
	  mmx_yuv2rgb (py, pu, pv);
	  mmx_unpack_32bgr (img, cpu);
	  py += 8;
	  pu += 4;
	  pv += 4;
	  img += 32;
	} while (--i);

	py += y_stride;
	image += rgb_stride;
	if (height & 1) {
	  pu += uv_stride;
	  pv += uv_stride;
	} else {
	  pu -= 4 * width;
	  pv -= 4 * width;
	}
      } while (--height);
    } else {

      scale_line_func_t scale_line = this->scale_line;
      uint8_t *y_buf, *u_buf, *v_buf;
      int      dy = 0;

      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);

      for (height = 0;; ) {

	y_buf = this->y_buffer;
	u_buf = this->u_buffer;
	v_buf = this->v_buffer;


	i = this->dest_width >> 3; img=image;
	do {
	  /* printf ("i : %d\n",i); */

	  mmx_yuv2rgb (y_buf, u_buf, v_buf); 
	  mmx_unpack_32bgr (img, cpu); 
	  y_buf += 8;
	  u_buf += 4;
	  v_buf += 4;
	  img += 32;
	} while (--i);
	
	dy += this->step_dy;
	image += rgb_stride;

	while (--dst_height > 0 && dy < 32768) {

	  xine_fast_memcpy (image, image-rgb_stride, this->dest_width*4); 

	  dy += this->step_dy;
	  image += rgb_stride;
	}

	if (dst_height <= 0)
	  break;

        do {
            dy -= 32768;
            py += y_stride;
	
            scale_line (py, this->y_buffer, 
                        this->dest_width, this->step_dx);

            if (height & 1) {
                pu += uv_stride;
                pv += uv_stride;
	  
                scale_line (pu, this->u_buffer,
                            this->dest_width >> 1, this->step_dx);
                scale_line (pv, this->v_buffer,
                            this->dest_width >> 1, this->step_dx);
            }
            height++;
        } while( dy>=32768 );

      }
      
    }
}

static void mmxext_rgb15 (yuv2rgb_t *this, uint8_t * image,
			  uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_rgb15 (this, image, py, pu, pv, CPU_MMXEXT);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmxext_rgb16 (yuv2rgb_t *this, uint8_t * image,
			  uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_rgb16 (this, image, py, pu, pv, CPU_MMXEXT);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmxext_rgb24 (yuv2rgb_t *this, uint8_t * image,
			   uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_rgb24 (this, image, py, pu, pv, CPU_MMXEXT);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmxext_argb32 (yuv2rgb_t *this, uint8_t * image,
			   uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_argb32 (this, image, py, pu, pv, CPU_MMXEXT);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmxext_abgr32 (yuv2rgb_t *this, uint8_t * image,
			   uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_abgr32 (this, image, py, pu, pv, CPU_MMXEXT);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmx_rgb15 (yuv2rgb_t *this, uint8_t * image,
		       uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_rgb15 (this, image, py, pu, pv, CPU_MMX);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmx_rgb16 (yuv2rgb_t *this, uint8_t * image,
		       uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_rgb16 (this, image, py, pu, pv, CPU_MMX);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmx_rgb24 (yuv2rgb_t *this, uint8_t * image,
		       uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_rgb24 (this, image, py, pu, pv, CPU_MMX);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmx_argb32 (yuv2rgb_t *this, uint8_t * image,
			uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_argb32 (this, image, py, pu, pv, CPU_MMX);
    emms();	/* re-initialize x86 FPU after MMX use */
}

static void mmx_abgr32 (yuv2rgb_t *this, uint8_t * image,
			uint8_t * py, uint8_t * pu, uint8_t * pv)
{
    yuv420_abgr32 (this, image, py, pu, pv, CPU_MMX);
    emms();	/* re-initialize x86 FPU after MMX use */
}

void yuv2rgb_init_mmxext (yuv2rgb_factory_t *this) {

  if (this->swapped) 
    return; /*no swapped pixel output upto now*/

  switch (this->mode) {
  case MODE_15_RGB:
    this->yuv2rgb_fun = mmxext_rgb15;
    break;
  case MODE_16_RGB:
    this->yuv2rgb_fun = mmxext_rgb16;
    break;
  case MODE_24_RGB:
    this->yuv2rgb_fun = mmxext_rgb24;
    break;
  case MODE_32_RGB:
    this->yuv2rgb_fun = mmxext_argb32;
    break;
  case MODE_32_BGR:
    this->yuv2rgb_fun = mmxext_abgr32;
    break;
  }
}

void yuv2rgb_init_mmx (yuv2rgb_factory_t *this) {

  if (this->swapped) 
    return; /*no swapped pixel output upto now*/

  switch (this->mode) {
  case MODE_15_RGB:
    this->yuv2rgb_fun = mmx_rgb15;
    break;
  case MODE_16_RGB:
    this->yuv2rgb_fun = mmx_rgb16;
    break;
  case MODE_24_RGB:
    this->yuv2rgb_fun = mmx_rgb24;
    break;
  case MODE_32_RGB:
    this->yuv2rgb_fun = mmx_argb32;
    break;
  case MODE_32_BGR:
    this->yuv2rgb_fun = mmx_abgr32;
    break;
  }
}


#endif
