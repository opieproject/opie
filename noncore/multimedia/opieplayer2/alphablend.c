//TOAST_SPU will define ALL spu entries - no matter the tranparency
//#define TOAST_SPU
/* #define PRIV_CLUT */
/* Currently only blend_yuv(..) works */
/*
 *
 * Copyright (C) James Courtier-Dutton James@superbug.demon.co.uk - July 2001
 * 
 * Copyright (C) 2000  Thomas Mirlacher
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * The author may be reached as <dent@linuxvideo.org>
 *
 *------------------------------------------------------------
 *
 */

/*
#define LOG_BLEND_YUV
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <xine/video_out.h>
#include "alphablend.h"


#define BLEND_COLOR(dst, src, mask, o) ((((src&mask)*o + ((dst&mask)*(0x0f-o)))/0xf) & mask)

#define BLEND_BYTE(dst, src, o) (((src)*o + ((dst)*(0xf-o)))/0xf)

static void mem_blend16(uint16_t *mem, uint16_t clr, uint8_t o, int len) {
  uint16_t *limit = mem + len;
  while (mem < limit) {
    *mem =
     BLEND_COLOR(*mem, clr, 0xf800, o) |
     BLEND_COLOR(*mem, clr, 0x07e0, o) |
     BLEND_COLOR(*mem, clr, 0x001f, o);
    mem++;
  }
}

static void mem_blend24(uint8_t *mem, uint8_t r, uint8_t g, uint8_t b,
 uint8_t o, int len) {
  uint8_t *limit = mem + len*3;
  while (mem < limit) {
    *mem = BLEND_BYTE(*mem, r, o);
    mem++;
    *mem = BLEND_BYTE(*mem, g, o);
    mem++;
    *mem = BLEND_BYTE(*mem, b, o);
    mem++;
  }
}

static void mem_blend24_32(uint8_t *mem, uint8_t r, uint8_t g, uint8_t b,
 uint8_t o, int len) {
  uint8_t *limit = mem + len*4;
  while (mem < limit) {
    *mem = BLEND_BYTE(*mem, r, o);
    mem++;
    *mem = BLEND_BYTE(*mem, g, o);
    mem++;
    *mem = BLEND_BYTE(*mem, b, o);
    mem += 2;
  }
}

static void mem_blend32(uint8_t *mem, uint8_t *src, uint8_t o, int len) {
  uint8_t *limit = mem + len*4;
  while (mem < limit) {
    *mem = BLEND_BYTE(*mem, src[0], o);
    mem++;
    *mem = BLEND_BYTE(*mem, src[1], o);
    mem++;
    *mem = BLEND_BYTE(*mem, src[2], o);
    mem++;
    *mem = BLEND_BYTE(*mem, src[3], o);
    mem++;
  }
}


/*
 * Some macros for fixed point arithmetic.
 *
 * The blend_rgb* routines perform rle image scaling using
 * scale factors that are expressed as integers scaled with
 * a factor of 2**16.
 *
 * INT_TO_SCALED()/SCALED_TO_INT() converts from integer
 * to scaled fixed point and back.
 */
#define	SCALE_SHIFT	  16
#define	SCALE_FACTOR	  (1<<SCALE_SHIFT)
#define	INT_TO_SCALED(i)  ((i)  << SCALE_SHIFT)
#define	SCALED_TO_INT(sc) ((sc) >> SCALE_SHIFT)


static rle_elem_t *
rle_img_advance_line(rle_elem_t *rle, rle_elem_t *rle_limit, int w)
{
  int x;

  for (x = 0; x < w && rle < rle_limit; ) {
    x += rle->len;
    rle++;
  }
  return rle;
}


void blend_rgb16 (uint8_t * img, vo_overlay_t * img_overl,
		  int img_width, int img_height,
		  int dst_width, int dst_height)
{
  uint8_t *trans;
  clut_t* clut = (clut_t*) img_overl->clip_color;

  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x, y, x1_scaled, x2_scaled;
  int dy, dy_step, x_scale;	/* scaled 2**SCALE_SHIFT */
  int clip_right;
  uint16_t *img_pix;

  dy_step = INT_TO_SCALED(dst_height) / img_height;
  x_scale = INT_TO_SCALED(img_width)  / dst_width;

  img_pix = (uint16_t *) img
      + (img_overl->y * img_height / dst_height) * img_width
      + (img_overl->x * img_width / dst_width);

  trans = img_overl->clip_trans;

  /* avoid wraping overlay if drawing to small image */
  if( (img_overl->x + img_overl->clip_right) < dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - 1 - img_overl->x;

  /* avoid buffer overflow */
  if( (src_height + img_overl->y) >= dst_height )
    src_height = dst_height - 1 - img_overl->y;

  for (y = dy = 0; y < src_height && rle < rle_limit;) {
    int mask = !(img_overl->clip_top > y || img_overl->clip_bottom < y);
    rle_elem_t *rle_start = rle;

    for (x = x1_scaled = 0; x < src_width;) {
      uint8_t clr;
      uint16_t o;
      int rlelen;

      clr = rle->color;
      o   = trans[clr];
      rlelen = rle->len;

      if (o && mask) {
        /* threat cases where clipping border is inside rle->len pixels */
        if ( img_overl->clip_left > x ) {
          if( img_overl->clip_left < x + rlelen ) {
            x1_scaled = SCALED_TO_INT( img_overl->clip_left * x_scale );
            rlelen -= img_overl->clip_left - x;
            x += img_overl->clip_left - x;
          } else {
            o = 0;
          }
        } else if( clip_right < x + rlelen ) {
          if( clip_right > x ) {
            x2_scaled = SCALED_TO_INT( clip_right * x_scale);
            mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o,
                        x2_scaled-x1_scaled);
            o = 0;            
          } else {
            o = 0;
          }
        } 
      }
      
      x2_scaled = SCALED_TO_INT((x + rlelen) * x_scale);
      if (o && mask) {
	mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o, x2_scaled-x1_scaled);
      }

      x1_scaled = x2_scaled;
      x += rlelen;
      rle++;
      if (rle >= rle_limit) break;
    }

    img_pix += img_width;
    dy += dy_step;
    if (dy >= INT_TO_SCALED(1)) {
      dy -= INT_TO_SCALED(1);
      ++y;
      while (dy >= INT_TO_SCALED(1)) {
	rle = rle_img_advance_line(rle, rle_limit, src_width);
	dy -= INT_TO_SCALED(1);
	++y;
      }
    } else {
      rle = rle_start;		/* y-scaling, reuse the last rle encoded line */
    }
  }
}

void blend_rgb24 (uint8_t * img, vo_overlay_t * img_overl,
		  int img_width, int img_height,
		  int dst_width, int dst_height)
{
  clut_t* clut = (clut_t*) img_overl->clip_color;
  uint8_t *trans;
  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x, y, x1_scaled, x2_scaled;
  int dy, dy_step, x_scale;	/* scaled 2**SCALE_SHIFT */
  int clip_right;
  uint8_t *img_pix;

  dy_step = INT_TO_SCALED(dst_height) / img_height;
  x_scale = INT_TO_SCALED(img_width)  / dst_width;

  img_pix = img + 3 * (  (img_overl->y * img_height / dst_height) * img_width
		       + (img_overl->x * img_width  / dst_width));

  trans = img_overl->clip_trans;

  /* avoid wraping overlay if drawing to small image */
  if( (img_overl->x + img_overl->clip_right) < dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - 1 - img_overl->x;

  /* avoid buffer overflow */
  if( (src_height + img_overl->y) >= dst_height )
    src_height = dst_height - 1 - img_overl->y;

  for (dy = y = 0; y < src_height && rle < rle_limit; ) {
    int mask = !(img_overl->clip_top > y || img_overl->clip_bottom < y);
    rle_elem_t *rle_start = rle;

    for (x = x1_scaled = 0; x < src_width;) {
      uint8_t clr;
      uint16_t o;
      int rlelen;

      clr = rle->color;
      o   = trans[clr];
      rlelen = rle->len;

      if (o && mask) {
        /* threat cases where clipping border is inside rle->len pixels */
        if ( img_overl->clip_left > x ) {
          if( img_overl->clip_left < x + rlelen ) {
            x1_scaled = SCALED_TO_INT( img_overl->clip_left * x_scale );
            rlelen -= img_overl->clip_left - x;
            x += img_overl->clip_left - x;
          } else {
            o = 0;
          }
        } else if( clip_right < x + rlelen ) {
          if( clip_right > x ) {
            x2_scaled = SCALED_TO_INT( clip_right * x_scale);
            mem_blend24(img_pix + x1_scaled*3, clut[clr].cb,
                    clut[clr].cr, clut[clr].y,
                    o, x2_scaled-x1_scaled);
            o = 0;            
          } else {
            o = 0;
          }
        } 
      }
      
      x2_scaled = SCALED_TO_INT((x + rlelen) * x_scale);
      if (o && mask) {
        mem_blend24(img_pix + x1_scaled*3, clut[clr].cb,
                    clut[clr].cr, clut[clr].y,
                    o, x2_scaled-x1_scaled);
      }

      x1_scaled = x2_scaled;
      x += rlelen;
      rle++;
      if (rle >= rle_limit) break;
    }

    img_pix += img_width * 3;
    dy += dy_step;
    if (dy >= INT_TO_SCALED(1)) {
      dy -= INT_TO_SCALED(1);
      ++y;
      while (dy >= INT_TO_SCALED(1)) {
	rle = rle_img_advance_line(rle, rle_limit, src_width);
	dy -= INT_TO_SCALED(1);
	++y;
      }
    } else {
      rle = rle_start;		/* y-scaling, reuse the last rle encoded line */
    }
  }
}

void blend_rgb32 (uint8_t * img, vo_overlay_t * img_overl,
		  int img_width, int img_height,
		  int dst_width, int dst_height)
{
  clut_t* clut = (clut_t*) img_overl->clip_color;
  uint8_t *trans;
  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x, y, x1_scaled, x2_scaled;
  int dy, dy_step, x_scale;	/* scaled 2**SCALE_SHIFT */
  int clip_right;
  uint8_t *img_pix;

  dy_step = INT_TO_SCALED(dst_height) / img_height;
  x_scale = INT_TO_SCALED(img_width)  / dst_width;

  img_pix = img + 4 * (  (img_overl->y * img_height / dst_height) * img_width
		       + (img_overl->x * img_width / dst_width));

  trans = img_overl->clip_trans;

  /* avoid wraping overlay if drawing to small image */
  if( (img_overl->x + img_overl->clip_right) < dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - 1 - img_overl->x;

  /* avoid buffer overflow */
  if( (src_height + img_overl->y) >= dst_height )
    src_height = dst_height - 1 - img_overl->y;

  for (y = dy = 0; y < src_height && rle < rle_limit; ) {
    int mask = !(img_overl->clip_top > y || img_overl->clip_bottom < y);
    rle_elem_t *rle_start = rle;

    for (x = x1_scaled = 0; x < src_width;) {
      uint8_t clr;
      uint16_t o;
      int rlelen;

      clr = rle->color;
      o   = trans[clr];
      rlelen = rle->len;

      if (o && mask) {
        /* threat cases where clipping border is inside rle->len pixels */
        if ( img_overl->clip_left > x ) {
          if( img_overl->clip_left < x + rlelen ) {
            x1_scaled = SCALED_TO_INT( img_overl->clip_left * x_scale );
            rlelen -= img_overl->clip_left - x;
            x += img_overl->clip_left - x;
          } else {
            o = 0;
          }
        } else if( clip_right < x + rlelen ) {
          if( clip_right > x ) {
            x2_scaled = SCALED_TO_INT( clip_right * x_scale);
            mem_blend24_32(img_pix + x1_scaled*4, clut[clr].cb,
                    clut[clr].cr, clut[clr].y,
                    o, x2_scaled-x1_scaled);
            o = 0;            
          } else {
            o = 0;
          }
        } 
      }

      x2_scaled = SCALED_TO_INT((x + rlelen) * x_scale);
      if (o && mask) {
        mem_blend24_32(img_pix + x1_scaled*4, clut[clr].cb,
                    clut[clr].cr, clut[clr].y,
                    o, x2_scaled-x1_scaled);
      }

      x1_scaled = x2_scaled;
      x += rlelen;
      rle++;
      if (rle >= rle_limit) break;
    }

    img_pix += img_width * 4;
    dy += dy_step;
    if (dy >= INT_TO_SCALED(1)) {
      dy -= INT_TO_SCALED(1);
      ++y;
      while (dy >= INT_TO_SCALED(1)) {
	rle = rle_img_advance_line(rle, rle_limit, src_width);
	dy -= INT_TO_SCALED(1);
	++y;
      }
    } else {
      rle = rle_start;		/* y-scaling, reuse the last rle encoded line */
    }
  }
}

static void mem_blend8(uint8_t *mem, uint8_t val, uint8_t o, size_t sz)
{
  uint8_t *limit = mem + sz;
  while (mem < limit) {
    *mem = BLEND_BYTE(*mem, val, o);
    mem++;
  }
}

void blend_yuv (uint8_t *dst_base[3], vo_overlay_t * img_overl,
                int dst_width, int dst_height)
{
  clut_t *my_clut;
  uint8_t *my_trans;

  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x_off = img_overl->x;
  int y_off = img_overl->y;
  int ymask,xmask;
  int rle_this_bite;
  int rle_remainder;
  int rlelen;
  int x, y;
  int clip_right;
  uint8_t clr=0;

  uint8_t *dst_y = dst_base[0] + dst_width * y_off + x_off;
  uint8_t *dst_cr = dst_base[2] +
    (y_off / 2) * (dst_width / 2) + (x_off / 2) + 1;
  uint8_t *dst_cb = dst_base[1] +
    (y_off / 2) * (dst_width / 2) + (x_off / 2) + 1;
#ifdef LOG_BLEND_YUV
  printf("overlay_blend started x=%d, y=%d, w=%d h=%d\n",img_overl->x,img_overl->y,img_overl->width,img_overl->height);
#endif
  my_clut = (clut_t*) img_overl->clip_color;
  my_trans = img_overl->clip_trans;

  /* avoid wraping overlay if drawing to small image */
  if( (x_off + img_overl->clip_right) < dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - 1 - x_off;

  /* avoid buffer overflow */
  if( (src_height + y_off) >= dst_height )
    src_height = dst_height - 1 - y_off;

  rlelen=rle_remainder=0;
  for (y = 0; y < src_height; y++) {
    ymask = ((img_overl->clip_top > y) || (img_overl->clip_bottom < y));
    xmask = 0;
#ifdef LOG_BLEND_YUV
    printf("X started ymask=%d y=%d src_height=%d\n",ymask, y, src_height);
#endif

    for (x = 0; x < src_width;) {
      uint16_t o;
#ifdef LOG_BLEND_YUV
      printf("1:rle_len=%d, remainder=%d, x=%d\n",rlelen, rle_remainder, x);
#endif

      if ((rlelen < 0) || (rle_remainder < 0)) {
        printf("alphablend: major bug in blend_yuv < 0\n");
      } 
      if (rlelen == 0) {
        rle_remainder = rlelen = rle->len;
        clr = rle->color;
        rle++;
      }
      if (rle_remainder == 0) {
        rle_remainder = rlelen;
      }
      if ((rle_remainder + x) > src_width) {
        /* Do something for long rlelengths */
        rle_remainder = src_width - x; 
        ;
      }
#ifdef LOG_BLEND_YUV
      printf("2:rle_len=%d, remainder=%d, x=%d\n",rlelen, rle_remainder, x);
#endif

      if (ymask == 0) {
        if (x <= img_overl->clip_left) { 
          /* Starts outside clip area */
          if ((x + rle_remainder - 1) > img_overl->clip_left ) {
#ifdef LOG_BLEND_YUV
            printf("Outside clip left %d, ending inside\n", img_overl->clip_left); 
#endif
            /* Cutting needed, starts outside, ends inside */
            rle_this_bite = (img_overl->clip_left - x + 1);
            rle_remainder -= rle_this_bite;
            rlelen -= rle_this_bite;
            my_clut = (clut_t*) img_overl->color;
            my_trans = img_overl->trans;
            xmask = 0;
          } else {
#ifdef LOG_BLEND_YUV
            printf("Outside clip left %d, ending outside\n", img_overl->clip_left); 
#endif
          /* no cutting needed, starts outside, ends outside */
            rle_this_bite = rle_remainder;
            rle_remainder = 0;
            rlelen -= rle_this_bite;
            my_clut = (clut_t*) img_overl->color;
            my_trans = img_overl->trans;
            xmask = 0;
          }
        } else if (x < clip_right) {
          /* Starts inside clip area */
          if ((x + rle_remainder) > clip_right ) {
#ifdef LOG_BLEND_YUV
            printf("Inside clip right %d, ending outside\n", clip_right);
#endif
            /* Cutting needed, starts inside, ends outside */
            rle_this_bite = (clip_right - x);
            rle_remainder -= rle_this_bite;
            rlelen -= rle_this_bite;
            my_clut = (clut_t*) img_overl->clip_color;
            my_trans = img_overl->clip_trans;
            xmask++;
          } else {
#ifdef LOG_BLEND_YUV
            printf("Inside clip right %d, ending inside\n", clip_right);
#endif
          /* no cutting needed, starts inside, ends inside */
            rle_this_bite = rle_remainder;
            rle_remainder = 0;
            rlelen -= rle_this_bite;
            my_clut = (clut_t*) img_overl->clip_color;
            my_trans = img_overl->clip_trans;
            xmask++;
          }
        } else if (x >= clip_right) {
          /* Starts outside clip area, ends outsite clip area */
          if ((x + rle_remainder ) > src_width ) { 
#ifdef LOG_BLEND_YUV
            printf("Outside clip right %d, ending eol\n", clip_right);
#endif
            /* Cutting needed, starts outside, ends at right edge */
            /* It should never reach here due to the earlier test of src_width */
            rle_this_bite = (src_width - x );
            rle_remainder -= rle_this_bite;
            rlelen -= rle_this_bite;
            my_clut = (clut_t*) img_overl->color;
            my_trans = img_overl->trans;
            xmask = 0;
          } else {
          /* no cutting needed, starts outside, ends outside */
#ifdef LOG_BLEND_YUV
            printf("Outside clip right %d, ending outside\n", clip_right);
#endif
            rle_this_bite = rle_remainder;
            rle_remainder = 0;
            rlelen -= rle_this_bite;
            my_clut = (clut_t*) img_overl->color;
            my_trans = img_overl->trans;
            xmask = 0;
          }
        }
      } else {
        /* Outside clip are due to y */
        /* no cutting needed, starts outside, ends outside */
        rle_this_bite = rle_remainder;
        rle_remainder = 0;
        rlelen -= rle_this_bite;
        my_clut = (clut_t*) img_overl->color;
        my_trans = img_overl->trans;
        xmask = 0;
      }
      o   = my_trans[clr];
#ifdef LOG_BLEND_YUV
      printf("Trans=%d clr=%d xmask=%d my_clut[clr]=%d\n",o, clr, xmask, my_clut[clr].y);
#endif
      if (o) {
        if(o >= 15) {
	  memset(dst_y + x, my_clut[clr].y, rle_this_bite);
	  if (y & 1) {
	    memset(dst_cr + (x >> 1), my_clut[clr].cr, (rle_this_bite+1) >> 1);
	    memset(dst_cb + (x >> 1), my_clut[clr].cb, (rle_this_bite+1) >> 1);
	  }
        } else {
	  mem_blend8(dst_y + x, my_clut[clr].y, o, rle_this_bite);
	  if (y & 1) {
            /* Blending cr and cb should use a different function, with pre -128 to each sample */
	    mem_blend8(dst_cr + (x >> 1), my_clut[clr].cr, o, (rle_this_bite+1) >> 1);
	    mem_blend8(dst_cb + (x >> 1), my_clut[clr].cb, o, (rle_this_bite+1) >> 1);
	  }
        }

      }
#ifdef LOG_BLEND_YUV
      printf("rle_this_bite=%d, remainder=%d, x=%d\n",rle_this_bite, rle_remainder, x);
#endif
      x += rle_this_bite;
      if (rle >= rle_limit) {
#ifdef LOG_BLEND_YUV
        printf("x-rle_limit\n");
#endif
        break;
      }
    }
    if (rle >= rle_limit) {
#ifdef LOG_BLEND_YUV
        printf("x-rle_limit\n");
#endif
        break;
    }

    dst_y += dst_width;

    if (y & 1) {
      dst_cr += (dst_width + 1) / 2;
      dst_cb += (dst_width + 1) / 2;
    }
  }
#ifdef LOG_BLEND_YUV
  printf("overlay_blend ended\n");
#endif
}
            
void blend_yuy2 (uint8_t * dst_img, vo_overlay_t * img_overl,
                int dst_width, int dst_height)
{
  clut_t *my_clut;
  uint8_t *my_trans;

  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x_off = img_overl->x;
  int y_off = img_overl->y;
  int mask;
  int x, y;
  int l;
  int clip_right;
  uint32_t yuy2;
  
  uint8_t *dst_y = dst_img + 2 * (dst_width * y_off + x_off);
  uint8_t *dst;

  my_clut = (clut_t*) img_overl->clip_color;
  my_trans = img_overl->clip_trans;

  /* avoid wraping overlay if drawing to small image */
  if( (x_off + img_overl->clip_right) < dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - 1 - x_off;

  /* avoid buffer overflow */
  if( (src_height + y_off) >= dst_height )
    src_height = dst_height - 1 - y_off;

  for (y = 0; y < src_height; y++) {
    mask = !(img_overl->clip_top > y || img_overl->clip_bottom < y);

    dst = dst_y;
    for (x = 0; x < src_width;) {
      uint8_t clr;
      uint16_t o;
      int rlelen;

      clr = rle->color;
      o   = my_trans[clr];
      rlelen = rle->len;

      if (o && mask) {
        /* threat cases where clipping border is inside rle->len pixels */
        if ( img_overl->clip_left > x ) {
          if( img_overl->clip_left < x + rlelen ) {
            rlelen -= img_overl->clip_left - x;
            x += img_overl->clip_left - x;
          } else {
            o = 0;
          }
        } else if( clip_right < x + rlelen ) {
          if( clip_right > x ) {
            /* fixme: case not implemented */
            o = 0;            
          } else {
            o = 0;
          }
        } 
      }
      

      if (o && mask) {
        l = rlelen>>1;
        if( !(x & 1) ) {
          yuy2 =  my_clut[clr].y + (my_clut[clr].cb << 8) +
                 (my_clut[clr].y << 16) + (my_clut[clr].cr << 24);
        } else {
          yuy2 =  my_clut[clr].y + (my_clut[clr].cr << 8) +
                 (my_clut[clr].y << 16) + (my_clut[clr].cb << 24);
        }
        
	if (o >= 15) {
	  while(l--) {
	    *((uint32_t *)dst)++ = yuy2;
	  }
	  if(rlelen & 1)
	    *((uint16_t *)dst)++ = yuy2 & 0xffff;
	} else {
	  if( l ) {
	    mem_blend32(dst, (uint8_t *)&yuy2, o, l);
	    dst += 4*l;
	  }
	  
	  if(rlelen & 1) {
	    *dst = BLEND_BYTE(*dst, *((uint8_t *)&yuy2), o);
	    dst++;
	    *dst = BLEND_BYTE(*dst, *((uint8_t *)&yuy2+1), o);
	    dst++;
	  }
	}
      } else {
        dst += rlelen*2;
      }

      x += rlelen;
      rle++;
      if (rle >= rle_limit) break;
    }
    if (rle >= rle_limit) break;

    dst_y += dst_width*2;
  }
}
