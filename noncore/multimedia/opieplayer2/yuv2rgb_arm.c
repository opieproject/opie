/*
 * yuv2rgb_arm.c
 * Copyright (C) 2000-2001 Project OPIE.
 * All Rights Reserved.
 *
 * Author: Robert Griebl <sandman@handhelds.org>
 *
 * This file is part of OpiePlayer2.
 *
 * OpiePlayer2 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * OpiePlayer2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef __arm__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "yuv2rgb.h"
#include <xine/xineutils.h>

#define RGB(i)							\
	U = pu[i];						\
	V = pv[i];						\
	r = this->table_rV[V];					\
	g = (void *) (((uint8_t *)this->table_gU[U]) + this->table_gV[V]);	\
	b = this->table_bU[U];

#define DST1(i)					\
	Y = py_1[2*i];                          \
	dst_1[2*i] = r[Y] + g[Y] + b[Y];	\
	Y = py_1[2*i+1];			\
	dst_1[2*i+1] = r[Y] + g[Y] + b[Y];


struct dummy {
	uint8_t *yuv [3];
	int stride [3];
};

extern void convert_yuv420_rgb565(struct dummy *picture, unsigned char *results, int w, int h) ;


static void arm_rgb16 (yuv2rgb_t *this, uint8_t * _dst,
               uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
	if ( !this-> do_scale ) {
		struct dummy d;
		d. yuv [0] = _py;
		d. yuv [1] = _pu;
		d. yuv [2] = _pv;
		d. stride [0] = this-> y_stride;
		d. stride [1] = d. stride [2] =  this-> uv_stride;

//		printf( "calling arm (%dx%d)\n", this-> dest_width, this-> dest_height );

		convert_yuv420_rgb565 ( &d, _dst, this->dest_width, this->dest_height );
		
//		printf ( "arm done\n" );
    }
    else {
		int U, V, Y;
		uint8_t * py_1, * py_2, * pu, * pv;
		uint16_t * r, * g, * b;
		uint16_t * dst_1, * dst_2;
		int width, height, dst_height;
		int dy;

		  scale_line_func_t scale_line = this->scale_line;

		  scale_line (_pu, this->u_buffer,
		this->dest_width >> 1, this->step_dx);
		  scale_line (_pv, this->v_buffer,
		this->dest_width >> 1, this->step_dx);
		  scale_line (_py, this->y_buffer, 
		this->dest_width, this->step_dx);

		  dy = 0;
		  dst_height = this->dest_height;

		  for (height = 0;; ) {
		    dst_1 = (uint16_t*)_dst;
		    py_1  = this->y_buffer;
		    pu    = this->u_buffer;
		    pv    = this->v_buffer;

		    width = this->dest_width >> 3;

		    do {
		 RGB(0);
		 DST1(0);

		 RGB(1);
		 DST1(1);
		    
		 RGB(2);
		 DST1(2);

		 RGB(3);
		 DST1(3);

		 pu += 4;
		 pv += 4;
		 py_1 += 8;
		 dst_1 += 8;
		    } while (--width);

		    dy += this->step_dy;
		    _dst += this->rgb_stride;

		    while (--dst_height > 0 && dy < 32768) {

		    xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*2); 

		    dy += this->step_dy;
		    _dst += this->rgb_stride;
		    }

		    if (dst_height <= 0)
		    break;

		    do {
		        dy -= 32768;
		        _py += this->y_stride;

		        scale_line (_py, this->y_buffer, 
		                    this->dest_width, this->step_dx);

		        if (height & 1) {
		            _pu += this->uv_stride;
		            _pv += this->uv_stride;
		 
		            scale_line (_pu, this->u_buffer,
		                        this->dest_width >> 1, this->step_dx);
		            scale_line (_pv, this->v_buffer,
		                        this->dest_width >> 1, this->step_dx);
		 
		        }
		        height++;
		    } while( dy>=32768);
		  }
    }
}
                       


void yuv2rgb_init_arm (yuv2rgb_factory_t *this) {

  if (this->swapped) 
    return; /*no swapped pixel output upto now*/

  switch (this->mode) {
  case MODE_16_RGB:
    this->yuv2rgb_fun = arm_rgb16;
    break;
  }
}



#endif
