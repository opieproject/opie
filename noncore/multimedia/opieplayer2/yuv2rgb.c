/*
 * yuv2rgb.c
 *
 * This file is part of xine, a unix video player.
 *
 * based on work from mpeg2dec:
 * Copyright (C) 1999-2001 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
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
 *
 * $Id: yuv2rgb.c,v 1.2 2002-08-04 20:23:19 sandman Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "yuv2rgb.h"
#include <xine/xineutils.h>


static int prof_scale_line = -1;

static scale_line_func_t find_scale_line_func(int step);


const int32_t Inverse_Table_6_9[8][4] = {
  {117504, 138453, 13954, 34903}, /* no sequence_display_extension */
  {117504, 138453, 13954, 34903}, /* ITU-R Rec. 709 (1990) */
  {104597, 132201, 25675, 53279}, /* unspecified */
  {104597, 132201, 25675, 53279}, /* reserved */
  {104448, 132798, 24759, 53109}, /* FCC */
  {104597, 132201, 25675, 53279}, /* ITU-R Rec. 624-4 System B, G */
  {104597, 132201, 25675, 53279}, /* SMPTE 170M */
  {117579, 136230, 16907, 35559}  /* SMPTE 240M (1987) */
};


static void *my_malloc_aligned (size_t alignment, size_t size, void **chunk) {

  char *pMem;

  pMem = xine_xmalloc (size+alignment);

  *chunk = pMem;

  while ((int) pMem % alignment)
    pMem++;

  return pMem;
}


static int yuv2rgb_configure (yuv2rgb_t *this, 
			      int source_width, int source_height,
			      int y_stride, int uv_stride,
			      int dest_width, int dest_height,
			      int rgb_stride) {
  /*
  printf ("yuv2rgb setup (%d x %d => %d x %d)\n", source_width, source_height,
	  dest_width, dest_height);
	  */
  if (prof_scale_line == -1)
    prof_scale_line = xine_profiler_allocate_slot("xshm scale line");

  this->source_width  = source_width;
  this->source_height = source_height;
  this->y_stride      = y_stride;
  this->uv_stride     = uv_stride;
  this->dest_width    = dest_width;
  this->dest_height   = dest_height;
  this->rgb_stride    = rgb_stride;
  
  if (this->y_chunk) {
    free (this->y_chunk);
    this->y_buffer = this->y_chunk = NULL;
  }
  if (this->u_chunk) {
    free (this->u_chunk);
    this->u_buffer = this->u_chunk = NULL;
  }
  if (this->v_chunk) {
    free (this->v_chunk);
    this->v_buffer = this->v_chunk = NULL;
  }

  
  this->step_dx = source_width  * 32768 / dest_width;
  this->step_dy = source_height * 32768 / dest_height;
    
  this->scale_line = find_scale_line_func(this->step_dx);

  if ((source_width == dest_width) && (source_height == dest_height)) {
    this->do_scale = 0;

    /*
     * space for two y-lines (for yuv2rgb_mlib)
     * u,v subsampled 2:1
     */
    this->y_buffer = my_malloc_aligned (16, 2*dest_width, &this->y_chunk);
    if (!this->y_buffer)
      return 0;
    this->u_buffer = my_malloc_aligned (16, (dest_width+1)/2, &this->u_chunk);
    if (!this->u_buffer)
      return 0;
    this->v_buffer = my_malloc_aligned (16, (dest_width+1)/2, &this->v_chunk);
    if (!this->v_buffer)
      return 0;

  } else {
    this->do_scale = 1;
    
    /*
     * space for two y-lines (for yuv2rgb_mlib)
     * u,v subsampled 2:1
     */
    this->y_buffer = my_malloc_aligned (16, 2*dest_width, &this->y_chunk);
    if (!this->y_buffer)
      return 0;
    this->u_buffer = my_malloc_aligned (16, (dest_width+1)/2, &this->u_chunk);
    if (!this->u_buffer)
      return 0;
    this->v_buffer = my_malloc_aligned (16, (dest_width+1)/2, &this->v_chunk);
    if (!this->v_buffer)
      return 0;
  }
  return 1;
}


static void scale_line_gen (uint8_t *source, uint8_t *dest,
			    int width, int step) {

  /*
   * scales a yuv source row to a dest row, with interpolation
   * (good quality, but slow)
   */
  int p1;
  int p2;
  int dx;

  xine_profiler_start_count(prof_scale_line);

  p1 = *source++;
  p2 = *source++;
  dx = 0;

  /*
   * the following code has been optimized by Scott Smith <ssmith@akamai.com>:
   *
   * ok now I have a meaningful optimization for yuv2rgb.c:scale_line_gen.
   * it removes the loop from within the while() loop by separating it out
   * into 3 cases: where you are enlarging the line (<32768), where you are
   * between 50% and 100% of the original line (<=65536), and where you are
   * shrinking it by a lot.  anyways, I went from 200 delivered / 100+
   * skipped to 200 delivered / 80 skipped for the enlarging case.  I
   * noticed when looking at the assembly that the compiler was able to
   * unroll these while(width) loops, whereas before it was trying to
   * unroll the while(dx>32768) loops.  so the compiler is better able to
   * deal with this code.
   */


  if (step < 32768) {
    while (width) {
      *dest = p1 + (((p2-p1) * dx)>>15);

      dx += step;
      if (dx > 32768) {
	dx -= 32768;
	p1 = p2;
	p2 = *source++;
      }
      
      dest ++;
      width --;
    }
  } else if (step <= 65536) {
    while (width) {
      *dest = p1 + (((p2-p1) * dx)>>15);

      dx += step;
      if (dx > 65536) {
	dx -= 65536;
	p1 = *source++;
	p2 = *source++;
      } else {
	dx -= 32768;
	p1 = p2;
	p2 = *source++;
      }
      
      dest ++;
      width --;
    }
  } else {
    while (width) {
      int offs;

      *dest = p1 + (((p2-p1) * dx)>>15);

      dx += step;
      offs=((dx-1)>>15);
      dx-=offs<<15;
      source+=offs-2;
      p1=*source++;
      p2=*source++;
      dest ++;
      width --;
    }
  }
  xine_profiler_stop_count(prof_scale_line);




}

/*
 * Interpolates 16 output pixels from 15 source pixels using shifts.
 * Useful for scaling a PAL mpeg2 dvd input source to 4:3 format on
 * a monitor using square pixels.
 * (720 x 576 ==> 768 x 576)
 */
static void scale_line_15_16 (uint8_t *source, uint8_t *dest,
			      int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 16) >= 0) {
    p1 = source[0];
    dest[0] = p1;
    p2 = source[1];
    dest[1] = (1*p1 + 7*p2) >> 3;
    p1 = source[2];
    dest[2] = (1*p2 + 7*p1) >> 3;
    p2 = source[3];
    dest[3] = (1*p1 + 3*p2) >> 2;
    p1 = source[4];
    dest[4] = (1*p2 + 3*p1) >> 2;
    p2 = source[5];
    dest[5] = (3*p1 + 5*p2) >> 3;
    p1 = source[6];
    dest[6] = (3*p2 + 5*p1) >> 3;
    p2 = source[7];
    dest[7] = (1*p1 + 1*p1) >> 1;
    p1 = source[8];
    dest[8] = (1*p2 + 1*p1) >> 1;
    p2 = source[9];
    dest[9] = (5*p1 + 3*p2) >> 3;
    p1 = source[10];
    dest[10] = (5*p2 + 3*p1) >> 3;
    p2 = source[11];
    dest[11] = (3*p1 + 1*p2) >> 2;
    p1 = source[12];
    dest[12] = (3*p2 + 1*p1) >> 2;
    p2 = source[13];
    dest[13] = (7*p1 + 1*p2) >> 3;
    p1 = source[14];
    dest[14] = (7*p2 + 1*p1) >> 3;
    dest[15] = p1;
    source += 15;
    dest += 16;
  }

  if ((width += 16) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 7*source[1]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[1] + 7*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[2] + 3*source[3]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[3] + 3*source[4]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[4] + 5*source[5]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[5] + 5*source[6]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[6] + 1*source[7]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (1*source[7] + 1*source[8]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (5*source[8] + 3*source[9]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[9] + 3*source[10]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[10] + 1*source[11]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[11] + 1*source[12]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (7*source[12] + 1*source[13]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[13] + 1*source[14]) >> 3;
 done:
  xine_profiler_stop_count(prof_scale_line);
}


/*
 * Interpolates 53 output pixels from 45 source pixels using shifts.
 * Useful for scaling a NTSC mpeg2 dvd input source to 16:9 display
 * resulution
 * fullscreen resolution, or to 16:9 format on a monitor using square
 * pixels.
 * (720 x 480 ==> 848 x 480)
 */
static void scale_line_45_53 (uint8_t *source, uint8_t *dest,
			      int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 53) >= 0) {
    p1 = source[0];
    p2 = source[1];
    dest[0] = p1;
    dest[1] = (1*p1 + 7*p2) >> 3;
    p1 = source[2];
    dest[2] = (1*p2 + 3*p1) >> 2;
    p2 = source[3];
    dest[3] = (1*p1 + 1*p2) >> 1;
    p1 = source[4];
    dest[4] = (5*p2 + 3*p1) >> 3;
    p2 = source[5];
    dest[5] = (3*p1 + 1*p2) >> 2;
    p1 = source[6];
    dest[6] = (7*p2 + 1*p1) >> 3;
    dest[7] = p1;
    p2 = source[7];
    dest[8] = (1*p1 + 3*p2) >> 2;
    p1 = source[8];
    dest[9] = (3*p2 + 5*p1) >> 3;
    p2 = source[9];
    dest[10] = (1*p1 + 1*p2) >> 1;
    p1 = source[10];
    dest[11] = (5*p2 + 3*p1) >> 3;
    p2 = source[11];
    dest[12] = (3*p1 + 1*p2) >> 2;
    p1 = source[12];
    dest[13] = p2;
    dest[14] = (1*p2 + 7*p1) >> 3;
    p2 = source[13];
    dest[15] = (1*p1 + 3*p2) >> 2;
    p1 = source[14];
    dest[16] = (3*p2 + 5*p1) >> 3;
    p2 = source[15];
    dest[17] = (5*p1 + 3*p2) >> 3;
    p1 = source[16];
    dest[18] = (3*p2 + 1*p1) >> 2;
    p2 = source[17];
    dest[19] = (7*p1 + 1*p2) >> 3;
    dest[20] = p2;
    p1 = source[18];
    dest[21] = (1*p2 + 7*p1) >> 3;
    p2 = source[19];
    dest[22] = (3*p1 + 5*p2) >> 3;
    p1 = source[20];
    dest[23] = (1*p2 + 1*p1) >> 1;
    p2 = source[21];
    dest[24] = (5*p1 + 3*p2) >> 3;
    p1 = source[22];
    dest[25] = (3*p2 + 1*p1) >> 2;
    p2 = source[23];
    dest[26] = (7*p1 + 1*p2) >> 3;
    dest[27] = (1*p1 + 7*p2) >> 3;
    p1 = source[24];
    dest[28] = (1*p2 + 3*p1) >> 2;
    p2 = source[25];
    dest[29] = (3*p1 + 5*p2) >> 3;
    p1 = source[26];
    dest[30] = (1*p2 + 1*p1) >> 1;
    p2 = source[27];
    dest[31] = (5*p1 + 3*p2) >> 3;
    p1 = source[28];
    dest[32] = (7*p2 + 1*p1) >> 3;
    p2 = source[29];
    dest[33] = p1;
    dest[34] = (1*p1 + 7*p2) >> 3;
    p1 = source[30];
    dest[35] = (1*p2 + 3*p1) >> 2;
    p2 = source[31];
    dest[36] = (3*p1 + 5*p2) >> 3;
    p1 = source[32];
    dest[37] = (5*p2 + 3*p1) >> 3;
    p2 = source[33];
    dest[38] = (3*p1 + 1*p2) >> 2;
    p1 = source[34];
    dest[39] = (7*p2 + 1*p1) >> 3;
    dest[40] = p1;
    p2 = source[35];
    dest[41] = (1*p1 + 3*p2) >> 2;
    p1 = source[36];
    dest[42] = (3*p2 + 5*p1) >> 3;
    p2 = source[37];
    dest[43] = (1*p1 + 1*p2) >> 1;
    p1 = source[38];
    dest[44] = (5*p2 + 3*p1) >> 3;
    p2 = source[39];
    dest[45] = (3*p1 + 1*p2) >> 2;
    p1 = source[40];
    dest[46] = p2;
    dest[47] = (1*p2 + 7*p1) >> 3;
    p2 = source[41];
    dest[48] = (1*p1 + 3*p2) >> 2;
    p1 = source[42];
    dest[49] = (3*p2 + 5*p1) >> 3;
    p2 = source[43];
    dest[50] = (1*p1 + 1*p2) >> 1;
    p1 = source[44];
    dest[51] = (3*p2 + 1*p1) >> 2;
    p2 = source[45];
    dest[52] = (7*p1 + 1*p2) >> 3;
    source += 45;
    dest += 53;
  }

  if ((width += 53) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 7*source[1]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[1] + 3*source[2]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[2] + 1*source[3]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (5*source[3] + 3*source[4]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[4] + 1*source[5]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (7*source[5] + 1*source[6]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[6];
  if (--width <= 0) goto done;
  *dest++ = (1*source[6] + 3*source[7]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[7] + 5*source[8]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[8] + 1*source[9]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (5*source[9] + 3*source[10]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[10] + 1*source[11]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = source[11];
  if (--width <= 0) goto done;
  *dest++ = (1*source[11] + 7*source[12]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[12] + 3*source[13]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[13] + 5*source[14]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[14] + 3*source[15]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[15] + 1*source[16]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (7*source[16] + 1*source[17]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[17];
  if (--width <= 0) goto done;
  *dest++ = (1*source[17] + 7*source[18]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[18] + 5*source[19]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[19] + 1*source[20]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (5*source[20] + 3*source[21]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[21] + 1*source[22]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (7*source[22] + 1*source[23]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[22] + 7*source[23]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[23] + 3*source[24]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[24] + 5*source[25]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[25] + 1*source[26]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (5*source[26] + 3*source[27]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[27] + 1*source[28]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[28];
  if (--width <= 0) goto done;
  *dest++ = (1*source[28] + 7*source[29]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[29] + 3*source[30]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[30] + 5*source[31]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[31] + 3*source[32]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[32] + 1*source[33]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (7*source[33] + 1*source[34]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[34];
  if (--width <= 0) goto done;
  *dest++ = (1*source[34] + 3*source[35]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[35] + 5*source[36]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[36] + 1*source[37]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (5*source[37] + 3*source[38]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[38] + 1*source[39]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = source[39];
  if (--width <= 0) goto done;
  *dest++ = (1*source[39] + 7*source[40]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[40] + 3*source[41]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[41] + 5*source[42]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[42] + 1*source[43]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (3*source[43] + 1*source[44]) >> 2;
 done:

  xine_profiler_stop_count(prof_scale_line);
}


/*
 * Interpolates 64 output pixels from 45 source pixels using shifts.
 * Useful for scaling a PAL mpeg2 dvd input source to 1024x768
 * fullscreen resolution, or to 16:9 format on a monitor using square
 * pixels.
 * (720 x 576 ==> 1024 x 576)
 */
static void scale_line_45_64 (uint8_t *source, uint8_t *dest,
			     int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 64) >= 0) {
    p1 = source[0];
    p2 = source[1];
    dest[0] = p1;
    dest[1] = (1*p1 + 3*p2) >> 2;
    p1 = source[2];
    dest[2] = (5*p2 + 3*p1) >> 3;
    p2 = source[3];
    dest[3] = (7*p1 + 1*p2) >> 3;
    dest[4] = (1*p1 + 3*p2) >> 2;
    p1 = source[4];
    dest[5] = (1*p2 + 1*p1) >> 1;
    p2 = source[5];
    dest[6] = (3*p1 + 1*p2) >> 2;
    dest[7] = (1*p1 + 7*p2) >> 3;
    p1 = source[6];
    dest[8] = (3*p2 + 5*p1) >> 3;
    p2 = source[7];
    dest[9] = (5*p1 + 3*p2) >> 3;
    p1 = source[8];
    dest[10] = p2;
    dest[11] = (1*p2 + 3*p1) >> 2;
    p2 = source[9];
    dest[12] = (5*p1 + 3*p2) >> 3;
    p1 = source[10];
    dest[13] = (7*p2 + 1*p1) >> 3;
    dest[14] = (1*p2 + 7*p1) >> 3;
    p2 = source[11];
    dest[15] = (1*p1 + 1*p2) >> 1;
    p1 = source[12];
    dest[16] = (3*p2 + 1*p1) >> 2;
    dest[17] = p1;
    p2 = source[13];
    dest[18] = (3*p1 + 5*p2) >> 3;
    p1 = source[14];
    dest[19] = (5*p2 + 3*p1) >> 3;
    p2 = source[15];
    dest[20] = p1;
    dest[21] = (1*p1 + 3*p2) >> 2;
    p1 = source[16];
    dest[22] = (1*p2 + 1*p1) >> 1;
    p2 = source[17];
    dest[23] = (7*p1 + 1*p2) >> 3;
    dest[24] = (1*p1 + 7*p2) >> 3;
    p1 = source[18];
    dest[25] = (3*p2 + 5*p1) >> 3;
    p2 = source[19];
    dest[26] = (3*p1 + 1*p2) >> 2;
    dest[27] = p2;
    p1 = source[20];
    dest[28] = (3*p2 + 5*p1) >> 3;
    p2 = source[21];
    dest[29] = (5*p1 + 3*p2) >> 3;
    p1 = source[22];
    dest[30] = (7*p2 + 1*p1) >> 3;
    dest[31] = (1*p2 + 3*p1) >> 2;
    p2 = source[23];
    dest[32] = (1*p1 + 1*p2) >> 1;
    p1 = source[24];
    dest[33] = (3*p2 + 1*p1) >> 2;
    dest[34] = (1*p2 + 7*p1) >> 3;
    p2 = source[25];
    dest[35] = (3*p1 + 5*p2) >> 3;
    p1 = source[26];
    dest[36] = (3*p2 + 1*p1) >> 2;
    p2 = source[27];
    dest[37] = p1;
    dest[38] = (1*p1 + 3*p2) >> 2;
    p1 = source[28];
    dest[39] = (5*p2 + 3*p1) >> 3;
    p2 = source[29];
    dest[40] = (7*p1 + 1*p2) >> 3;
    dest[41] = (1*p1 + 7*p2) >> 3;
    p1 = source[30];
    dest[42] = (1*p2 + 1*p1) >> 1;
    p2 = source[31];
    dest[43] = (3*p1 + 1*p2) >> 2;
    dest[44] = (1*p1 + 7*p2) >> 3;
    p1 = source[32];
    dest[45] = (3*p2 + 5*p1) >> 3;
    p2 = source[33];
    dest[46] = (5*p1 + 3*p2) >> 3;
    p1 = source[34];
    dest[47] = p2;
    dest[48] = (1*p2 + 3*p1) >> 2;
    p2 = source[35];
    dest[49] = (1*p1 + 1*p2) >> 1;
    p1 = source[36];
    dest[50] = (7*p2 + 1*p1) >> 3;
    dest[51] = (1*p2 + 7*p1) >> 3;
    p2 = source[37];
    dest[52] = (1*p1 + 1*p2) >> 1;
    p1 = source[38];
    dest[53] = (3*p2 + 1*p1) >> 2;
    dest[54] = p1;
    p2 = source[39];
    dest[55] = (3*p1 + 5*p2) >> 3;
    p1 = source[40];
    dest[56] = (5*p2 + 3*p1) >> 3;
    p2 = source[41];
    dest[57] = (7*p1 + 1*p2) >> 3;
    dest[58] = (1*p1 + 3*p2) >> 2;
    p1 = source[42];
    dest[59] = (1*p2 + 1*p1) >> 1;
    p2 = source[43];
    dest[60] = (7*p1 + 1*p2) >> 3;
    dest[61] = (1*p1 + 7*p2) >> 3;
    p1 = source[44];
    dest[62] = (3*p2 + 5*p1) >> 3;
    p2 = source[45];
    dest[63] = (3*p1 + 1*p2) >> 2;
    source += 45;
    dest += 64;
  }

  if ((width += 64) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 3*source[1]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (5*source[1] + 3*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[2] + 1*source[3]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[2] + 3*source[3]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[3] + 1*source[4]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (3*source[4] + 1*source[5]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[4] + 7*source[5]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[5] + 5*source[6]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[6] + 3*source[7]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[7];
  if (--width <= 0) goto done;
  *dest++ = (1*source[7] + 3*source[8]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (5*source[8] + 3*source[9]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[9] + 1*source[10]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[9] + 7*source[10]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[10] + 1*source[11]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (3*source[11] + 1*source[12]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = source[12];
  if (--width <= 0) goto done;
  *dest++ = (3*source[12] + 5*source[13]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[13] + 3*source[14]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[14];
  if (--width <= 0) goto done;
  *dest++ = (1*source[14] + 3*source[15]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[15] + 1*source[16]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (7*source[16] + 1*source[17]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[16] + 7*source[17]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[17] + 5*source[18]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[18] + 1*source[19]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = source[19];
  if (--width <= 0) goto done;
  *dest++ = (3*source[19] + 5*source[20]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[20] + 3*source[21]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[21] + 1*source[22]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[21] + 3*source[22]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[22] + 1*source[23]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (3*source[23] + 1*source[24]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[23] + 7*source[24]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[24] + 5*source[25]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[25] + 1*source[26]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = source[26];
  if (--width <= 0) goto done;
  *dest++ = (1*source[26] + 3*source[27]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (5*source[27] + 3*source[28]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[28] + 1*source[29]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[28] + 7*source[29]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[29] + 1*source[30]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (3*source[30] + 1*source[31]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[30] + 7*source[31]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[31] + 5*source[32]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[32] + 3*source[33]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[33];
  if (--width <= 0) goto done;
  *dest++ = (1*source[33] + 3*source[34]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[34] + 1*source[35]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (7*source[35] + 1*source[36]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[35] + 7*source[36]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[36] + 1*source[37]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (3*source[37] + 1*source[38]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = source[38];
  if (--width <= 0) goto done;
  *dest++ = (3*source[38] + 5*source[39]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[39] + 3*source[40]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[40] + 1*source[41]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[40] + 3*source[41]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[41] + 1*source[42]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (7*source[42] + 1*source[43]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[42] + 7*source[43]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[43] + 5*source[44]) >> 3;
 done:

  xine_profiler_stop_count(prof_scale_line);
}


/*
 * Interpolates 16 output pixels from 9 source pixels using shifts.
 * Useful for scaling a PAL mpeg2 dvd input source to 1280x1024 fullscreen
 * (720 x 576 ==> 1280 x XXX)
 */
static void scale_line_9_16 (uint8_t *source, uint8_t *dest,
			     int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 16) >= 0) {
    p1 = source[0];
    p2 = source[1];
    dest[0] = p1;
    dest[1] = (1*p1 + 1*p2) >> 1;
    p1 = source[2];
    dest[2] = (7*p2 + 1*p1) >> 3;
    dest[3] = (3*p2 + 5*p1) >> 3;
    p2 = source[3];
    dest[4] = (3*p1 + 1*p2) >> 2;
    dest[5] = (1*p1 + 3*p2) >> 2;
    p1 = source[4];
    dest[6] = (5*p2 + 3*p1) >> 3;
    dest[7] = (1*p2 + 7*p1) >> 3;
    p2 = source[5];
    dest[8] = (1*p1 + 1*p2) >> 1;
    p1 = source[6];
    dest[9] = p2;
    dest[10] = (3*p2 + 5*p1) >> 3;
    p2 = source[7];
    dest[11] = (7*p1 + 1*p2) >> 3;
    dest[12] = (1*p1 + 3*p2) >> 2;
    p1 = source[8];
    dest[13] = (3*p2 + 1*p1) >> 2;
    dest[14] = (1*p2 + 7*p1) >> 3;
    p2 = source[9];
    dest[15] = (5*p1 + 3*p2) >> 3;
    source += 9;
    dest += 16;
  }

  if ((width += 16) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 1*source[1]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (7*source[1] + 1*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[1] + 5*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[2] + 1*source[3]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[2] + 3*source[3]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (5*source[3] + 3*source[4]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[3] + 7*source[4]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[4] + 1*source[5]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = source[5];
  if (--width <= 0) goto done;
  *dest++ = (3*source[5] + 5*source[6]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[6] + 1*source[7]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[6] + 3*source[7]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[7] + 1*source[8]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[7] + 7*source[8]) >> 3;
done:
  xine_profiler_stop_count(prof_scale_line);
}


/*
 * Interpolates 12 output pixels from 11 source pixels using shifts.
 * Useful for scaling a PAL vcd input source to 4:3 display format.
 */
static void scale_line_11_12 (uint8_t *source, uint8_t *dest,
			     int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 12) >= 0) {
    p1 = source[0];
    p2 = source[1];
    dest[0] = p1;
    dest[1] = (1*p1 + 7*p2) >> 3;
    p1 = source[2];
    dest[2] = (1*p2 + 7*p1) >> 3;
    p2 = source[3];
    dest[3] = (1*p1 + 3*p2) >> 2;
    p1 = source[4];
    dest[4] = (3*p2 + 5*p1) >> 3;
    p2 = source[5];
    dest[5] = (3*p1 + 5*p2) >> 3;
    p1 = source[6];
    dest[6] = (1*p2 + 1*p1) >> 1;
    p2 = source[7];
    dest[7] = (5*p1 + 3*p2) >> 3;
    p1 = source[8];
    dest[8] = (5*p2 + 3*p1) >> 3;
    p2 = source[9];
    dest[9] = (3*p1 + 1*p2) >> 2;
    p1 = source[10];
    dest[10] = (7*p2 + 1*p1) >> 3;
    p2 = source[11];
    dest[11] = (7*p1 + 1*p2) >> 3;
    source += 11;
    dest += 12;
  }

  if ((width += 12) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 7*source[1]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[1] + 7*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[2] + 3*source[3]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[3] + 5*source[4]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[4] + 5*source[5]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[5] + 1*source[6]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (5*source[6] + 3*source[7]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[7] + 3*source[8]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[8] + 1*source[9]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (7*source[9] + 1*source[10]) >> 3;
done:

  xine_profiler_stop_count(prof_scale_line);
}


/*
 * Interpolates 24 output pixels from 11 source pixels using shifts.
 * Useful for scaling a PAL vcd input source to 4:3 display format
 * at 2*zoom.
 */
static void scale_line_11_24 (uint8_t *source, uint8_t *dest,
			     int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 24) >= 0) {
    p1 = source[0];
    p2 = source[1];
    dest[0] = p1;
    dest[1] = (1*p1 + 1*p2) >> 1;
    dest[2] = (1*p1 + 7*p2) >> 3;
    p1 = source[2];
    dest[3] = (5*p2 + 3*p1) >> 3;
    dest[4] = (1*p2 + 7*p1) >> 3;
    p2 = source[3];
    dest[5] = (3*p1 + 1*p2) >> 2;
    dest[6] = (1*p1 + 3*p2) >> 2;
    p1 = source[4];
    dest[7] = (3*p2 + 1*p1) >> 2;
    dest[8] = (3*p2 + 5*p1) >> 3;
    p2 = source[5];
    dest[9] = (7*p1 + 1*p2) >> 3;
    dest[10] = (3*p1 + 5*p2) >> 3;
    p1 = source[6];
    dest[11] = p2;
    dest[12] = (1*p2 + 1*p1) >> 1;
    dest[13] = p1;
    p2 = source[7];
    dest[14] = (5*p1 + 3*p2) >> 3;
    dest[15] = (1*p1 + 7*p2) >> 3;
    p1 = source[8];
    dest[16] = (5*p2 + 3*p1) >> 3;
    dest[17] = (1*p2 + 3*p1) >> 2;
    p2 = source[9];
    dest[18] = (3*p1 + 1*p2) >> 2;
    dest[19] = (1*p1 + 3*p2) >> 2;
    p1 = source[10];
    dest[20] = (7*p2 + 1*p1) >> 3;
    dest[21] = (3*p2 + 5*p1) >> 3;
    p2 = source[11];
    dest[22] = (7*p1 + 1*p2) >> 3;
    dest[23] = (1*p1 + 1*p2) >> 1;
    source += 11;
    dest += 24;
  }

  if ((width += 24) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 1*source[1]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 7*source[1]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[1] + 3*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[1] + 7*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[2] + 1*source[3]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[2] + 3*source[3]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[3] + 1*source[4]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[3] + 5*source[4]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[4] + 1*source[5]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[4] + 5*source[5]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = source[5];
  if (--width <= 0) goto done;
  *dest++ = (1*source[5] + 1*source[6]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = source[6];
  if (--width <= 0) goto done;
  *dest++ = (5*source[6] + 3*source[7]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[6] + 7*source[7]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (5*source[7] + 3*source[8]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[7] + 3*source[8]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (3*source[8] + 1*source[9]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[8] + 3*source[9]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (7*source[9] + 1*source[10]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[9] + 5*source[10]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (7*source[10] + 1*source[11]) >> 3;
done:

  xine_profiler_stop_count(prof_scale_line);
}


/*
 * Interpolates 8 output pixels from 5 source pixels using shifts.
 * Useful for scaling a PAL svcd input source to 4:3 display format.
 */
static void scale_line_5_8 (uint8_t *source, uint8_t *dest,
			    int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 8) >= 0) {
    p1 = source[0];
    p2 = source[1];
    dest[0] = p1;
    dest[1] = (3*p1 + 5*p2) >> 3;
    p1 = source[2];
    dest[2] = (3*p2 + 1*p1) >> 2;
    dest[3] = (1*p2 + 7*p1) >> 3;
    p2 = source[3];
    dest[4] = (1*p1 + 1*p2) >> 1;
    p1 = source[4];
    dest[5] = (7*p2 + 1*p1) >> 3;
    dest[6] = (1*p2 + 3*p1) >> 2;
    p2 = source[5];
    dest[7] = (5*p1 + 3*p2) >> 3;
    source += 5;
    dest += 8;
  }

  if ((width += 8) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (3*source[0] + 5*source[1]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (3*source[1] + 1*source[2]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[1] + 7*source[2]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[2] + 1*source[3]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = (7*source[3] + 1*source[4]) >> 3;
  if (--width <= 0) goto done;
  *dest++ = (1*source[3] + 3*source[4]) >> 2;
done:

  xine_profiler_stop_count(prof_scale_line);
}


/*
 * Interpolates 4 output pixels from 3 source pixels using shifts.
 * Useful for scaling a NTSC svcd input source to 4:3 display format.
 */
static void scale_line_3_4 (uint8_t *source, uint8_t *dest,
			    int width, int step) {

  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  while ((width -= 4) >= 0) {
    p1 = source[0];
    p2 = source[1];
    dest[0] = p1;
    dest[1] = (1*p1 + 3*p2) >> 2;
    p1 = source[2];
    dest[2] = (1*p2 + 1*p1) >> 1;
    p2 = source[3];
    dest[3] = (3*p1 + 1*p2) >> 2;
    source += 3;
    dest += 4;
  }

  if ((width += 4) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (1*source[0] + 3*source[1]) >> 2;
  if (--width <= 0) goto done;
  *dest++ = (1*source[1] + 1*source[2]) >> 1;
done:

  xine_profiler_stop_count(prof_scale_line);
}


/* Interpolate 2 output pixels from one source pixel. */

static void scale_line_1_2 (uint8_t *source, uint8_t *dest,
			    int width, int step) {
  int p1, p2;

  xine_profiler_start_count(prof_scale_line);

  p1 = *source;
  while ((width -= 4) >= 0) {
    *dest++ = p1;
    p2 = *++source;
    *dest++ = (p1 + p2) >> 1;
    *dest++ = p2;
    p1 = *++source;
    *dest++ = (p2 + p1) >> 1;
  }

  if ((width += 4) <= 0) goto done;
  *dest++ = source[0];
  if (--width <= 0) goto done;
  *dest++ = (source[0] + source[1]) >> 1;
  if (--width <= 0) goto done;
  *dest++ = source[1];
 done:

  xine_profiler_stop_count(prof_scale_line);
}

			
/*
 * Scale line with no horizontal scaling. For NTSC mpeg2 dvd input in
 * 4:3 output format (720x480 -> 720x540)
 */
static void scale_line_1_1 (uint8_t *source, uint8_t *dest,
			    int width, int step) {

  xine_profiler_start_count(prof_scale_line);
  xine_fast_memcpy(dest, source, width);
  xine_profiler_stop_count(prof_scale_line);
}

			
static scale_line_func_t find_scale_line_func(int step) {
  static struct {
    int			src_step;
    int			dest_step;
    scale_line_func_t	func;
    char	       *desc;
  } scale_line[] = {
    { 15, 16, scale_line_15_16, "dvd 4:3(pal)" },
    { 45, 64, scale_line_45_64, "dvd 16:9(pal), fullscreen(1024x768)" },
    {  9, 16, scale_line_9_16,  "dvd fullscreen(1280x1024)" },
    { 45, 53, scale_line_45_53, "dvd 16:9(ntsc)" },
    { 11, 12, scale_line_11_12, "vcd 4:3(pal)" },
    { 11, 24, scale_line_11_24, "vcd 4:3(pal) 2*zoom" },
    {  5,  8, scale_line_5_8,   "svcd 4:3(pal)" },
    {  3,  4, scale_line_3_4,   "svcd 4:3(ntsc)" },
    {  1,  2, scale_line_1_2,   "2*zoom" },
    {  1,  1, scale_line_1_1,   "non-scaled" },
  };
  int i;

  for (i = 0; i < sizeof(scale_line)/sizeof(scale_line[0]); i++) {
    if (step == scale_line[i].src_step*32768/scale_line[i].dest_step) {
      printf("yuv2rgb: using %s optimized scale_line\n", scale_line[i].desc);
      return scale_line[i].func;
    }
  }
  printf("yuv2rgb: using generic scale_line with interpolation\n");
  return scale_line_gen;

}


static void scale_line_2 (uint8_t *source, uint8_t *dest,
			  int width, int step) {
  int p1;
  int p2;
  int dx;

  p1 = *source; source+=2;
  p2 = *source; source+=2;
  dx = 0;

  while (width) {

    *dest = (p1 * (32768 - dx) + p2 * dx) / 32768;

    dx += step;
    while (dx > 32768) {
      dx -= 32768;
      p1 = p2;
      p2 = *source;
      source+=2;
    }

    dest ++;
    width --;
  }
}

static void scale_line_4 (uint8_t *source, uint8_t *dest,
			  int width, int step) {
  int p1;
  int p2;
  int dx;

  p1 = *source; source+=4;
  p2 = *source; source+=4;
  dx = 0;

  while (width) {

    *dest = (p1 * (32768 - dx) + p2 * dx) / 32768;

    dx += step;
    while (dx > 32768) {
      dx -= 32768;
      p1 = p2;
      p2 = *source;
      source+=4;
    }

    dest ++;
    width --;
  }
}


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

#define DST2(i)					\
	Y = py_2[2*i];				\
	dst_2[2*i] = r[Y] + g[Y] + b[Y];	\
	Y = py_2[2*i+1];			\
	dst_2[2*i+1] = r[Y] + g[Y] + b[Y];

#define DST1RGB(i)							\
	Y = py_1[2*i];							\
	dst_1[6*i] = r[Y]; dst_1[6*i+1] = g[Y]; dst_1[6*i+2] = b[Y];	\
	Y = py_1[2*i+1];						\
	dst_1[6*i+3] = r[Y]; dst_1[6*i+4] = g[Y]; dst_1[6*i+5] = b[Y];

#define DST2RGB(i)							\
	Y = py_2[2*i];							\
	dst_2[6*i] = r[Y]; dst_2[6*i+1] = g[Y]; dst_2[6*i+2] = b[Y];	\
	Y = py_2[2*i+1];						\
	dst_2[6*i+3] = r[Y]; dst_2[6*i+4] = g[Y]; dst_2[6*i+5] = b[Y];

#define DST1BGR(i)							\
	Y = py_1[2*i];							\
	dst_1[6*i] = b[Y]; dst_1[6*i+1] = g[Y]; dst_1[6*i+2] = r[Y];	\
	Y = py_1[2*i+1];						\
	dst_1[6*i+3] = b[Y]; dst_1[6*i+4] = g[Y]; dst_1[6*i+5] = r[Y];

#define DST2BGR(i)							\
	Y = py_2[2*i];							\
	dst_2[6*i] = b[Y]; dst_2[6*i+1] = g[Y]; dst_2[6*i+2] = r[Y];	\
	Y = py_2[2*i+1];						\
	dst_2[6*i+3] = b[Y]; dst_2[6*i+4] = g[Y]; dst_2[6*i+5] = r[Y];

#define DST1CMAP(i)							\
	Y = py_1[2*i];                          			\
	dst_1[2*i] = this->cmap[r[Y] + g[Y] + b[Y]];		\
	Y = py_1[2*i+1];						\
	dst_1[2*i+1] = this->cmap[r[Y] + g[Y] + b[Y]];

#define DST2CMAP(i)							\
	Y = py_2[2*i];							\
	dst_2[2*i] = this->cmap[r[Y] + g[Y] + b[Y]];		\
	Y = py_2[2*i+1];						\
	dst_2[2*i+1] = this->cmap[r[Y] + g[Y] + b[Y]];

static void yuv2rgb_c_32 (yuv2rgb_t *this, uint8_t * _dst,
			  uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
  int U, V, Y;
  uint8_t  * py_1, * py_2, * pu, * pv;
  uint32_t * r, * g, * b;
  uint32_t * dst_1, * dst_2;
  int width, height, dst_height;
  int dy;

  if (this->do_scale) {
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
      dst_1 = (uint32_t*)_dst;
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

	xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*4); 

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
  } else {
    height = this->source_height >> 1;
    do {
      dst_1 = (uint32_t*)_dst;
      dst_2 = (void*)( (uint8_t *)_dst + this->rgb_stride );
      py_1 = _py;
      py_2 = _py + this->y_stride;
      pu   = _pu;
      pv   = _pv;

      width = this->source_width >> 3;
      do {
	RGB(0);
	DST1(0);
	DST2(0);

	RGB(1);
	DST2(1);
	DST1(1);

	RGB(2);
	DST1(2);
	DST2(2);

	RGB(3);
	DST2(3);
	DST1(3);
      
	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
      } while (--width);

      _dst += 2 * this->rgb_stride; 
      _py += 2 * this->y_stride;
      _pu += this->uv_stride;
      _pv += this->uv_stride;

    } while (--height);
  }
}

/* This is very near from the yuv2rgb_c_32 code */
static void yuv2rgb_c_24_rgb (yuv2rgb_t *this, uint8_t * _dst,
			      uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
  int U, V, Y;
  uint8_t * py_1, * py_2, * pu, * pv;
  uint8_t * r, * g, * b;
  uint8_t * dst_1, * dst_2;
  int width, height, dst_height;
  int dy;

  if (this->do_scale) {

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
      dst_1 = _dst;
      py_1  = this->y_buffer;
      pu    = this->u_buffer;
      pv    = this->v_buffer;

      width = this->dest_width >> 3;

      do {
	  RGB(0);
	  DST1RGB(0);

	  RGB(1);
	  DST1RGB(1);
      
	  RGB(2);
	  DST1RGB(2);

	  RGB(3);
	  DST1RGB(3);

	  pu += 4;
	  pv += 4;
	  py_1 += 8;
	  dst_1 += 24;
      } while (--width);

      dy += this->step_dy;
      _dst += this->rgb_stride;

      while (--dst_height > 0 && dy < 32768) {

	xine_fast_memcpy (_dst, _dst-this->rgb_stride, this->dest_width*3); 

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
      } while (dy>=32768);
    }
  } else {
    height = this->source_height >> 1;
    do {
      dst_1 = _dst;
      dst_2 = (void*)( (uint8_t *)_dst + this->rgb_stride );
      py_1  = _py;
      py_2  = _py + this->y_stride;
      pu    = _pu;
      pv    = _pv;

      width = this->source_width >> 3;
      do {
	RGB(0);
	DST1RGB(0);
	DST2RGB(0);

	RGB(1);
	DST2RGB(1);
	DST1RGB(1);

	RGB(2);
	DST1RGB(2);
	DST2RGB(2);

	RGB(3);
	DST2RGB(3);
	DST1RGB(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 24;
	dst_2 += 24;
      } while (--width);

      _dst += 2 * this->rgb_stride; 
      _py += 2 * this->y_stride;
      _pu += this->uv_stride;
      _pv += this->uv_stride;
      
    } while (--height);
  }
}

/* only trivial mods from yuv2rgb_c_24_rgb */
static void yuv2rgb_c_24_bgr (yuv2rgb_t *this, uint8_t * _dst,
			      uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
  int U, V, Y;
  uint8_t * py_1, * py_2, * pu, * pv;
  uint8_t * r, * g, * b;
  uint8_t * dst_1, * dst_2;
  int width, height, dst_height;
  int dy;

  if (this->do_scale) {

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
      dst_1 = _dst;
      py_1  = this->y_buffer;
      pu    = this->u_buffer;
      pv    = this->v_buffer;

      width = this->dest_width >> 3;

      do {
	  RGB(0);
	  DST1BGR(0);

	  RGB(1);
	  DST1BGR(1);
      
	  RGB(2);
	  DST1BGR(2);

	  RGB(3);
	  DST1BGR(3);

	  pu += 4;
	  pv += 4;
	  py_1 += 8;
	  dst_1 += 24;
      } while (--width);

      dy += this->step_dy;
      _dst += this->rgb_stride;

      while (--dst_height > 0 && dy < 32768) {

	xine_fast_memcpy (_dst, _dst-this->rgb_stride, this->dest_width*3);

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
      } while( dy>=32768 );
    }

  } else {
    height = this->source_height >> 1;
    do {
      dst_1 = _dst;
      dst_2 = (void*)( (uint8_t *)_dst + this->rgb_stride );
      py_1 = _py;
      py_2 = _py + this->y_stride;
      pu   = _pu;
      pv   = _pv;
      width = this->source_width >> 3;
      do {
	RGB(0);
	DST1BGR(0);
	DST2BGR(0);

	RGB(1);
	DST2BGR(1);
	DST1BGR(1);

	RGB(2);
	DST1BGR(2);
	DST2BGR(2);

	RGB(3);
	DST2BGR(3);
	DST1BGR(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 24;
	dst_2 += 24;
      } while (--width);

      _dst += 2 * this->rgb_stride; 
      _py += 2 * this->y_stride;
      _pu += this->uv_stride;
      _pv += this->uv_stride;

    } while (--height);
  }
}

/* This is exactly the same code as yuv2rgb_c_32 except for the types of */
/* r, g, b, dst_1, dst_2 */
static void yuv2rgb_c_16 (yuv2rgb_t *this, uint8_t * _dst,
			  uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
  int U, V, Y;
  uint8_t * py_1, * py_2, * pu, * pv;
  uint16_t * r, * g, * b;
  uint16_t * dst_1, * dst_2;
  int width, height, dst_height;
  int dy;

  if (this->do_scale) {
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
  } else {
    height = this->source_height >> 1;
    do {
      dst_1 = (uint16_t*)_dst;
      dst_2 = (void*)( (uint8_t *)_dst + this->rgb_stride );
      py_1 = _py;
      py_2 = _py + this->y_stride;
      pu   = _pu;
      pv   = _pv;
      width = this->source_width >> 3;
      do {
	RGB(0);
	DST1(0);
	DST2(0);

	RGB(1);
	DST2(1);
	DST1(1);

	RGB(2);
	DST1(2);
	DST2(2);

	RGB(3);
	DST2(3);
	DST1(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
      } while (--width);

      _dst += 2 * this->rgb_stride; 
      _py += 2 * this->y_stride;
      _pu += this->uv_stride;
      _pv += this->uv_stride;

    } while (--height);
  }
}

/* This is exactly the same code as yuv2rgb_c_32 except for the types of */
/* r, g, b, dst_1, dst_2 */
static void yuv2rgb_c_8 (yuv2rgb_t *this, uint8_t * _dst,
			  uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
  int U, V, Y;
  uint8_t  * py_1, * py_2, * pu, * pv;
  uint8_t * r, * g, * b;
  uint8_t * dst_1, * dst_2;
  int width, height, dst_height;
  int dy;

  if (this->do_scale) {
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
      dst_1 = (uint8_t*)_dst;
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

	xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width); 

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
      } while( dy>=32768 );
    }
  } else {
    height = this->source_height >> 1;
    do {
      dst_1 = (uint8_t*)_dst;
      dst_2 = (void*)( (uint8_t *)_dst + this->rgb_stride );
      py_1 = _py;
      py_2 = _py + this->y_stride;
      pu   = _pu;
      pv   = _pv;

      width = this->source_width >> 3;
      do {
	RGB(0);
	DST1(0);
	DST2(0);

	RGB(1);
	DST2(1);
	DST1(1);

	RGB(2);
	DST1(2);
	DST2(2);

	RGB(3);
	DST2(3);
	DST1(3);
      
	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
      } while (--width);

      _dst += 2 * this->rgb_stride; 
      _py += 2 * this->y_stride;
      _pu += this->uv_stride;
      _pv += this->uv_stride;

    } while (--height);
  }
}

/* now for something different: 256 grayscale mode */
static void yuv2rgb_c_gray (yuv2rgb_t *this, uint8_t * _dst,
			    uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
  int height, dst_height;
  int dy;

  if (this->do_scale) {
    scale_line_func_t scale_line = this->scale_line;

    dy = 0;
    dst_height = this->dest_height;

    for (;;) {
      scale_line (_py, _dst, this->dest_width, this->step_dx);

      dy += this->step_dy;
      _dst += this->rgb_stride;

      while (--dst_height > 0 && dy < 32768) {

	xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width); 

	dy += this->step_dy;
	_dst += this->rgb_stride;
      }

      if (dst_height <= 0)
	break;

      _py += this->y_stride*(dy>>15);
      dy &= 32767;
      /* dy -= 32768; 
	 _py += this->y_stride;
      */
    }
  } else {
    for (height = this->source_height; --height >= 0; ) {
      xine_fast_memcpy(_dst, _py, this->dest_width);
      _dst += this->rgb_stride;
      _py += this->y_stride;
    }
  }
}

/* now for something different: 256 color mode */
static void yuv2rgb_c_palette (yuv2rgb_t *this, uint8_t * _dst,
			       uint8_t * _py, uint8_t * _pu, uint8_t * _pv)
{
  int U, V, Y;
  uint8_t * py_1, * py_2, * pu, * pv;
  uint16_t * r, * g, * b;
  uint8_t * dst_1, * dst_2;
  int width, height, dst_height;
  int dy;

  if (this->do_scale) {
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
      dst_1 = _dst;
      py_1  = this->y_buffer;
      pu    = this->u_buffer;
      pv    = this->v_buffer;

      width = this->dest_width >> 3;

      do {
	  RGB(0);
	  DST1CMAP(0);

	  RGB(1);
	  DST1CMAP(1);
      
	  RGB(2);
	  DST1CMAP(2);

	  RGB(3);
	  DST1CMAP(3);

	  pu += 4;
	  pv += 4;
	  py_1 += 8;
	  dst_1 += 8;
      } while (--width);

      dy += this->step_dy;
      _dst += this->rgb_stride;

      while (--dst_height > 0 && dy < 32768) {

	xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width); 

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
      } while( dy>=32768 );
    }
  } else {
    height = this->source_height >> 1;
    do {
      dst_1 = _dst;
      dst_2 = _dst + this->rgb_stride;
      py_1 = _py;
      py_2 = _py + this->y_stride;
      pu   = _pu;
      pv   = _pv;
      width = this->source_width >> 3;
      do {
	RGB(0);
	DST1CMAP(0);
	DST2CMAP(0);

	RGB(1);
	DST2CMAP(1);
	DST1CMAP(1);

	RGB(2);
	DST1CMAP(2);
	DST2CMAP(2);

	RGB(3);
	DST2CMAP(3);
	DST1CMAP(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
      } while (--width);

      _dst += 2 * this->rgb_stride; 
      _py += 2 * this->y_stride;
      _pu += this->uv_stride;
      _pv += this->uv_stride;

    } while (--height);
  }
}

static int div_round (int dividend, int divisor)
{
  if (dividend > 0)
    return (dividend + (divisor>>1)) / divisor;
  else
    return -((-dividend + (divisor>>1)) / divisor);
}

static void yuv2rgb_setup_tables (yuv2rgb_factory_t *this, int mode, int swapped) 
{
  int i;
  uint8_t table_Y[1024];
  uint32_t * table_32 = 0;
  uint16_t * table_16 = 0;
  uint8_t * table_8 = 0;
  int entry_size = 0;
  void *table_r = 0, *table_g = 0, *table_b = 0;
  int shift_r = 0, shift_g = 0, shift_b = 0;

  int crv = Inverse_Table_6_9[this->matrix_coefficients][0];
  int cbu = Inverse_Table_6_9[this->matrix_coefficients][1];
  int cgu = -Inverse_Table_6_9[this->matrix_coefficients][2];
  int cgv = -Inverse_Table_6_9[this->matrix_coefficients][3];

  for (i = 0; i < 1024; i++) {
    int j;

    j = (76309 * (i - 384 - 16) + 32768) >> 16;
    j = (j < 0) ? 0 : ((j > 255) ? 255 : j);
    table_Y[i] = j;
  }

  switch (mode) {
  case MODE_32_RGB:
  case MODE_32_BGR:
    table_32 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint32_t));

    entry_size = sizeof (uint32_t);
    table_r = table_32 + 197;
    table_b = table_32 + 197 + 685;
    table_g = table_32 + 197 + 2*682;

    if (swapped) {
      switch (mode) {
      case MODE_32_RGB: shift_r =  8; shift_g = 16; shift_b = 24; break;
      case MODE_32_BGR:	shift_r = 24; shift_g = 16; shift_b =  8; break;
      }
    } else {
      switch (mode) {
      case MODE_32_RGB:	shift_r = 16; shift_g =  8; shift_b =  0; break;
      case MODE_32_BGR:	shift_r =  0; shift_g =  8; shift_b = 16; break;
      }
    }

    for (i = -197; i < 256+197; i++)
      ((uint32_t *) table_r)[i] = table_Y[i+384] << shift_r;
    for (i = -132; i < 256+132; i++)
      ((uint32_t *) table_g)[i] = table_Y[i+384] << shift_g;
    for (i = -232; i < 256+232; i++)
      ((uint32_t *) table_b)[i] = table_Y[i+384] << shift_b;
    break;

  case MODE_24_RGB:
  case MODE_24_BGR:
    table_8 = malloc ((256 + 2*232) * sizeof (uint8_t));

    entry_size = sizeof (uint8_t);
    table_r = table_g = table_b = table_8 + 232;

    for (i = -232; i < 256+232; i++)
      ((uint8_t * )table_b)[i] = table_Y[i+384];
    break;

  case MODE_15_BGR:
  case MODE_16_BGR:
  case MODE_15_RGB:
  case MODE_16_RGB:
    table_16 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint16_t));

    entry_size = sizeof (uint16_t);
    table_r = table_16 + 197;
    table_b = table_16 + 197 + 685;
    table_g = table_16 + 197 + 2*682;

    if (swapped) {
      switch (mode) {
      case MODE_15_BGR: shift_r =  8; shift_g =  5; shift_b = 2; break;
      case MODE_16_BGR:	shift_r =  8; shift_g =  5; shift_b = 3; break;
      case MODE_15_RGB:	shift_r =  2; shift_g =  5; shift_b = 8; break;
      case MODE_16_RGB:	shift_r =  3; shift_g =  5; shift_b = 8; break;
      }
    } else {
      switch (mode) {
      case MODE_15_BGR:	shift_r =  0; shift_g =  5; shift_b = 10; break;
      case MODE_16_BGR:	shift_r =  0; shift_g =  5; shift_b = 11; break;
      case MODE_15_RGB:	shift_r = 10; shift_g =  5; shift_b =  0; break;
      case MODE_16_RGB:	shift_r = 11; shift_g =  5; shift_b =  0; break;
      }
    }

    for (i = -197; i < 256+197; i++)
      ((uint16_t *)table_r)[i] = (table_Y[i+384] >> 3) << shift_r;

    for (i = -132; i < 256+132; i++) {
      int j = table_Y[i+384] >> (((mode==MODE_16_RGB) || (mode==MODE_16_BGR)) ? 2 : 3);
      if (swapped)
	((uint16_t *)table_g)[i] = (j&7) << 13 | (j>>3);
      else
	((uint16_t *)table_g)[i] = j << 5;
    }
    for (i = -232; i < 256+232; i++)
      ((uint16_t *)table_b)[i] = (table_Y[i+384] >> 3) << shift_b;

    break;

  case MODE_8_RGB:
  case MODE_8_BGR:
    table_8 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint8_t));

    entry_size = sizeof (uint8_t);
    table_r = table_8 + 197;
    table_b = table_8 + 197 + 685;
    table_g = table_8 + 197 + 2*682;

    switch (mode) {
    case MODE_8_RGB: shift_r =  5; shift_g =  2; shift_b =  0; break;
    case MODE_8_BGR: shift_r =  0; shift_g =  3; shift_b =  6; break;
    }

    for (i = -197; i < 256+197; i++)
      ((uint8_t *) table_r)[i] = (table_Y[i+384] >> 5) << shift_r;
    for (i = -132; i < 256+132; i++)
      ((uint8_t *) table_g)[i] = (table_Y[i+384] >> 5) << shift_g;
    for (i = -232; i < 256+232; i++)
      ((uint8_t *) table_b)[i] = (table_Y[i+384] >> 6) << shift_b;
    break;

  case MODE_8_GRAY:
    return;

  case MODE_PALETTE:
    table_16 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint16_t));

    entry_size = sizeof (uint16_t);
    table_r = table_16 + 197;
    table_b = table_16 + 197 + 685;
    table_g = table_16 + 197 + 2*682;

    shift_r = 10;
    shift_g = 5;
    shift_b = 0;

    for (i = -197; i < 256+197; i++)
      ((uint16_t *)table_r)[i] = (table_Y[i+384] >> 3) << 10;

    for (i = -132; i < 256+132; i++)
      ((uint16_t *)table_g)[i] = (table_Y[i+384] >> 3) << 5;

    for (i = -232; i < 256+232; i++)
      ((uint16_t *)table_b)[i] = (table_Y[i+384] >> 3) << 0;

    break;


  default:
    fprintf (stderr, "mode %d not supported by yuv2rgb\n", mode);
    abort();
  }
  
  for (i = 0; i < 256; i++) {
    this->table_rV[i] = (((uint8_t *) table_r) +
			 entry_size * div_round (crv * (i-128), 76309));
    this->table_gU[i] = (((uint8_t *) table_g) +
			 entry_size * div_round (cgu * (i-128), 76309));
    this->table_gV[i] = entry_size * div_round (cgv * (i-128), 76309);
    this->table_bU[i] = (((uint8_t *)table_b) +
			 entry_size * div_round (cbu * (i-128), 76309));
  }
  this->gamma = 0;
  this->entry_size = entry_size;
}

static uint32_t yuv2rgb_single_pixel_32 (yuv2rgb_t *this, uint8_t y, uint8_t u, uint8_t v)
{
  uint32_t * r, * g, * b;

  r = this->table_rV[v];
  g = (void *) (((uint8_t *)this->table_gU[u]) + this->table_gV[v]);
  b = this->table_bU[u];

  return r[y] + g[y] + b[y];
}

static uint32_t yuv2rgb_single_pixel_24_rgb (yuv2rgb_t *this, uint8_t y, uint8_t u, uint8_t v)
{
  uint8_t * r, * g, * b;

  r = this->table_rV[v];
  g = (void *) (((uint8_t *)this->table_gU[u]) + this->table_gV[v]);
  b = this->table_bU[u];

  return (uint32_t) r[y] +
          ((uint32_t) g[y] << 8) +
          ((uint32_t) b[y] << 16);
}

static uint32_t yuv2rgb_single_pixel_24_bgr (yuv2rgb_t *this, uint8_t y, uint8_t u, uint8_t v)
{
  uint8_t * r, * g, * b;

  r = this->table_rV[v];
  g = (void *) (((uint8_t *)this->table_gU[u]) + this->table_gV[v]);
  b = this->table_bU[u];

  return (uint32_t) b[y] +
          ((uint32_t) g[y] << 8) +
          ((uint32_t) r[y] << 16);
}

static uint32_t yuv2rgb_single_pixel_16 (yuv2rgb_t *this, uint8_t y, uint8_t u, uint8_t v)
{
  uint16_t * r, * g, * b;

  r = this->table_rV[v];
  g = (void *) (((uint8_t *)this->table_gU[u]) + this->table_gV[v]);
  b = this->table_bU[u];

  return r[y] + g[y] + b[y];
}

static uint32_t yuv2rgb_single_pixel_8 (yuv2rgb_t *this, uint8_t y, uint8_t u, uint8_t v)
{
  uint8_t * r, * g, * b;

  r = this->table_rV[v];
  g = (void *) (((uint8_t *)this->table_gU[u]) + this->table_gV[v]);
  b = this->table_bU[u];

  return r[y] + g[y] + b[y];
}

static uint32_t yuv2rgb_single_pixel_gray (yuv2rgb_t *this, uint8_t y, uint8_t u, uint8_t v)
{
  return y;
}

static uint32_t yuv2rgb_single_pixel_palette (yuv2rgb_t *this, uint8_t y, uint8_t u, uint8_t v)
{
  uint16_t * r, * g, * b;

  r = this->table_rV[v];
  g = (void *) (((uint8_t *)this->table_gU[u]) + this->table_gV[v]);
  b = this->table_bU[u];

  return this->cmap[r[y] + g[y] + b[y]];
}


static void yuv2rgb_c_init (yuv2rgb_factory_t *this)
{
  switch (this->mode) {
  case MODE_32_RGB:
  case MODE_32_BGR:
    this->yuv2rgb_fun = yuv2rgb_c_32;
    break;

  case MODE_24_RGB:
  case MODE_24_BGR:
    this->yuv2rgb_fun =
	(this->mode==MODE_24_RGB && !this->swapped) || (this->mode==MODE_24_BGR && this->swapped)
	    ? yuv2rgb_c_24_rgb
	    : yuv2rgb_c_24_bgr;
    break;

  case MODE_15_BGR:
  case MODE_16_BGR:
  case MODE_15_RGB:
  case MODE_16_RGB:
    this->yuv2rgb_fun = yuv2rgb_c_16;
    break;

  case MODE_8_RGB:
  case MODE_8_BGR:
    this->yuv2rgb_fun = yuv2rgb_c_8;
    break;

  case MODE_8_GRAY:
    this->yuv2rgb_fun = yuv2rgb_c_gray;
    break;

  case MODE_PALETTE:
    this->yuv2rgb_fun = yuv2rgb_c_palette;
    break;

  default:
    printf ("yuv2rgb: mode %d not supported by yuv2rgb\n", this->mode);
    abort();
  }

}

static void yuv2rgb_single_pixel_init (yuv2rgb_factory_t *this) {

  switch (this->mode) {
  case MODE_32_RGB:
  case MODE_32_BGR:
    this->yuv2rgb_single_pixel_fun = yuv2rgb_single_pixel_32;
    break;

  case MODE_24_RGB:
  case MODE_24_BGR:
    this->yuv2rgb_single_pixel_fun =
	(this->mode==MODE_24_RGB && !this->swapped) || (this->mode==MODE_24_BGR && this->swapped)
	    ? yuv2rgb_single_pixel_24_rgb
	    : yuv2rgb_single_pixel_24_bgr;
    break;

  case MODE_15_BGR:
  case MODE_16_BGR:
  case MODE_15_RGB:
  case MODE_16_RGB:
    this->yuv2rgb_single_pixel_fun = yuv2rgb_single_pixel_16;
    break;

  case MODE_8_RGB:
  case MODE_8_BGR:
    this->yuv2rgb_single_pixel_fun = yuv2rgb_single_pixel_8;
    break;

  case MODE_8_GRAY:
    this->yuv2rgb_single_pixel_fun = yuv2rgb_single_pixel_gray;
    break;

  case MODE_PALETTE:
    this->yuv2rgb_single_pixel_fun = yuv2rgb_single_pixel_palette;
    break;

  default:
    printf ("yuv2rgb: mode %d not supported by yuv2rgb\n", this->mode);
    abort();
  }
}


/*
 * yuy2 stuff
 */

static void yuy22rgb_c_32 (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _p)
{
  int U, V, Y;
  uint8_t * py_1, * pu, * pv;
  uint32_t * r, * g, * b;
  uint32_t * dst_1;
  int width, height;
  int dy;

  /* FIXME: implement unscaled version */

  scale_line_4 (_p+1, this->u_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_4 (_p+3, this->v_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_2 (_p, this->y_buffer,
		this->dest_width, this->step_dx);
  
  dy = 0;
  height = this->dest_height;
  
  for (;;) {
    dst_1 = (uint32_t*)_dst;
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
    
    while (--height > 0 && dy < 32768) {
      
      xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*4);
      
      dy += this->step_dy;
      _dst += this->rgb_stride;
    }
    
    if (height <= 0)
      break;

    _p += this->y_stride*2*(dy>>15);
    dy &= 32767;
    /*
      dy -= 32768;
      _p += this->y_stride*2;
    */
    
    scale_line_4 (_p+1, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_4 (_p+3, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_2 (_p, this->y_buffer, 
		  this->dest_width, this->step_dx);
  }
}

static void yuy22rgb_c_24_rgb (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _p)
{
  int U, V, Y;
  uint8_t * py_1, * pu, * pv;
  uint8_t * r, * g, * b;
  uint8_t * dst_1;
  int width, height;
  int dy;

  /* FIXME: implement unscaled version */

  scale_line_4 (_p+1, this->u_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_4 (_p+3, this->v_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_2 (_p, this->y_buffer, 
		this->dest_width, this->step_dx);

  dy = 0;
  height = this->dest_height;
  
  for (;;) {
    dst_1 = _dst;
    py_1  = this->y_buffer;
    pu    = this->u_buffer;
    pv    = this->v_buffer;
    
    width = this->dest_width >> 3;
    
    do {
      RGB(0);
      DST1RGB(0);
      
      RGB(1);
      DST1RGB(1);
      
      RGB(2);
      DST1RGB(2);
      
      RGB(3);
      DST1RGB(3);

      pu += 4;
      pv += 4;
      py_1 += 8;
      dst_1 += 24;
    } while (--width);

    dy += this->step_dy;
    _dst += this->rgb_stride;
    
    while (--height > 0 && dy < 32768) {

      xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*3);
      
      dy += this->step_dy;
      _dst += this->rgb_stride;
    }
    
    if (height <= 0)
      break;

    _p += this->y_stride*2*(dy>>15);
    dy &= 32767;
    /*
      dy -= 32768;
      _p += this->y_stride*2;
    */
    
    scale_line_4 (_p+1, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_4 (_p+3, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_2 (_p, this->y_buffer,
		  this->dest_width, this->step_dx);
  }
}

static void yuy22rgb_c_24_bgr (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _p)
{
  int U, V, Y;
  uint8_t * py_1, * pu, * pv;
  uint8_t * r, * g, * b;
  uint8_t * dst_1;
  int width, height;
  int dy;

  /* FIXME: implement unscaled version */

  scale_line_4 (_p+1, this->u_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_4 (_p+3, this->v_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_2 (_p, this->y_buffer, 
		this->dest_width, this->step_dx);

  dy = 0;
  height = this->dest_height;
  
  for (;;) {
    dst_1 = _dst;
    py_1  = this->y_buffer;
    pu    = this->u_buffer;
    pv    = this->v_buffer;
    
    width = this->dest_width >> 3;
    
    do {
      RGB(0);
      DST1BGR(0);
      
      RGB(1);
      DST1BGR(1);
      
      RGB(2);
      DST1BGR(2);

      RGB(3);
      DST1BGR(3);
      
      pu += 4;
      pv += 4;
      py_1 += 8;
      dst_1 += 24;
    } while (--width);
    
    dy += this->step_dy;
    _dst += this->rgb_stride;
    
    while (--height > 0 && dy < 32768) {

      xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*3);
      
      dy += this->step_dy;
      _dst += this->rgb_stride;
    }
    
    if (height <= 0)
      break;

    _p += this->y_stride*2*(dy>>15);
    dy &= 32767;

    scale_line_4 (_p+1, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_4 (_p+3, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_2 (_p, this->y_buffer,
		  this->dest_width, this->step_dx);
  }
}

static void yuy22rgb_c_16 (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _p)
{
  int U, V, Y;
  uint8_t * py_1, * pu, * pv;
  uint16_t * r, * g, * b;
  uint16_t * dst_1;
  int width, height;
  int dy;

  /* FIXME: implement unscaled version */

  scale_line_4 (_p+1, this->u_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_4 (_p+3, this->v_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_2 (_p, this->y_buffer, 
		this->dest_width, this->step_dx);
  
  dy = 0;
  height = this->dest_height;

  for (;;) {
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

    while (--height > 0 && dy < 32768) {
      
      xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width*2); 

      dy += this->step_dy;
      _dst += this->rgb_stride;
    }
    
    if (height <= 0)
      break;

    _p += this->y_stride*2*(dy>>15);
    dy &= 32767;
    
    scale_line_4 (_p+1, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_4 (_p+3, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_2 (_p, this->y_buffer,
		  this->dest_width, this->step_dx);
  }
}

static void yuy22rgb_c_8 (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _p)
{
  int U, V, Y;
  uint8_t * py_1, * pu, * pv;
  uint8_t * r, * g, * b;
  uint8_t * dst_1;
  int width, height;
  int dy;

  /* FIXME: implement unscaled version */

  scale_line_4 (_p+1, this->u_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_4 (_p+3, this->v_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_2 (_p, this->y_buffer,
		this->dest_width, this->step_dx);
  
  dy = 0;
  height = this->dest_height;
  
  for (;;) {
    dst_1 = _dst;
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
    
    while (--height > 0 && dy < 32768) {
      
      xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width); 
      
      dy += this->step_dy;
      _dst += this->rgb_stride;
    }
    
    if (height <= 0)
      break;

    _p += this->y_stride*2*(dy>>15);
    dy &= 32767;
    
    scale_line_4 (_p+1, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_4 (_p+3, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_2 (_p, this->y_buffer, 
		  this->dest_width, this->step_dx);
  }
}

static void yuy22rgb_c_gray (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _p)
{
  int width, height;
  int dy;
  uint8_t * dst;
  uint8_t * y;

  if (this->do_scale) {
    dy = 0;
    height = this->dest_height;
  
    for (;;) {
      scale_line_2 (_p, _dst, this->dest_width, this->step_dx);
    
      dy += this->step_dy;
      _dst += this->rgb_stride;
    
      while (--height > 0 && dy < 32768) {
      
	xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width); 
      
	dy += this->step_dy;
	_dst += this->rgb_stride;
      }
    
      if (height <= 0)
	break;

      _p += this->y_stride*2*(dy>>15);
      dy &= 32767;
    }
  } else {
    for (height = this->source_height; --height >= 0; ) { 
      dst = _dst;
      y = _p;
      for (width = this->source_width; --width >= 0; ) {
	*dst++ = *y;
	y += 2;
      }
      _dst += this->rgb_stride;
      _p += this->y_stride*2;
    }
  }
}

static void yuy22rgb_c_palette (yuv2rgb_t *this, uint8_t * _dst, uint8_t * _p)
{
  int U, V, Y;
  uint8_t * py_1, * pu, * pv;
  uint16_t * r, * g, * b;
  uint8_t * dst_1;
  int width, height;
  int dy;
  
  scale_line_4 (_p+1, this->u_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_4 (_p+3, this->v_buffer,
		this->dest_width >> 1, this->step_dx);
  scale_line_2 (_p, this->y_buffer,
		this->dest_width, this->step_dx);
    
  dy = 0;
  height = this->dest_height;
  
  for (;;) {
    dst_1 = _dst;
    py_1  = this->y_buffer;
    pu    = this->u_buffer;
    pv    = this->v_buffer;
    
    width = this->dest_width >> 3;
    
    do {
      RGB(0);
      DST1CMAP(0);

      RGB(1);
      DST1CMAP(1);

      RGB(2);
      DST1CMAP(2);

      RGB(3);
      DST1CMAP(3);

      pu += 4;
      pv += 4;
      py_1 += 8;
      dst_1 += 8;
    } while (--width);

    dy += this->step_dy;
    _dst += this->rgb_stride;

    while (--height > 0 && dy < 32768) {

      xine_fast_memcpy (_dst, (uint8_t*)_dst-this->rgb_stride, this->dest_width);

      dy += this->step_dy;
      _dst += this->rgb_stride;
    }

    if (height <= 0)
      break;

    _p += this->y_stride*2*(dy>>15);
    dy &= 32767;

    scale_line_4 (_p+1, this->u_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_4 (_p+3, this->v_buffer,
		  this->dest_width >> 1, this->step_dx);
    scale_line_2 (_p, this->y_buffer,
		  this->dest_width, this->step_dx);
  }
}

static void yuy22rgb_c_init (yuv2rgb_factory_t *this)
{
  switch (this->mode) {
  case MODE_32_RGB:
  case MODE_32_BGR:
    this->yuy22rgb_fun = yuy22rgb_c_32;
    break;

  case MODE_24_RGB:
  case MODE_24_BGR:
    this->yuy22rgb_fun =
	(this->mode==MODE_24_RGB && !this->swapped) || (this->mode==MODE_24_BGR && this->swapped)
	    ? yuy22rgb_c_24_rgb
	    : yuy22rgb_c_24_bgr;
    break;
  case MODE_15_BGR:
  case MODE_16_BGR:
  case MODE_15_RGB:
  case MODE_16_RGB:
    this->yuy22rgb_fun = yuy22rgb_c_16;
    break;

  case MODE_8_RGB:
  case MODE_8_BGR:
    this->yuy22rgb_fun = yuy22rgb_c_8;
    break;

  case MODE_8_GRAY:
    this->yuy22rgb_fun = yuy22rgb_c_gray;
    break;

  case MODE_PALETTE:
    this->yuy22rgb_fun = yuy22rgb_c_palette;
    break;

  default:
    printf ("yuv2rgb: mode %d not supported for yuy2\n", this->mode);
  }
}

yuv2rgb_t *yuv2rgb_create_converter (yuv2rgb_factory_t *factory) {

  yuv2rgb_t *this = xine_xmalloc (sizeof (yuv2rgb_t));
  
  this->cmap                     = factory->cmap;

  this->y_chunk = this->y_buffer = NULL;
  this->u_chunk = this->u_buffer = NULL;
  this->v_chunk = this->v_buffer = NULL;

  this->table_rV                 = factory->table_rV;
  this->table_gU                 = factory->table_gU;
  this->table_gV                 = factory->table_gV;
  this->table_bU                 = factory->table_bU;

  this->yuv2rgb_fun              = factory->yuv2rgb_fun;
  this->yuy22rgb_fun             = factory->yuy22rgb_fun;
  this->yuv2rgb_single_pixel_fun = factory->yuv2rgb_single_pixel_fun;

  this->configure                = yuv2rgb_configure;
  return this;
}

/*
 * factory functions 
 */

void yuv2rgb_set_gamma (yuv2rgb_factory_t *this, int gamma) {

  int i;
  
  for (i = 0; i < 256; i++) {
    (uint8_t *)this->table_rV[i] += this->entry_size*(gamma - this->gamma);
    (uint8_t *)this->table_gU[i] += this->entry_size*(gamma - this->gamma);
    (uint8_t *)this->table_bU[i] += this->entry_size*(gamma - this->gamma);
  }
#ifdef ARCH_X86
  mmx_yuv2rgb_set_gamma(gamma);
#endif  
  this->gamma = gamma;
}

int yuv2rgb_get_gamma (yuv2rgb_factory_t *this) {

  return this->gamma;
}

yuv2rgb_factory_t* yuv2rgb_factory_init (int mode, int swapped, 
					 uint8_t *cmap) {

  yuv2rgb_factory_t *this;

#ifdef ARCH_X86
  uint32_t mm = xine_mm_accel();
#endif

  this = malloc (sizeof (yuv2rgb_factory_t));

  this->mode                = mode;
  this->swapped             = swapped;
  this->cmap                = cmap;
  this->create_converter    = yuv2rgb_create_converter;
  this->set_gamma           = yuv2rgb_set_gamma;
  this->get_gamma           = yuv2rgb_get_gamma;
  this->matrix_coefficients = 6;


  yuv2rgb_setup_tables (this, mode, swapped);

  /*
   * auto-probe for the best yuv2rgb function
   */

  this->yuv2rgb_fun = NULL;
#ifdef ARCH_X86
  if ((this->yuv2rgb_fun == NULL) && (mm & MM_ACCEL_X86_MMXEXT)) {

    yuv2rgb_init_mmxext (this);

    if (this->yuv2rgb_fun != NULL)
      printf ("yuv2rgb: using MMXEXT for colorspace transform\n");
  }

  if ((this->yuv2rgb_fun == NULL) && (mm & MM_ACCEL_X86_MMX)) {

    yuv2rgb_init_mmx (this);

    if (this->yuv2rgb_fun != NULL)
      printf ("yuv2rgb: using MMX for colorspace transform\n");
  }
#endif
#if HAVE_MLIB
  if (this->yuv2rgb_fun == NULL) {

    yuv2rgb_init_mlib (this);

    if (this->yuv2rgb_fun != NULL)
      printf ("yuv2rgb: using medialib for colorspace transform\n");
  }
#endif
#ifdef __arm__
  if (this->yuv2rgb_fun == NULL) {
  yuv2rgb_init_arm ( this );
  
  if(this->yuv2rgb_fun != NULL)
  	printf("yuv2rgb: using arm4l assembler for colorspace transform\n" );
  }
#endif
  if (this->yuv2rgb_fun == NULL) {
    printf ("yuv2rgb: no accelerated colorspace conversion found\n");
    yuv2rgb_c_init (this);
  }

  /*
   * auto-probe for the best yuy22rgb function
   */

  /* FIXME: implement mmx/mlib functions */
  yuy22rgb_c_init (this);

  /*
   * set up single pixel function
   */

  yuv2rgb_single_pixel_init (this);

  return this;
}

