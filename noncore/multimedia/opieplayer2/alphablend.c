/*
 *
 * Copyright (C) James Courtier-Dutton James@superbug.demon.co.uk - July 2001
 * 
 * Copyright (C) 2000  Thomas Mirlacher
 *               2002-2004 the xine project
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
#define LOG_BLEND_RGB16
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <xine/xine_internal.h>
#include <xine/video_out.h>
#include "alphablend.h"
#include "bswap.h"


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
                  int dst_width, int dst_height,
                  alphablend_t *extra_data)
{
  uint8_t *trans;
  clut_t *clut;

  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_start = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x, y, x1_scaled, x2_scaled;
  int dy, dy_step, x_scale;     /* scaled 2**SCALE_SHIFT */
  int clip_right;
  uint16_t *img_pix;
  int rlelen;
  int rle_this_bite;
  int rle_remainder;
  int zone_state=0;
  uint8_t clr_next,clr;
  uint16_t o;
  double img_offset;
  int stripe_height;
/*
 * Let zone_state keep state.
 * 0 = Starting.
 * 1 = Above button.
 * 2 = Left of button.
 * 3 = Inside of button.
 * 4 = Right of button.
 * 5 = Below button.
 * 6 = Finished.
 *
 * Each time round the loop, update the state.
 * We can do this easily and cheaply(fewer IF statements per cycle) as we are testing rle end position anyway.
 * Possible optimization is to ensure that rle never overlaps from outside to inside a button.
 * Possible optimization is to pre-scale the RLE overlay, so that no scaling is needed here.
 */

#ifdef LOG_BLEND_RGB16
  printf("blend_rgb16: img_height=%i, dst_height=%i\n", img_height, dst_height);
  printf("blend_rgb16: img_width=%i, dst_width=%i\n", img_width, dst_width);
  if (img_width & 1) { printf("blend_rgb16s: odd\n");}
  else { printf("blend_rgb16s: even\n");}

#endif
/* stripe_height is used in yuv2rgb scaling, so use the same scale factor here for overlays. */
  stripe_height = 16 * img_height / dst_height;
/*  dy_step = INT_TO_SCALED(dst_height) / img_height; */
  dy_step = INT_TO_SCALED(16) / stripe_height;
  x_scale = INT_TO_SCALED(img_width)  / dst_width;
#ifdef LOG_BLEND_RGB16
  printf("blend_rgb16: dy_step=%i, x_scale=%i\n", dy_step, x_scale);
#endif
  if (img_width & 1) img_width++;
  img_offset = ( ( (img_overl->y * img_height) / dst_height) * img_width) 
             + ( (img_overl->x * img_width) / dst_width);
#ifdef LOG_BLEND_RGB16
  printf("blend_rgb16: x=%i, y=%i, w=%i, h=%i, img_offset=%lf\n", img_overl->x, img_overl->y,
    img_overl->width,
    img_overl->height,
    img_offset);
#endif
  img_pix = (uint16_t *) img + (int)img_offset;
/* 
      + (img_overl->y * img_height / dst_height) * img_width
      + (img_overl->x * img_width / dst_width);
*/

  /* avoid wraping overlay if drawing to small image */
  if( (img_overl->x + img_overl->clip_right) < dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - img_overl->x;

  /* avoid buffer overflow */
  if( (src_height + img_overl->y) >= dst_height )
    src_height = dst_height - img_overl->y;

  rlelen = rle_remainder = rle_this_bite = 0;
  rle_remainder = rlelen = rle->len;
  clr_next = rle->color;
  rle++;
  y = dy = 0;
  x = x1_scaled = x2_scaled = 0;

#ifdef LOG_BLEND_RGB16
  printf("blend_rgb16 started\n"); 
#endif

  while (zone_state != 6) {
    clr = clr_next;
    switch (zone_state) {
    case 0:  /* Starting */
      /* FIXME: Get libspudec to set clip_top to -1 if no button */
      if (img_overl->clip_top < 0) {
#ifdef LOG_BLEND_RGB16
        printf("blend_rgb16: No button clip area\n");
#endif

        zone_state = 7;
        break;
      }
#ifdef LOG_BLEND_RGB16
      printf("blend_rgb16: Button clip area found. (%d,%d) .. (%d,%d)\n",
        img_overl->clip_left,
        img_overl->clip_top,
        img_overl->clip_right,
        img_overl->clip_bottom);
#endif
      if (y < img_overl->clip_top) {
        zone_state = 1;
        break;
      } else if (y >= img_overl->clip_bottom) {
        zone_state = 5;
        break;
      } else if (x < img_overl->clip_left) {
        zone_state = 2;
        break;
      } else if (x >= img_overl->clip_right) {
        zone_state = 4;
        break;
      } else {
        zone_state = 3;
        break;
      }
      break;
    case 1:  /* Above clip area */
      clut = (clut_t*) img_overl->color;
      trans = img_overl->trans;
      o   = trans[clr];
      rle_this_bite = rle_remainder;
      rle_remainder = 0;
      rlelen -= rle_this_bite;
      /*printf("(x,y) = (%03i,%03i), clr=%03x, len=%03i, zone=%i\n", x, y, clr, rle_this_bite, zone_state); */
      if (o) {
        x1_scaled = SCALED_TO_INT( x * x_scale );
        x2_scaled = SCALED_TO_INT( (x + rle_this_bite) * x_scale);
        mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o, x2_scaled-x1_scaled);
      }
      x += rle_this_bite;
      if (x >= src_width ) { 
        x -= src_width;
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
          rle_start = rle;
        } else {
          rle = rle_start;          /* y-scaling, reuse the last rle encoded line */
        }
      } 
      rle_remainder = rlelen = rle->len;
      clr_next = rle->color;
      rle++;
      if (rle >= rle_limit) {
        zone_state = 6;
      }
      if (y >= img_overl->clip_top) {
        zone_state = 2;
#ifdef LOG_BLEND_RGB16
        printf("blend_rgb16: Button clip top reached. y=%i, top=%i\n",
                y, img_overl->clip_top);
#endif
        if (x >= img_overl->clip_left) {
          zone_state = 3;
          if (x >= img_overl->clip_right) {
            zone_state = 4;
          }
        }
      }
      break;
    case 2:  /* Left of button */
      clut = (clut_t*) img_overl->color;
      trans = img_overl->trans;
      o   = trans[clr];
      if (x + rle_remainder <= img_overl->clip_left) {
        rle_this_bite = rle_remainder;
        rle_remainder = rlelen = rle->len;
        clr_next = rle->color;
        rle++;
      } else {
        rle_this_bite = img_overl->clip_left - x;
        rle_remainder -= rle_this_bite;
        zone_state = 3;
      }
      if (o) {
        x1_scaled = SCALED_TO_INT( x * x_scale );
        x2_scaled = SCALED_TO_INT( (x + rle_this_bite) * x_scale);
        mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o, x2_scaled-x1_scaled);
      }
      x += rle_this_bite;
      if (x >= src_width ) { 
        x -= src_width;
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
          rle_start = rle;
        } else {
          rle = rle_start;          /* y-scaling, reuse the last rle encoded line */
        }
        if (y >= img_overl->clip_bottom) {
          zone_state = 5;
          break;
        }
      }
      if (rle >= rle_limit) {
        zone_state = 6;
      }
      break;
    case 3:  /* In button */
      clut = (clut_t*) img_overl->clip_color;
      trans = img_overl->clip_trans;
      o   = trans[clr];
      if (x + rle_remainder <= img_overl->clip_right) {
        rle_this_bite = rle_remainder;
        rle_remainder = rlelen = rle->len;
        clr_next = rle->color;
        rle++;
      } else {
        rle_this_bite = img_overl->clip_right - x;
        rle_remainder -= rle_this_bite;
        zone_state = 4;
      }
      if (o) {
        x1_scaled = SCALED_TO_INT( x * x_scale );
        x2_scaled = SCALED_TO_INT( (x + rle_this_bite) * x_scale);
        mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o, x2_scaled-x1_scaled);
      }
      x += rle_this_bite;
      if (x >= src_width ) { 
        x -= src_width;
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
          rle_start = rle;
        } else {
          rle = rle_start;          /* y-scaling, reuse the last rle encoded line */
        }
        if (y >= img_overl->clip_bottom) {
          zone_state = 5;
          break;
        }
      } 
      if (rle >= rle_limit) {
        zone_state = 6;
      }
      break;
    case 4:  /* Right of button */
      clut = (clut_t*) img_overl->color;
      trans = img_overl->trans;
      o   = trans[clr];
      if (x + rle_remainder <= src_width) {
        rle_this_bite = rle_remainder;
        rle_remainder = rlelen = rle->len;
        clr_next = rle->color;
        rle++;
      } else {
        rle_this_bite = src_width - x;
        rle_remainder -= rle_this_bite;
        zone_state = 2;
      }
      if (o) {
        x1_scaled = SCALED_TO_INT( x * x_scale );
        x2_scaled = SCALED_TO_INT( (x + rle_this_bite) * x_scale);
        mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o, x2_scaled-x1_scaled);
      }
      x += rle_this_bite;
      if (x >= src_width ) { 
        x -= src_width;
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
          rle_start = rle;
        } else {
          rle = rle_start;          /* y-scaling, reuse the last rle encoded line */
        }
        if (y >= img_overl->clip_bottom) {
          zone_state = 5;
          break;
        }
      } 
      if (rle >= rle_limit) {
        zone_state = 6;
      }
      break;
    case 5:  /* Below button */
      clut = (clut_t*) img_overl->color;
      trans = img_overl->trans;
      o   = trans[clr];
      rle_this_bite = rle_remainder;
      rle_remainder = 0;
      rlelen -= rle_this_bite;
      if (o) {
        x1_scaled = SCALED_TO_INT( x * x_scale );
        x2_scaled = SCALED_TO_INT( (x + rle_this_bite) * x_scale);
        mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o, x2_scaled-x1_scaled);
      }
      x += rle_this_bite;
      if (x >= src_width ) { 
        x -= src_width;
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
          rle_start = rle;
        } else {
          rle = rle_start;          /* y-scaling, reuse the last rle encoded line */
        }
      } 
      rle_remainder = rlelen = rle->len;
      clr_next = rle->color;
      rle++;
      if (rle >= rle_limit) {
        zone_state = 6;
      }
      break;
    case 6:  /* Finished */
      _x_abort();

    case 7:  /* No button */
      clut = (clut_t*) img_overl->color;
      trans = img_overl->trans;
      o   = trans[clr];
      rle_this_bite = rle_remainder;
      rle_remainder = 0;
      rlelen -= rle_this_bite;
      if (o) {
        x1_scaled = SCALED_TO_INT( x * x_scale );
        x2_scaled = SCALED_TO_INT( (x + rle_this_bite) * x_scale);
        mem_blend16(img_pix+x1_scaled, *((uint16_t *)&clut[clr]), o, x2_scaled-x1_scaled);
      }
      x += rle_this_bite;
      if (x >= src_width ) { 
        x -= src_width;
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
          rle_start = rle;
        } else {
          rle = rle_start;          /* y-scaling, reuse the last rle encoded line */
        }
      } 
      rle_remainder = rlelen = rle->len;
      clr_next = rle->color;
      rle++;
      if (rle >= rle_limit) {
        zone_state = 6;
      }
      break;
    default:
      ;
    }
  }
#ifdef LOG_BLEND_RGB16
  printf("blend_rgb16 ended\n");
#endif
 
}

void blend_rgb24 (uint8_t * img, vo_overlay_t * img_overl,
		  int img_width, int img_height,
		  int dst_width, int dst_height,
                  alphablend_t *extra_data)
{
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

  /* avoid wraping overlay if drawing to small image */
  if( (img_overl->x + img_overl->clip_right) <= dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - img_overl->x;

  /* avoid buffer overflow */
  if( (src_height + img_overl->y) > dst_height )
    src_height = dst_height - img_overl->y;

  for (dy = y = 0; y < src_height && rle < rle_limit; ) {
    int mask = !(y < img_overl->clip_top || y >= img_overl->clip_bottom);
    rle_elem_t *rle_start = rle;

    int rlelen = 0;
    uint8_t clr = 0;

    for (x = x1_scaled = 0; x < src_width;) {
      int rle_bite;
      clut_t *colors;
      uint8_t *trans;
      uint16_t o;

      /* take next element from rle list everytime an element is finished */
      if (rlelen <= 0) {
        if (rle >= rle_limit)
          break;
        
        rlelen = rle->len;
        clr = rle->color;
        rle++;
      }

      if (!mask) {
        /* above or below clipping area */
        
        rle_bite = rlelen;
        /* choose palette for surrounding area */
        colors = (clut_t*)img_overl->color;
        trans = img_overl->trans;
      } else {
        /* treat cases where clipping border is inside rle->len pixels */
        if ( x < img_overl->clip_left ) {
          /* starts left */
          if( x + rlelen > img_overl->clip_left ) {
            /* ends not left */

            /* choose the largest "bite" up to palette change */
            rle_bite = img_overl->clip_left - x;
            /* choose palette for surrounding area */
            colors = (clut_t*)img_overl->color;
            trans = img_overl->trans;
          } else {
            /* ends left */
            
            rle_bite = rlelen;
            /* choose palette for surrounding area */
            colors = (clut_t*)img_overl->color;
            trans = img_overl->trans;
          }
        } else if( x + rlelen > clip_right ) {
          /* ends right */
          if( x < clip_right ) {
            /* starts not right */

            /* choose the largest "bite" up to palette change */
            rle_bite = clip_right - x;
            /* we're in the center area so choose clip palette */
            colors = (clut_t*)img_overl->clip_color;
            trans = img_overl->clip_trans;
          } else {
            /* starts right */
            
            rle_bite = rlelen;
            /* choose palette for surrounding area */
            colors = (clut_t*)img_overl->color;
            trans = img_overl->trans;
          }
        } else {
          /* starts not left and ends not right */
          
          rle_bite = rlelen;
          /* we're in the center area so choose clip palette */
          colors = (clut_t*)img_overl->clip_color;
          trans = img_overl->clip_trans;
        }
      }
      
      x2_scaled = SCALED_TO_INT((x + rle_bite) * x_scale);

      o = trans[clr];
      if (o) {
        mem_blend24(img_pix + x1_scaled*3,
                    colors[clr].cb, colors[clr].cr, colors[clr].y,
                    o, x2_scaled-x1_scaled);
      }

      x1_scaled = x2_scaled;
      x += rle_bite;
      rlelen -= rle_bite;
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
		  int dst_width, int dst_height,
                  alphablend_t *extra_data)
{
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

  /* avoid wraping overlay if drawing to small image */
  if( (img_overl->x + img_overl->clip_right) <= dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - img_overl->x;

  /* avoid buffer overflow */
  if( (src_height + img_overl->y) > dst_height )
    src_height = dst_height - img_overl->y;

  for (y = dy = 0; y < src_height && rle < rle_limit; ) {
    int mask = !(y < img_overl->clip_top || y >= img_overl->clip_bottom);
    rle_elem_t *rle_start = rle;

    int rlelen = 0;
    uint8_t clr = 0;
    
    for (x = x1_scaled = 0; x < src_width;) {
      int rle_bite;
      clut_t *colors;
      uint8_t *trans;
      uint16_t o;
      
      /* take next element from rle list everytime an element is finished */
      if (rlelen <= 0) {
        if (rle >= rle_limit)
          break;
        
        rlelen = rle->len;
        clr = rle->color;
        rle++;
      }

      if (!mask) {
        /* above or below clipping area */
        
        rle_bite = rlelen;
        /* choose palette for surrounding area */
        colors = (clut_t*)img_overl->color;
        trans = img_overl->trans;
      } else {
        /* treat cases where clipping border is inside rle->len pixels */
        if ( x < img_overl->clip_left ) {
          /* starts left */
          if( x + rlelen > img_overl->clip_left ) {
            /* ends not left */

            /* choose the largest "bite" up to palette change */
            rle_bite = img_overl->clip_left - x;
            /* choose palette for surrounding area */
            colors = (clut_t*)img_overl->color;
            trans = img_overl->trans;
          } else {
            /* ends left */
            
            rle_bite = rlelen;
            /* choose palette for surrounding area */
            colors = (clut_t*)img_overl->color;
            trans = img_overl->trans;
          }
        } else if( x + rlelen > clip_right ) {
          /* ends right */
          if( x < clip_right ) {
            /* starts not right */

            /* choose the largest "bite" up to palette change */
            rle_bite = clip_right - x;
            /* we're in the center area so choose clip palette */
            colors = (clut_t*)img_overl->clip_color;
            trans = img_overl->clip_trans;
          } else {
            /* starts right */
            
            rle_bite = rlelen;
            /* choose palette for surrounding area */
            colors = (clut_t*)img_overl->color;
            trans = img_overl->trans;
          }
        } else {
          /* starts not left and ends not right */
          
          rle_bite = rlelen;
          /* we're in the center area so choose clip palette */
          colors = (clut_t*)img_overl->clip_color;
          trans = img_overl->clip_trans;
        }
      }
      
      x2_scaled = SCALED_TO_INT((x + rle_bite) * x_scale);

      o = trans[clr];
      if (o) {
        mem_blend32(img_pix + x1_scaled*4, (uint8_t *)&colors[clr], o, x2_scaled-x1_scaled);
      }

      x1_scaled = x2_scaled;
      x += rle_bite;
      rlelen -= rle_bite;
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

static void blend_yuv_exact(uint8_t *dst_cr, uint8_t *dst_cb,
                            int src_width, int x_odd,
                            uint8_t *(*blend_yuv_data)[ 3 ][ 2 ])
{
  int x;
  
  for (x = 0; x < src_width; x += 2) {
    /* get opacity of the 4 pixels that share chroma */
    int o00 = (*blend_yuv_data)[ 0 ][ 0 ][ x + 0 ];
    int o01 = (*blend_yuv_data)[ 0 ][ 0 ][ x + 1 ];
    int o10 = (*blend_yuv_data)[ 0 ][ 1 ][ x + 0 ];
    int o11 = (*blend_yuv_data)[ 0 ][ 1 ][ x + 1 ];

    /* are there any pixels a little bit opaque? */
    if (o00 || o01 || o10 || o11) {
      /* get the chroma components of the 4 pixels */
      int cr00 = -128 + (*blend_yuv_data)[ 1 ][ 0 ][ x + 0 ];
      int cr01 = -128 + (*blend_yuv_data)[ 1 ][ 0 ][ x + 1 ];
      int cr10 = -128 + (*blend_yuv_data)[ 1 ][ 1 ][ x + 0 ];
      int cr11 = -128 + (*blend_yuv_data)[ 1 ][ 1 ][ x + 1 ];
          
      int cb00 = -128 + (*blend_yuv_data)[ 2 ][ 0 ][ x + 0 ];
      int cb01 = -128 + (*blend_yuv_data)[ 2 ][ 0 ][ x + 1 ];
      int cb10 = -128 + (*blend_yuv_data)[ 2 ][ 1 ][ x + 0 ];
      int cb11 = -128 + (*blend_yuv_data)[ 2 ][ 1 ][ x + 1 ];

      /* are all pixels completely opaque? */
      if (o00 >= 0xf && o01 >= 0xf && o10 >= 0xf && o11 >= 0xf) {
        /* set the output chroma to the average of the four pixels */
        *dst_cr = 128 + (cr00 + cr01 + cr10 + cr11) / 4;
        *dst_cb = 128 + (cb00 + cb01 + cb10 + cb11) / 4;
      } else {
        int t4, cr, cb;
        
        /* blending required, so clamp opacity values to allowed range */
        if (o00 > 0xf) o00 = 0xf;
        if (o01 > 0xf) o01 = 0xf;
        if (o10 > 0xf) o10 = 0xf;
        if (o11 > 0xf) o11 = 0xf;

        /* calculate transparency of background over the four pixels */
        t4 = (0xf - o00) + (0xf - o01) + (0xf - o10) + (0xf - o11);

        /* get background chroma */
        cr = -128 + *dst_cr;
        cb = -128 + *dst_cb;

        /* blend the output chroma to the average of the four pixels */
        *dst_cr = 128 + (cr * t4 + cr00 * o00 + cr01 * o01 + cr10 * o10 + cr11 * o11) / (4 * 0xf);
        *dst_cb = 128 + (cb * t4 + cb00 * o00 + cb01 * o01 + cb10 * o10 + cb11 * o11) / (4 * 0xf);
      }
    }

    /* next chroma sample */
    dst_cr++;
    dst_cb++;
  }
}

static uint8_t *(*blend_yuv_grow_extra_data(alphablend_t *extra_data, int osd_width))[ 3 ][ 2 ]
{
  struct __attribute__((packed)) header_s {
    int id;
    int max_width;
    uint8_t *data[ 3 ][ 2 ];
  } **header = (struct header_s **)&extra_data->buffer;
  
  int needed_buffer_size = sizeof (**header) + sizeof (uint8_t[ 3 ][ 2 ][ osd_width ]);
  
  if (extra_data->buffer_size < needed_buffer_size) {
    
    free(extra_data->buffer);
    extra_data->buffer = xine_xmalloc(needed_buffer_size);

    if (!extra_data->buffer) {
      extra_data->buffer_size = 0;
      return 0;
    }
    
    extra_data->buffer_size = needed_buffer_size;
    (*header)->max_width = 0;
  }

  if ((*header)->id != ME_FOURCC('y', 'u', 'v', 0) || (*header)->max_width < osd_width) {
    (*header)->id = ME_FOURCC('y', 'u', 'v', 0);
    (*header)->max_width = osd_width;

    (*header)->data[ 0 ][ 0 ] = ((uint8_t *)extra_data->buffer) + sizeof (**header);
    (*header)->data[ 0 ][ 1 ] = (*header)->data[ 0 ][ 0 ] + osd_width;
    (*header)->data[ 1 ][ 0 ] = (*header)->data[ 0 ][ 1 ] + osd_width;
    (*header)->data[ 1 ][ 1 ] = (*header)->data[ 1 ][ 0 ] + osd_width;
    (*header)->data[ 2 ][ 0 ] = (*header)->data[ 1 ][ 1 ] + osd_width;
    (*header)->data[ 2 ][ 1 ] = (*header)->data[ 2 ][ 0 ] + osd_width;
  }

  return &(*header)->data;
}
  
void blend_yuv (uint8_t *dst_base[3], vo_overlay_t * img_overl,
                int dst_width, int dst_height, int dst_pitches[3],
                alphablend_t *extra_data)
{
  int enable_exact_blending = !extra_data->disable_exact_blending;
  clut_t *my_clut;
  uint8_t *my_trans;

  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x_off = img_overl->x;
  int y_off = img_overl->y;
  int x_odd = x_off & 1;
  int y_odd = y_off & 1;
  int ymask,xmask;
  int rle_this_bite;
  int rle_remainder;
  int rlelen;
  int x, y;
  int clip_right;
  uint8_t clr=0;
  
  int any_line_buffered = 0;
  int exact_blend_width = ((src_width <= (dst_width - x_off)) ? src_width : (dst_width - x_off));
  int exact_blend_width_m2 = (x_odd + exact_blend_width + 1) & ~1; /* make it a (larger) multiple of 2 */
  uint8_t *(*blend_yuv_data)[ 3 ][ 2 ] = 0;
  
  uint8_t *dst_y = dst_base[0] + dst_pitches[0] * y_off + x_off;
  uint8_t *dst_cr = dst_base[2] + (y_off / 2) * dst_pitches[1] + (x_off / 2);
  uint8_t *dst_cb = dst_base[1] + (y_off / 2) * dst_pitches[2] + (x_off / 2);
#ifdef LOG_BLEND_YUV
  printf("overlay_blend started x=%d, y=%d, w=%d h=%d\n",img_overl->x,img_overl->y,img_overl->width,img_overl->height);
#endif
  my_clut = (clut_t*) img_overl->clip_color;
  my_trans = img_overl->clip_trans;

  /* avoid wraping overlay if drawing to small image */
  if( (x_off + img_overl->clip_right) <= dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - x_off;

  /* avoid buffer overflow */
  if( (src_height + y_off) > dst_height )
    src_height = dst_height - y_off;

  if (src_height <= 0)
    return;

  if (enable_exact_blending) {
    if (exact_blend_width <= 0)
      return;
  
    blend_yuv_data = blend_yuv_grow_extra_data(extra_data, exact_blend_width_m2);
    if (!blend_yuv_data)
      return;
    
    /* make linebuffer transparent */
    memset(&(*blend_yuv_data)[ 0 ][ 0 ][ 0 ], 0, exact_blend_width_m2);
    memset(&(*blend_yuv_data)[ 0 ][ 1 ][ 0 ], 0, exact_blend_width_m2);
  }
  
  rlelen=rle_remainder=0;
  for (y = 0; y < src_height; y++) {
    if (rle >= rle_limit) {
#ifdef LOG_BLEND_YUV
      printf("y-rle_limit\n");
#endif
      break;
    }
    
    ymask = ((y < img_overl->clip_top) || (y >= img_overl->clip_bottom));
    xmask = 0;
#ifdef LOG_BLEND_YUV
    printf("X started ymask=%d y=%d src_height=%d\n",ymask, y, src_height);
#endif

    for (x = 0; x < src_width;) {
      uint16_t o;
      
      if (rle >= rle_limit) {
#ifdef LOG_BLEND_YUV
        printf("x-rle_limit\n");
#endif
        break;
      }
      
#ifdef LOG_BLEND_YUV
      printf("1:rle_len=%d, remainder=%d, x=%d\n",rlelen, rle_remainder, x);
#endif

      if ((rlelen < 0) || (rle_remainder < 0)) {
#ifdef LOG_BLEND_YUV
        printf("alphablend: major bug in blend_yuv < 0\n");
#endif
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
      }
#ifdef LOG_BLEND_YUV
      printf("2:rle_len=%d, remainder=%d, x=%d\n",rlelen, rle_remainder, x);
#endif

      if (ymask == 0) {
        if (x < img_overl->clip_left) { 
          /* Starts outside clip area */
          if ((x + rle_remainder) > img_overl->clip_left ) {
#ifdef LOG_BLEND_YUV
            printf("Outside clip left %d, ending inside\n", img_overl->clip_left); 
#endif
            /* Cutting needed, starts outside, ends inside */
            rle_this_bite = (img_overl->clip_left - x);
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
          /* Starts outside clip area, ends outside clip area */
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

      if (x < (dst_width - x_off)) {
        /* clip against right edge of destination area */
        if ((x + rle_this_bite) > (dst_width - x_off)) {
          int toClip = (x + rle_this_bite) - (dst_width - x_off);
          
          rle_this_bite -= toClip;
          rle_remainder += toClip;
          rlelen += toClip;
        }

        if (enable_exact_blending) {
          /* remember opacity of current line */
          memset(&(*blend_yuv_data)[ 0 ][ (y + y_odd) & 1 ][ x + x_odd ], o, rle_this_bite);
          any_line_buffered |= ((y + y_odd) & 1) ? 2 : 1;
        }
      
        if (o) {
          if(o >= 15) {
            memset(dst_y + x, my_clut[clr].y, rle_this_bite);
            if (!enable_exact_blending) {
              if ((y + y_odd) & 1) {
                memset(dst_cr + ((x + x_odd) >> 1), my_clut[clr].cr, (rle_this_bite+1) >> 1);
                memset(dst_cb + ((x + x_odd) >> 1), my_clut[clr].cb, (rle_this_bite+1) >> 1);
              }
            }
          } else {
            mem_blend8(dst_y + x, my_clut[clr].y, o, rle_this_bite);
            if (!enable_exact_blending) {
              if ((y + y_odd) & 1) {
                /* Blending cr and cb should use a different function, with pre -128 to each sample */
                mem_blend8(dst_cr + ((x + x_odd) >> 1), my_clut[clr].cr, o, (rle_this_bite+1) >> 1);
                mem_blend8(dst_cb + ((x + x_odd) >> 1), my_clut[clr].cb, o, (rle_this_bite+1) >> 1);
              }
            }
          }

          if (enable_exact_blending) {
            /* remember chroma of current line */
            memset(&(*blend_yuv_data)[ 1 ][ (y + y_odd) & 1 ][ x + x_odd ], my_clut[ clr ].cr, rle_this_bite);
            memset(&(*blend_yuv_data)[ 2 ][ (y + y_odd) & 1 ][ x + x_odd ], my_clut[ clr ].cb, rle_this_bite);
          }
        }
      }
#ifdef LOG_BLEND_YUV
      printf("rle_this_bite=%d, remainder=%d, x=%d\n",rle_this_bite, rle_remainder, x);
#endif
      x += rle_this_bite;
    }

    if ((y + y_odd) & 1) {
      if (enable_exact_blending) {
        /* blend buffered lines */
        if (any_line_buffered) {
          if (!(any_line_buffered & 2)) {
            /* make second line transparent */
            memset(&(*blend_yuv_data)[ 0 ][ 1 ][ 0 ], 0, exact_blend_width_m2);
          }
          
          blend_yuv_exact(dst_cr, dst_cb, exact_blend_width, x_odd, blend_yuv_data);
          
          any_line_buffered = 0;
        }
      }
      
      dst_cr += dst_pitches[2];
      dst_cb += dst_pitches[1];
    }
    
    dst_y += dst_pitches[0];
  }

  if (enable_exact_blending) {
    /* blend buffered lines */
    if (any_line_buffered) {
      if (!(any_line_buffered & 2)) {
        /* make second line transparent */
        memset(&(*blend_yuv_data)[ 0 ][ 1 ][ 0 ], 0, exact_blend_width_m2);
      }
      
      blend_yuv_exact(dst_cr, dst_cb, exact_blend_width, x_odd, blend_yuv_data);
    }
  }
      
#ifdef LOG_BLEND_YUV
  printf("overlay_blend ended\n");
#endif
}
            
static void blend_yuy2_exact(uint8_t *dst_cr, uint8_t *dst_cb,
                             int src_width, int x_odd,
                             uint8_t *(*blend_yuy2_data)[ 3 ])
{
  int x;
  
  for (x = 0; x < src_width; x += 2) {
    /* get opacity of the 2 pixels that share chroma */
    int o0 = (*blend_yuy2_data)[ 0 ][ x + 0 ];
    int o1 = (*blend_yuy2_data)[ 0 ][ x + 1 ];

    /* are there any pixels a little bit opaque? */
    if (o0 || o1) {
      /* get the chroma components of the 2 pixels */
      int cr0 = -128 + (*blend_yuy2_data)[ 1 ][ x + 0 ];
      int cr1 = -128 + (*blend_yuy2_data)[ 1 ][ x + 1 ];
          
      int cb0 = -128 + (*blend_yuy2_data)[ 2 ][ x + 0 ];
      int cb1 = -128 + (*blend_yuy2_data)[ 2 ][ x + 1 ];

      /* are all pixels completely opaque? */
      if (o0 >= 0xf && o1 >= 0xf) {
        /* set the output chroma to the average of the two pixels */
        *dst_cr = 128 + (cr0 + cr1) / 2;
        *dst_cb = 128 + (cb0 + cb1) / 2;
      } else {
        int t2, cr, cb;
        
        /* blending required, so clamp opacity values to allowed range */
        if (o0 > 0xf) o0 = 0xf;
        if (o1 > 0xf) o1 = 0xf;

        /* calculate transparency of background over the two pixels */
        t2 = (0xf - o0) + (0xf - o1);

        /* get background chroma */
        cr = -128 + *dst_cr;
        cb = -128 + *dst_cb;

        /* blend the output chroma to the average of the two pixels */
        *dst_cr = 128 + (cr * t2 + cr0 * o0 + cr1 * o1) / (2 * 0xf);
        *dst_cb = 128 + (cb * t2 + cb0 * o0 + cb1 * o1) / (2 * 0xf);
      }
    }

    /* next chroma sample */
    dst_cr += 4;
    dst_cb += 4;
  }
}

static uint8_t *(*blend_yuy2_grow_extra_data(alphablend_t *extra_data, int osd_width))[ 3 ]
{
  struct __attribute__((packed)) header_s {
    int id;
    int max_width;
    uint8_t *data[ 3 ];
  } **header = (struct header_s **)&extra_data->buffer;
  
  int needed_buffer_size = sizeof (**header) + sizeof (uint8_t[ 3 ][ osd_width ]);
  
  if (extra_data->buffer_size < needed_buffer_size) {
    
    free(extra_data->buffer);
    extra_data->buffer = xine_xmalloc(needed_buffer_size);

    if (!extra_data->buffer) {
      extra_data->buffer_size = 0;
      return 0;
    }
    
    extra_data->buffer_size = needed_buffer_size;
    (*header)->max_width = 0;
  }

  if ((*header)->id != ME_FOURCC('y', 'u', 'y', '2') || (*header)->max_width < osd_width) {
    (*header)->id = ME_FOURCC('y', 'u', 'y', '2');
    (*header)->max_width = osd_width;

    (*header)->data[ 0 ] = ((uint8_t *)extra_data->buffer) + sizeof (**header);
    (*header)->data[ 1 ] = (*header)->data[ 0 ] + osd_width;
    (*header)->data[ 2 ] = (*header)->data[ 1 ] + osd_width;
  }

  return &(*header)->data;
}

void blend_yuy2 (uint8_t * dst_img, vo_overlay_t * img_overl,
                 int dst_width, int dst_height, int dst_pitch,
                 alphablend_t *extra_data)
{
  int enable_exact_blending = !extra_data->disable_exact_blending;
  clut_t *my_clut;
  uint8_t *my_trans;

  int src_width = img_overl->width;
  int src_height = img_overl->height;
  rle_elem_t *rle = img_overl->rle;
  rle_elem_t *rle_limit = rle + img_overl->num_rle;
  int x_off = img_overl->x;
  int y_off = img_overl->y;
  int x_odd = x_off & 1;
  int ymask;
  int rle_this_bite;
  int rle_remainder;
  int rlelen;
  int x, y;
  int l = 0;
  int clip_right;

  union {
    uint32_t value;
    uint8_t  b[4];
    uint16_t h[2];
  } yuy2;

  uint8_t clr = 0;

  int any_line_buffered = 0;
  int exact_blend_width = ((src_width <= (dst_width - x_off)) ? src_width : (dst_width - x_off));
  int exact_blend_width_m2 = (x_odd + exact_blend_width + 1) & ~1; /* make it a (larger) multiple of 2 */
  uint8_t *(*blend_yuy2_data)[ 3 ] = 0;
  
  uint8_t *dst_y = dst_img + dst_pitch * y_off + 2 * x_off;
  uint8_t *dst;

  my_clut = (clut_t*) img_overl->clip_color;
  my_trans = img_overl->clip_trans;

  /* avoid wraping overlay if drawing to small image */
  if( (x_off + img_overl->clip_right) <= dst_width )
    clip_right = img_overl->clip_right;
  else
    clip_right = dst_width - x_off;

  /* avoid buffer overflow */
  if( (src_height + y_off) > dst_height )
    src_height = dst_height - y_off;

  if (src_height <= 0)
    return;

  if (enable_exact_blending) {
    if (exact_blend_width <= 0)
      return;
    
    blend_yuy2_data = blend_yuy2_grow_extra_data(extra_data, exact_blend_width_m2);
    if (!blend_yuy2_data)
      return;
    
    /* make linebuffer transparent */
    memset(&(*blend_yuy2_data)[ 0 ][ 0 ], 0, exact_blend_width_m2);
  }

  rlelen=rle_remainder=0;
  for (y = 0; y < src_height; y++) {
    if (rle >= rle_limit)
      break;
    
    ymask = ((y < img_overl->clip_top) || (y >= img_overl->clip_bottom));

    dst = dst_y;
    for (x = 0; x < src_width;) {
      uint16_t o;

      if (rle >= rle_limit)
        break;
    
      if ((rlelen < 0) || (rle_remainder < 0)) {
#ifdef LOG_BLEND_YUV
        printf("alphablend: major bug in blend_yuv < 0\n");
#endif
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
      }
#ifdef LOG_BLEND_YUV
      printf("2:rle_len=%d, remainder=%d, x=%d\n",rlelen, rle_remainder, x);
#endif

      if (ymask == 0) {
        if (x < img_overl->clip_left) { 
          /* Starts outside clip area */
          if ((x + rle_remainder) > img_overl->clip_left ) {
#ifdef LOG_BLEND_YUV
            printf("Outside clip left %d, ending inside\n", img_overl->clip_left); 
#endif
            /* Cutting needed, starts outside, ends inside */
            rle_this_bite = (img_overl->clip_left - x);
            rle_remainder -= rle_this_bite;
            rlelen -= rle_this_bite;
            my_clut = (clut_t*) img_overl->color;
            my_trans = img_overl->trans;
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
      }
      o   = my_trans[clr];

      if (x < (dst_width - x_off)) {
        /* clip against right edge of destination area */
        if ((x + rle_this_bite) > (dst_width - x_off)) {
          int toClip = (x + rle_this_bite) - (dst_width - x_off);
          
          rle_this_bite -= toClip;
          rle_remainder += toClip;
          rlelen += toClip;
        }

        if (enable_exact_blending) {
          /* remember opacity of current line */
          memset(&(*blend_yuy2_data)[ 0 ][ x + x_odd ], o, rle_this_bite);
          any_line_buffered = 1;
        }

        if (o) {
          if (!enable_exact_blending) {
            l = rle_this_bite>>1;
            if( !((x_odd+x) & 1) ) {
              yuy2.b[0] = my_clut[clr].y;
              yuy2.b[1] = my_clut[clr].cb;
              yuy2.b[2] = my_clut[clr].y;
              yuy2.b[3] = my_clut[clr].cr;
            } else {
              yuy2.b[0] = my_clut[clr].y;
              yuy2.b[1] = my_clut[clr].cr;
              yuy2.b[2] = my_clut[clr].y;
              yuy2.b[3] = my_clut[clr].cb;
            }
          }
	  if (o >= 15) {
            if (!enable_exact_blending) {
              while(l--) {
                *(uint16_t *)dst = yuy2.h[0];
                dst += 2;
                *(uint16_t *)dst = yuy2.h[1];
                dst += 2;
              }
              if(rle_this_bite & 1) {
                *(uint16_t *)dst = yuy2.h[0];
                dst += 2;
              }
            } else {
              l = rle_this_bite;
              while (l--) {
                *dst = my_clut[clr].y;
                dst += 2;
              }
            }
          } else {
            if (!enable_exact_blending) {
              if( l ) {
                mem_blend32(dst, &yuy2.b[0], o, l);
                dst += 4*l;
              }
              
              if(rle_this_bite & 1) {
                *dst = BLEND_BYTE(*dst, yuy2.b[0], o);
                dst++;
                *dst = BLEND_BYTE(*dst, yuy2.b[1], o);
                dst++;
              }
            } else {
              l = rle_this_bite;
              while (l--) {
                *dst = BLEND_BYTE(*dst, my_clut[clr].y, o);
                dst += 2;
              }
            }
          }

          if (enable_exact_blending) {
            /* remember chroma of current line */
            memset(&(*blend_yuy2_data)[ 1 ][ x + x_odd ], my_clut[ clr ].cr, rle_this_bite);
            memset(&(*blend_yuy2_data)[ 2 ][ x + x_odd ], my_clut[ clr ].cb, rle_this_bite);
          }
        } else {
          dst += rle_this_bite*2;
        }
      }
      
      x += rle_this_bite;
    }
    
    if (enable_exact_blending) {
      /* blend buffered line */
      if (any_line_buffered) {
        blend_yuy2_exact(dst_y - x_odd * 2 + 3, dst_y - x_odd * 2 + 1, exact_blend_width, x_odd, blend_yuy2_data);
        
        any_line_buffered = 0;
      }
    }

    dst_y += dst_pitch;
  }
}

void clear_xx44_palette(xx44_palette_t *p) 
{
  register int i;
  register uint32_t *cluts = p->cluts;
  register int *ids = p->lookup_cache;

  i= p->size;
  while(i--)
    *cluts++ = 0;
  i = 2*OVL_PALETTE_SIZE;
  while(i--)
    *ids++ = -1;
  p->max_used=1; 
}

void init_xx44_palette(xx44_palette_t *p, unsigned num_entries) 
{
  p->size = (num_entries > XX44_PALETTE_SIZE) ? XX44_PALETTE_SIZE : num_entries; 
}

void dispose_xx44_palette(xx44_palette_t *p) 
{
}

static void colorToPalette(const uint32_t *icolor, unsigned char *palette_p,
			   unsigned num_xvmc_components, char *xvmc_components) 
{
  const clut_t *color = (const clut_t *) icolor;
  int i;
  for (i=0; i<num_xvmc_components; ++i) {
    switch(xvmc_components[i]) {
    case 'V': *palette_p = color->cr; break;
    case 'U': *palette_p = color->cb; break;
    case 'Y': 
    default:  *palette_p = color->y; break;
    }
    *palette_p++;
  }
}


void xx44_to_xvmc_palette(const xx44_palette_t *p,unsigned char *xvmc_palette,
			  unsigned first_xx44_entry, unsigned num_xx44_entries,
			  unsigned num_xvmc_components, char *xvmc_components) 
{
  register int i;
  register const uint32_t *cluts = p->cluts + first_xx44_entry;

  for (i=0; i<num_xx44_entries; ++i) {
    if ((cluts - p->cluts) < p->size) {
      colorToPalette(cluts++, xvmc_palette, num_xvmc_components, xvmc_components);
      xvmc_palette += num_xvmc_components;
    }
  }
} 

static int xx44_paletteIndex(xx44_palette_t *p, int color, uint32_t clut) 
{

  register int i;
  register uint32_t *cluts = p->cluts;
  register int tmp;

  if ((tmp = p->lookup_cache[color]) >= 0) 
    if (cluts[tmp] == clut) return tmp;

  for (i=0; i<p->max_used; ++i) {
    if (*cluts++ == clut) return p->lookup_cache[color] = i;
  }

  if (p->max_used == p->size -1) {
    printf("video_out: Warning! Out of xx44 palette colors!\n");
    return 1;
  }
  p->cluts[p->max_used] = clut;
  return p->lookup_cache[color] = p->max_used++;
}

static void memblend_xx44(uint8_t *mem,uint8_t val, register size_t size, uint8_t mask)
{
  register uint8_t masked_val = val & mask;

  while(size--) {
    if ((*mem & mask) <= masked_val ) *mem = val;
    mem++;
  }
}

void blend_xx44 (uint8_t *dst_img, vo_overlay_t *img_overl,
		int dst_width, int dst_height, int dst_pitch, 
                alphablend_t *extra_data,
		xx44_palette_t *palette,int ia44) 
{
  int  src_width  = img_overl->width;
  int  src_height = img_overl->height;
  rle_elem_t *rle        = img_overl->rle;
  rle_elem_t *rle_limit  = rle + img_overl->num_rle;
  int mask;
  int x_off = img_overl->x;
  int y_off = img_overl->y;
  int x, y;
  uint8_t norm_pixel,clip_pixel;  
  uint8_t *dst_y;
  uint8_t *dst;
  uint8_t alphamask = (ia44) ? 0x0F : 0xF0;
  int clip_right;

  if (!img_overl)
    return;

  dst_y = dst_img + dst_pitch*y_off + x_off;

  if( (x_off + img_overl->width) <= dst_width )
    clip_right = img_overl->width;
  else
    clip_right = dst_width - x_off;

  if ((src_height + y_off) > dst_height)
    src_height = dst_height - y_off;

  for (y = 0; y < src_height; y++) {

    mask = !(y < img_overl->clip_top || y >= img_overl->clip_bottom);
    dst = dst_y;

    for (x = 0; x < src_width;) {
      int len = (x + rle->len > clip_right) ? clip_right - x : rle->len;

      if (len > 0) {
	norm_pixel = (uint8_t)((xx44_paletteIndex(palette,rle->color,
					     img_overl->color[rle->color]) << 4) | 
			       (img_overl->trans[rle->color] & 0x0F));
	clip_pixel = (uint8_t)((xx44_paletteIndex(palette,rle->color+OVL_PALETTE_SIZE,
					     img_overl->clip_color[rle->color]) << 4) | 
			       (img_overl->clip_trans[rle->color] & 0x0F));
	if (!ia44) {
	  norm_pixel = ((norm_pixel & 0x0F) << 4) | ((norm_pixel & 0xF0) >> 4);
	  clip_pixel = ((clip_pixel & 0x0F) << 4) | ((clip_pixel & 0xF0) >> 4);
	}
	if (mask) {
	  if (x < img_overl->clip_left) {
	    if (x + len <= img_overl->clip_left) {
	      memblend_xx44(dst,norm_pixel,len, alphamask);
	      dst += len;
	    } else {
	      memblend_xx44(dst,norm_pixel,img_overl->clip_left -x, alphamask);
	      dst += img_overl->clip_left - x;
	      len -= img_overl->clip_left - x;
	      if (len <= img_overl->clip_right - img_overl->clip_left) {
		memblend_xx44(dst,clip_pixel,len, alphamask);
		dst += len;
	      } else {
		memblend_xx44(dst,clip_pixel,img_overl->clip_right - img_overl->clip_left,
			      alphamask);
		dst += img_overl->clip_right - img_overl->clip_left;
		len -= img_overl->clip_right - img_overl->clip_left;
		memblend_xx44(dst,norm_pixel,len, alphamask);
		dst += len;
	      }
	    }
	  } else if (x < img_overl->clip_right) {  
	    if (len <= img_overl->clip_right - x) {
	      memblend_xx44(dst,clip_pixel,len, alphamask);
	      dst += len;
	    } else {
	      memblend_xx44(dst,clip_pixel,img_overl->clip_right - x,alphamask);
	      dst += img_overl->clip_right - x;
	      len -= img_overl->clip_right - x;
	      memblend_xx44(dst,norm_pixel,len, alphamask);
	      dst += len;
	    }
	  } else {
	    memblend_xx44(dst,norm_pixel,len, alphamask);
	    dst += len;
	  }
	} else {
	  memblend_xx44(dst,norm_pixel,len, alphamask);
	  dst += len;
	}
      }
      x += rle->len;
      rle++;
      if (rle >= rle_limit) break;
    }  
    if (rle >= rle_limit) break;
    dst_y += dst_pitch;
  }
}

static void alphablend_disable_exact_osd_alpha_blending_changed(void *user_data, xine_cfg_entry_t *entry)
{
  alphablend_t *extra_data = (alphablend_t *)user_data;

  extra_data->disable_exact_blending = entry->num_value;
}

void _x_alphablend_init(alphablend_t *extra_data, xine_t *xine)
{
  config_values_t *config = xine->config;
  
  extra_data->buffer = 0;
  extra_data->buffer_size = 0;

  extra_data->disable_exact_blending =
    config->register_bool(config, "video.output.disable_exact_alphablend", 0,
      _("disable exact alpha blending of overlays"),
      _("If you experience a performance impact when an On Screen Display or other "
        "overlays like DVD subtitles are active, then you might want to enable this option.\n"
        "The result is that alpha blending of overlays is less accurate than before, "
        "but the CPU usage will be decreased as well."),
      10, alphablend_disable_exact_osd_alpha_blending_changed, extra_data);
}

void _x_alphablend_free(alphablend_t *extra_data)
{
  if (extra_data->buffer) {
    free(extra_data->buffer);
    extra_data->buffer = NULL;
  }

  extra_data->buffer_size = 0;
}

