/*
 * yuv2rgb_arm2.c
 * Copyright (C) 2002 Frederic 'dilb' Boulay.
 * All Rights Reserved.
 *
 * Author: Frederic Boulay <dilb@handhelds.org>
 *
 * you can redistribute this file and/or modify
 * it under the terms of the GNU General Public License (version 2)
 * as published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * The function defined in this file, are derived from work done in the xine
 * project.
 * In order to improve performance, by strongly reducing memory bandwidth
 * needed, the scaling functions are merged with the yuv2rgb function.
 */

#ifdef __arm__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "yuv2rgb.h"
#include <xine/xineutils.h>

/* Prototypes of the "local" functions available here: */
/* first prototype, function called when no scaling is needed: */
static void arm_rgb16_noscale(yuv2rgb_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
/* second prototype, function called when no horizontal scaling is needed: */
static void arm_rgb16_step_dx_32768(yuv2rgb_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
/* third prototype, function called when scaling is needed for zooming in: */
static void arm_rgb16_step_dx_inf_32768(yuv2rgb_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
/* fourth prototype, function called when scaling is needed for zooming out (between 1x and 2x): */
static void arm_rgb16_step_dx_bet_32768_65536(yuv2rgb_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
/* fifth prototype, function called when scaling is needed for zooming out (greater than 2x): */
static void arm_rgb16_step_dx_sup_65536(yuv2rgb_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
/* sixth prototype, function where the decision of the scaling function to use is made.*/
static void arm_rgb16_2 (yuv2rgb_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);




/* extern function: */

/* Function: */
void yuv2rgb_init_arm (yuv2rgb_factory_t *this)
/* This function initialise the member yuv2rgb_fun, if everything is right
the function optimised for the arm target should be used.*/
	{
	if (this->swapped)
		return; /*no swapped pixel output upto now*/

	switch (this->mode)
		{
		case MODE_16_RGB:
			this->yuv2rgb_fun = arm_rgb16_2;
			break;
		default:
		}
	}



/* local functions: */

/* Function: */
static void arm_rgb16_2 (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
/* This function takes care of applying the right scaling conversion
(yuv2rgb is included in each scaling function!)*/
	{
	if (!this->do_scale)
		{
		arm_rgb16_noscale(this, _dst, _py, _pu, _pv);
		return;
		}
	if (this->step_dx<32768)
		{
		arm_rgb16_step_dx_inf_32768(this, _dst, _py, _pu, _pv);
		return;
		}
	if (this->step_dx==32768)
		{
		arm_rgb16_step_dx_32768(this, _dst, _py, _pu, _pv);
		return;
		}
	if (this->step_dx<65536)
		{
		arm_rgb16_step_dx_bet_32768_65536(this, _dst, _py, _pu, _pv);
		return;
		}
	arm_rgb16_step_dx_sup_65536(this, _dst, _py, _pu, _pv);
	return;
	}


/* Function: */
static void arm_rgb16_noscale(yuv2rgb_t *this, uint8_t * _dst, uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
/* This function is called when the source and the destination pictures have the same size.
 In this case, scaling part is not needed.
 (This code is probably far from being optimised, in particular, the asm
 generated is not the most efficient, a pure asm version will probably
 emerge sooner or later). But at least, this version is faster than what
 was used before.*/
	{
	int height;

	height=this->dest_height;

	while (height>0)
		{
		uint16_t *r, *g, *b;
		uint8_t *py, *py2, *pu, *pv;
		uint16_t *dst, *dst2;
		int width;
		register uint8_t p1y, p1u, p1v;

		height-=2;
		width=this->dest_width;
		dst = _dst;
		dst2 = _dst + this->rgb_stride;
		py = _py;
		py2 = _py + this->y_stride;
		pu = _pu;
		pv = _pv;

		while (width>0)
			{
			width-=2;
			p1y=*py++;
			p1u=*pu++;
			p1v=*pv++;

			r = this->table_rV[p1v];
			g = (void *) (((uint8_t *)this->table_gU[p1u]) + this->table_gV[p1v]);
			b = this->table_bU[p1u];

			*dst++ = r[p1y] + g[p1y] + b[p1y];
			p1y=*py++;
			*dst++ = r[p1y] + g[p1y] + b[p1y];

			p1y=*py2++;

			*dst2++ = r[p1y] + g[p1y] + b[p1y];
			p1y=*py2++;
			*dst2++ = r[p1y] + g[p1y] + b[p1y];
			}
		_dst += (this->rgb_stride)<<1;
		_py += (this->y_stride)<<1;
		_pu += this->uv_stride;
		_pv += this->uv_stride;
		}
	}


/* Function: */
static void arm_rgb16_step_dx_inf_32768(yuv2rgb_t *this, uint8_t * _dst, uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
/* This function is called when the destination picture is bigger than the size
 of the source picture.
*/
	{
	int recal_uv, height; /* Note about recal_uv: bit0 is for
applying scale on u and v, bit1 is for increments of u and v pointers.*/
	int dy;

	dy = 0;
	height = this->dest_height;
	recal_uv=1; // 1 for evaluation of scale_line, needed the first time

	while(1)
		{
		register int dxy;
		register int dxuv;
		register uint8_t p1y, p2y;
		uint8_t dest1y, dest2y;
		register uint8_t p1u, p2u;
		register uint8_t p1v, p2v;
		uint8_t dest1u;
		uint8_t dest1v;
		int width;
		uint8_t *u_buffer;
		uint8_t *v_buffer;
		uint16_t *r, *g, *b;
		uint8_t *py, *pu, *pv;
		uint16_t *dst;

		dxy = 0;
		dxuv = 0;
		width = this->dest_width;
		u_buffer=this->u_buffer;
		v_buffer=this->v_buffer;
		dst = (uint16_t*)_dst;
		py = _py;
		pu = _pu;
		pv = _pv;

		//proceed with line scaling/conversion
		if ((recal_uv&1)!=0)
			{
			recal_uv^=1; //reset bit0.
			// init values:
			p1u = *pu++;
			p2u = *pu++;
			p1v = *pv++;
			p2v = *pv++;
			p1y = *py++;
			p2y = *py++;

			//width loop (compute all data for a line).
			while (width>0)
				{
				// proceed with u and v first (ok, and y too finally :)) [scaling part]:
				// evaluate 1u, 1v, and 2y
				//block1_uvy
				dest1u=p1u + ((dxuv*(p2u-p1u))>>15);
				dest1v=p1v + ((dxuv*(p2v-p1v))>>15);
				// as u and v are evaluated, better save them now
				*u_buffer++ = (uint8_t)dest1u;
				*v_buffer++ = (uint8_t)dest1v;

				dest1y=p1y + ((dxy*(p2y-p1y))>>15);

				dxuv += this->step_dx;
				dxy += this->step_dx;
				if (dxuv > 32768)
					{
					dxuv -= 32768;
					p1u = p2u;
					p2u = *pu++; //idee pour asm, cf cas then, un merge est possible!!!
					p1v = p2v;
					p2v = *pv++;
					}
				if (dxy > 32768)
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++;
					}
				//end block1_uvy

				//block2_y
				dest2y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				if (dxy > 32768)
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++; // idee pour asm, cf cas then, un merge est possible!!!
					}
				//end block2_y

				// proceed now with YUV2RGB [conversion part]:
				// u and v are currently in dest1u and dest1v
				// the 2 y are in dest1y and dest2y.
				// RGB(0),DST1(0), RGB(1), DST1(1)
				r = this->table_rV[dest1v];
				g = (void *) (((uint8_t *)this->table_gU[dest1u]) + this->table_gV[dest1v]);
				b = this->table_bU[dest1u];

				*dst++ = r[dest1y] + g[dest1y] + b[dest1y];
				*dst++ = r[dest2y] + g[dest2y] + b[dest2y];

				width -=2;
				}
			}
		else
			{
			// this case is simple, u and v are already evaluated,
			// Note pour moi: r, g et b pourraient etre reutilises!!

			// init values:
			p1y = *py++;
			p2y = *py++;

			//width loop (compute all data for a line).
			while (width>0)
				{
				// proceed with y [scaling part]:
				// evaluate 2y
				//block1_y
				dest1y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				if (dxy > 32768)
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++;
					}
				//end block1_uvy

				//block2_y
				dest2y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				if (dxy > 32768)
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++; // idee pour asm, cf cas then, un merge est possible!!!
					}
				//end block2_y

				// proceed now with YUV2RGB [conversion part]:
				// u and v are currently in dest1u and dest1v
				// the 2 y are in dest1y and dest2y.
				// RGB(0),DST1(0)
				dest1u=*u_buffer++;
				dest1v=*v_buffer++;
				r = this->table_rV[dest1v];
				g = (void *) (((uint8_t *)this->table_gU[dest1u]) + this->table_gV[dest1v]);
				b = this->table_bU[dest1u];

				*dst++ = r[dest1y] + g[dest1y] + b[dest1y];
				*dst++ = r[dest2y] + g[dest2y] + b[dest2y];

				width -=2;
				}
			}
		// end of line scaling/conversion
		dy += this->step_dy;
		_dst += this->rgb_stride;

		while (--height > 0 && dy < 32768)
			{
			xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*2); // *2 because of int8 cast!
			dy += this->step_dy;
			_dst += this->rgb_stride;
			}


		if (height <= 0)
			break;

		do
			{
			dy -= 32768;
			_py += this->y_stride;

			recal_uv^=2; /*bit 0 for reevaluation of scanline, bit 1 for offset.*/

			if ((recal_uv&2)==0)
				{
				_pu += this->uv_stride;
				_pv += this->uv_stride;
				recal_uv|=1; // if update, then reevaluate scanline!
				}
			}
		while( dy>=32768);
		}
	}



/* Function: */
static void arm_rgb16_step_dx_32768(yuv2rgb_t *this, uint8_t * _dst, uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
/* This function is called when the widht of the destination picture is the
 same as the size of the source picture.
*/
	{
	int recal_uv, height;
	int dy;

	dy = 0;
	height=this->dest_height;
	recal_uv=0;

	while (1)
		{
		uint16_t *r, *g, *b;
		uint8_t *py, *pu, *pv;
		uint16_t *dst;
		int width;
		register uint8_t p1y, p1u, p1v;

		width=this->dest_width;
		dst = (uint16_t*)_dst;
		py = _py;
		pu = _pu;
		pv = _pv;

		while (width>0)
			{
			width-=2;
			p1y=*py++;
			p1u=*pu++;
			p1v=*pv++;

			r = this->table_rV[p1v];
			g = (void *) (((uint8_t *)this->table_gU[p1u]) + this->table_gV[p1v]);
			b = this->table_bU[p1u];

			*dst++ = r[p1y] + g[p1y] + b[p1y];
			p1y=*py++;
			*dst++ = r[p1y] + g[p1y] + b[p1y];
			}



		// end of line scaling/conversion
		dy += this->step_dy;
		_dst += this->rgb_stride;

		while (--height > 0 && dy < 32768)
			{
			xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*2); // *2 because of int8 cast!
			dy += this->step_dy;
			_dst += this->rgb_stride;
			}


		if (height <= 0)
			break;

		do
			{
			dy -= 32768;
			_py += this->y_stride;

			recal_uv^=2; /*bit 0 for reevaluation of scanline, bit 1 for offset.*/

			if ((recal_uv&2)==0)
				{
				_pu += this->uv_stride;
				_pv += this->uv_stride;
				recal_uv|=1; // if update, then reevaluate scanline!
				}
			}
		while( dy>=32768);
		}
	}



/* Function: */
static void arm_rgb16_step_dx_bet_32768_65536(yuv2rgb_t *this, uint8_t * _dst, uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
/* This function is called when the destination picture is between the size
 of the source picture, and half its size.
*/
	{
	int recal_uv, height; /* Note about recal_uv: bit0 is for
applying scale on u and v, bit1 is for increments of u and v pointers.*/
	int dy;

	dy = 0;
	height = this->dest_height;
	recal_uv=1; // 1 for evaluation of scale_line, needed the first time

	while(1)
		{
		register int dxy;
		register int dxuv;
		register uint8_t p1y, p2y;
		uint8_t dest1y, dest2y;
		register uint8_t p1u, p2u;
		register uint8_t p1v, p2v;
		uint8_t dest1u;
		uint8_t dest1v;
		int width;
		uint8_t *u_buffer;
		uint8_t *v_buffer;
		uint16_t *r, *g, *b;
		uint8_t *py, *pu, *pv;
		uint16_t *dst;

		dxy = 0;
		dxuv = 0;
		width = this->dest_width;
		u_buffer=this->u_buffer;
		v_buffer=this->v_buffer;
		dst = (uint16_t*)_dst;
		py = _py;
		pu = _pu;
		pv = _pv;

		//proceed with line scaling/conversion
		if ((recal_uv&1)!=0)
			{
			recal_uv^=1; //reset bit0.
			// init values:
			p1u = *pu++;
			p2u = *pu++;
			p1v = *pv++;
			p2v = *pv++;
			p1y = *py++;
			p2y = *py++;

			//width loop (compute all data for a line).
			while (width>0)
				{
				// proceed with u and v first (ok, and y too finally :)) [scaling part]:
				// evaluate 1u, 1v, and 2y
				//block1_uvy
				dest1u=p1u + ((dxuv*(p2u-p1u))>>15);
				dest1v=p1v + ((dxuv*(p2v-p1v))>>15);
				// as u and v are evaluated, better save them now
				*u_buffer++ = (uint8_t)dest1u;
				*v_buffer++ = (uint8_t)dest1v;

				dest1y=p1y + ((dxy*(p2y-p1y))>>15);

				dxuv += this->step_dx;
				dxy += this->step_dx;
				if (dxuv > 65536)
					{
					dxuv -= 65536;
					p1u = *pu++;
					p2u = *pu++;
					p1v = *pv++;
					p2v = *pv++;
					}
				else
					{
					dxuv -= 32768;
					p1u = p2u;
					p2u = *pu++; //idee pour asm, cf cas then, un merge est possible!!!
					p1v = p2v;
					p2v = *pv++;
					}
				if (dxy > 65536)
					{
					dxy -= 65536;
					p1y = *py++;
					p2y = *py++;
					}
				else
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++;
					}
				//end block1_uvy

				//block2_y
				dest2y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				if (dxy > 65536)
					{
					dxy -= 65536;
					p1y = *py++;
					p2y = *py++;
					}
				else
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++; // idee pour asm, cf cas then, un merge est possible!!!
					}
				//end block2_y

				// proceed now with YUV2RGB [conversion part]:
				// u and v are currently in dest1u and dest1v
				// the 2 y are in dest1y and dest2y.
				// RGB(0),DST1(0), RGB(1), DST1(1)
				r = this->table_rV[dest1v];
				g = (void *) (((uint8_t *)this->table_gU[dest1u]) + this->table_gV[dest1v]);
				b = this->table_bU[dest1u];

				*dst++ = r[dest1y] + g[dest1y] + b[dest1y];
				*dst++ = r[dest2y] + g[dest2y] + b[dest2y];

				width -=2;
				}
			}
		else
			{
			// this case is simple, u and v are already evaluated,
			// Note pour moi: r, g et b pourraient etre reutilises!!

			// init values:
			p1y = *py++;
			p2y = *py++;

			//width loop (compute all data for a line).
			while (width>0)
				{
				// proceed with y [scaling part]:
				// evaluate 2y
				//block1_y
				dest1y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				if (dxy > 65536)
					{
					dxy -= 65536;
					p1y = *py++;
					p2y = *py++;
					}
				else
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++;
					}
				//end block1_uvy

				//block2_y
				dest2y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				if (dxy > 65536)
					{
					dxy -= 65536;
					p1y = *py++;
					p2y = *py++;
					}
				else
					{
					dxy -= 32768;
					p1y = p2y;
					p2y = *py++; // idee pour asm, cf cas then, un merge est possible!!!
					}
				//end block2_y

				// proceed now with YUV2RGB [conversion part]:
				// u and v are currently in dest1u and dest1v
				// the 2 y are in dest1y and dest2y.
				// RGB(0),DST1(0)
				dest1u=*u_buffer++;
				dest1v=*v_buffer++;
				r = this->table_rV[dest1v];
				g = (void *) (((uint8_t *)this->table_gU[dest1u]) + this->table_gV[dest1v]);
				b = this->table_bU[dest1u];

				*dst++ = r[dest1y] + g[dest1y] + b[dest1y];
				*dst++ = r[dest2y] + g[dest2y] + b[dest2y];

				width -=2;
				}
			}
		// end of line scaling/conversion
		dy += this->step_dy;
		_dst += this->rgb_stride;

		while (--height > 0 && dy < 32768)
			{
			xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*2); // *2 because of int8 cast!
			dy += this->step_dy;
			_dst += this->rgb_stride;
			}


		if (height <= 0)
			break;

		do
			{
			dy -= 32768;
			_py += this->y_stride;

			recal_uv^=2; /*bit 0 for reevaluation of scanline, bit 1 for offset.*/

			if ((recal_uv&2)==0)
				{
				_pu += this->uv_stride;
				_pv += this->uv_stride;
				recal_uv|=1; // if update, then reevaluate scanline!
				}
			}
		while( dy>=32768);
		}
	}



/* Function: */
static void arm_rgb16_step_dx_sup_65536(yuv2rgb_t *this, uint8_t * _dst, uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
/* This function is called when the destination picture is smaller than half
 the size of the source picture, and half its size.
*/
	{
	int recal_uv, height; /* Note about recal_uv: bit0 is for
applying scale on u and v, bit1 is for increments of u and v pointers.*/
	int dy;

	dy = 0;
	height = this->dest_height;
	recal_uv=1; // 1 for evaluation of scale_line, needed the first time

	while(1)
		{
		register int dxy;
		register int dxuv;
		int offdxy, offdxuv;
		register uint8_t p1y, p2y;
		uint8_t dest1y, dest2y;
		register uint8_t p1u, p2u;
		register uint8_t p1v, p2v;
		uint8_t dest1u;
		uint8_t dest1v;
		int width;
		uint8_t *u_buffer;
		uint8_t *v_buffer;
		uint16_t *r, *g, *b;
		uint8_t *py, *pu, *pv;
		uint16_t *dst;

		dxy = 0;
		dxuv = 0;
		width = this->dest_width;
		u_buffer=this->u_buffer;
		v_buffer=this->v_buffer;
		dst = (uint16_t*)_dst;
		py = _py;
		pu = _pu;
		pv = _pv;

		//proceed with line scaling/conversion
		if ((recal_uv&1)!=0)
			{
			recal_uv^=1; //reset bit0.
			// init values:
			p1u = *pu++;
			p2u = *pu++;
			p1v = *pv++;
			p2v = *pv++;
			p1y = *py++;
			p2y = *py++;

			//width loop (compute all data for a line).
			while (width>0)
				{
				// proceed with u and v first (ok, and y too finally :)) [scaling part]:
				// evaluate 1u, 1v, and 2y
				//block1_uvy
				dest1u=p1u + ((dxuv*(p2u-p1u))>>15);
				dest1v=p1v + ((dxuv*(p2v-p1v))>>15);
				// as u and v are evaluated, better save them now
				*u_buffer++ = (uint8_t)dest1u;
				*v_buffer++ = (uint8_t)dest1v;

				dest1y=p1y + ((dxy*(p2y-p1y))>>15);

				dxuv += this->step_dx;
				dxy += this->step_dx;

				offdxuv=((dxuv-1)>>15);
				dxuv-=offdxuv<<15;
				pu+=offdxuv-2;
				pv+=offdxuv-2;
				p1u = *pu++;
				p2u = *pu++;
				p1v = *pv++;
				p2v = *pv++;
				offdxy=((dxy-1)>>15);
				dxy-=offdxy<<15;
				py+=offdxy-2;
				p1y = *py++;
				p2y = *py++;

				//block2_y
				dest2y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				offdxy=((dxy-1)>>15);
				dxy-=offdxy<<15;
				py+=offdxy-2;
				p1y = *py++;
				p2y = *py++;

				// proceed now with YUV2RGB [conversion part]:
				// u and v are currently in dest1u and dest1v
				// the 2 y are in dest1y and dest2y.
				// RGB(0),DST1(0), RGB(1), DST1(1)
				r = this->table_rV[dest1v];
				g = (void *) (((uint8_t *)this->table_gU[dest1u]) + this->table_gV[dest1v]);
				b = this->table_bU[dest1u];

				*dst++ = r[dest1y] + g[dest1y] + b[dest1y];
				*dst++ = r[dest2y] + g[dest2y] + b[dest2y];

				width -=2;
				}
			}
		else
			{
			// this case is simple, u and v are already evaluated,
			// Note pour moi: r, g et b pourraient etre reutilises!!

			// init values:
			p1y = *py++;
			p2y = *py++;

			//width loop (compute all data for a line).
			while (width>0)
				{
				// proceed with y [scaling part]:
				// evaluate 2y
				//block1_y
				dest1y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				offdxy=((dxy-1)>>15);
				dxy-=offdxy<<15;
				py+=offdxy-2;
				p1y = *py++;
				p2y = *py++;

				//end block1_uvy

				//block2_y
				dest2y=p1y + ((dxy*(p2y-p1y))>>15);

				dxy += this->step_dx;
				offdxy=((dxy-1)>>15);
				dxy-=offdxy<<15;
				py+=offdxy-2;
				p1y = *py++;
				p2y = *py++;
				//end block2_y

				// proceed now with YUV2RGB [conversion part]:
				// u and v are currently in dest1u and dest1v
				// the 2 y are in dest1y and dest2y.
				// RGB(0),DST1(0)
				dest1u=*u_buffer++;
				dest1v=*v_buffer++;
				r = this->table_rV[dest1v];
				g = (void *) (((uint8_t *)this->table_gU[dest1u]) + this->table_gV[dest1v]);
				b = this->table_bU[dest1u];

				*dst++ = r[dest1y] + g[dest1y] + b[dest1y];
				*dst++ = r[dest2y] + g[dest2y] + b[dest2y];

				width -=2;
				}
			}
		// end of line scaling/conversion
		dy += this->step_dy;
		_dst += this->rgb_stride;

		while (--height > 0 && dy < 32768)
			{
			xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*2); // *2 because of int8 cast!
			dy += this->step_dy;
			_dst += this->rgb_stride;
			}


		if (height <= 0)
			break;

		do
			{
			dy -= 32768;
			_py += this->y_stride;

			recal_uv^=2; /*bit 0 for reevaluation of scanline, bit 1 for offset.*/

			if ((recal_uv&2)==0)
				{
				_pu += this->uv_stride;
				_pv += this->uv_stride;
				recal_uv|=1; // if update, then reevaluate scanline!
				}
			}
		while( dy>=32768);
		}
	}


#endif
