/*
 * yuv2rgb_mlib.c
 * Copyright (C) 2000-2001 Silicon Integrated System Corp.
 * All Rights Reserved.
 *
 * Author: Juergen Keil <jk@tools.de>
 *
 * This file is part of xine, a free unix video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#if HAVE_MLIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <mlib_video.h>

#include "attributes.h"
#include "yuv2rgb.h"


static void scale_line (uint8_t *source, uint8_t *dest,
			int width, int step) {

  unsigned p1;
  unsigned p2;
  int dx;

  p1 = *source++;
  p2 = *source++;
  dx = 0;

  while (width) {

    /*
    printf ("scale_line, width = %d\n", width);
    printf ("scale_line, dx = %d, p1 = %d, p2 = %d\n", dx, p1, p2);
    */
 
    *dest = (p1 * (32768 - dx) + p2 * dx) / 32768;

    dx += step;
    while (dx > 32768) {
      dx -= 32768;
      p1 = p2;
      p2 = *source++;
    }

    dest ++;
    width --;
  }
}
			


static void mlib_yuv420_rgb24 (yuv2rgb_t *this,
			       uint8_t * image, uint8_t * py,
			       uint8_t * pu, uint8_t * pv)
{
  int dst_height;
  int dy;
  mlib_status mlib_stat;

  if (this->do_scale) {
    dy = 0;
    dst_height = this->dest_height;

    for (;;) {
      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      pu += this->uv_stride;

      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      pv += this->uv_stride;

      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);
      py += this->y_stride;
      scale_line (py, this->y_buffer + this->dest_width, 
		  this->dest_width, this->step_dx);
      py += this->y_stride;
	
      mlib_stat = mlib_VideoColorYUV2RGB420(image,
					    this->y_buffer, 
					    this->u_buffer,
					    this->v_buffer,
					    this->dest_width & ~1, 2,
					    this->rgb_stride,
					    this->dest_width,
					    this->dest_width >> 1);
      dy += this->step_dy;
      image += this->rgb_stride;
      
      while (--dst_height > 0 && dy < 32768) {
	memcpy (image, (uint8_t*)image-this->rgb_stride, this->dest_width*6);
	dy += this->step_dy;
	image += this->rgb_stride;
      }

      if (dst_height <= 0)
	break;

      dy -= 32768;

      dy += this->step_dy;
      image += this->rgb_stride;
      
      while (--dst_height > 0 && dy < 32768) {
	memcpy (image, (uint8_t*)image-this->rgb_stride, this->dest_width*3);
	dy += this->step_dy;
	image += this->rgb_stride;
      }

      if (dst_height <= 0)
	break;

      dy -= 32768;
    }
  } else {
    mlib_stat = mlib_VideoColorYUV2RGB420(image, py, pu, pv,
					  this->source_width,
					  this->source_height,
					  this->rgb_stride,
					  this->y_stride,
					  this->uv_stride);
  }
}

static void mlib_yuv420_argb32 (yuv2rgb_t *this,
				uint8_t * image, uint8_t * py,
				uint8_t * pu, uint8_t * pv)
{
  int dst_height;
  int dy;
  mlib_status mlib_stat;

  if (this->do_scale) {
    dy = 0;
    dst_height = this->dest_height;

    for (;;) {
      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      pu += this->uv_stride;

      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      pv += this->uv_stride;

      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);
      py += this->y_stride;
      scale_line (py, this->y_buffer + this->dest_width, 
		  this->dest_width, this->step_dx);
      py += this->y_stride;
	
      mlib_stat = mlib_VideoColorYUV2ARGB420(image,
					     this->y_buffer, 
					     this->u_buffer,
					     this->v_buffer,
					     this->dest_width & ~1, 2,
					     this->rgb_stride,
					     this->dest_width,
					     this->dest_width >> 1);
      dy += this->step_dy;
      image += this->rgb_stride;
      
      while (--dst_height > 0 && dy < 32768) {
	memcpy (image, (uint8_t*)image-this->rgb_stride, this->dest_width*8);
	dy += this->step_dy;
	image += this->rgb_stride;
      }
      
      if (dst_height <= 0)
	break;

      dy -= 32768;

      dy += this->step_dy;
      image += this->rgb_stride;
      
      while (--dst_height > 0 && dy < 32768) {
	memcpy (image, (uint8_t*)image-this->rgb_stride, this->dest_width*4);
	dy += this->step_dy;
	image += this->rgb_stride;
      }

      if (dst_height <= 0)
	break;

      dy -= 32768;
    }
  } else {
    mlib_stat = mlib_VideoColorYUV2ARGB420(image, py, pu, pv,
					   this->source_width,
					   this->source_height,
					   this->rgb_stride,
					   this->y_stride,
					   this->uv_stride);
  }
}

static void mlib_yuv420_abgr32 (yuv2rgb_t *this,
				uint8_t * image, uint8_t * py,
				uint8_t * pu, uint8_t * pv)
{
  int dst_height;
  int dy;
  mlib_status mlib_stat;

  if (this->do_scale) {
    dy = 0;
    dst_height = this->dest_height;

    for (;;) {
      scale_line (pu, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
      pu += this->uv_stride;

      scale_line (pv, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
      pv += this->uv_stride;

      scale_line (py, this->y_buffer, 
		  this->dest_width, this->step_dx);
      py += this->y_stride;
      scale_line (py, this->y_buffer + this->dest_width, 
		  this->dest_width, this->step_dx);
      py += this->y_stride;
	
      mlib_stat = mlib_VideoColorYUV2ABGR420(image,
					     this->y_buffer, 
					     this->u_buffer,
					     this->v_buffer,
					     this->dest_width & ~1, 2,
					     this->rgb_stride,
					     this->dest_width,
					     this->dest_width >> 1);
      dy += this->step_dy;
      image += this->rgb_stride;
      
      while (--dst_height > 0 && dy < 32768) {
	memcpy (image, (uint8_t*)image-this->rgb_stride, this->dest_width*8);
	dy += this->step_dy;
	image += this->rgb_stride;
      }

      if (dst_height <= 0)
	break;

      dy -= 32768;

      dy += this->step_dy;
      image += this->rgb_stride;
      
      while (--dst_height > 0 && dy < 32768) {
	memcpy (image, (uint8_t*)image-this->rgb_stride, this->dest_width*4);
	dy += this->step_dy;
	image += this->rgb_stride;
      }

      if (dst_height <= 0)
	break;

      dy -= 32768;
    }
  } else {
    mlib_stat = mlib_VideoColorYUV2ABGR420(image, py, pu, pv,
					   this->source_width,
					   this->source_height,
					   this->rgb_stride,
					   this->y_stride,
					   this->uv_stride);
  }
}


void yuv2rgb_init_mlib (yuv2rgb_factory_t *this) {

  if (this->swapped) return; /*no swapped pixel output upto now*/

  switch (this->mode) {
  case MODE_24_RGB:
    this->yuv2rgb_fun = mlib_yuv420_rgb24;
    break;
  case MODE_32_RGB:
    this->yuv2rgb_fun = mlib_yuv420_argb32;
    break;
  case MODE_32_BGR:
    this->yuv2rgb_fun = mlib_yuv420_abgr32;
    break;
  }
}


#endif	/* HAVE_MLIB */
