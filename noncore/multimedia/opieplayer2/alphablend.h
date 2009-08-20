/*
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

#ifndef __ALPHABLEND_H__
#define __ALPHABLEND_H__

#include <xine/video_out.h>

typedef struct {
  void *buffer;
  int buffer_size;

  int disable_exact_blending;
} alphablend_t;

void _x_alphablend_init(alphablend_t *extra_data, xine_t *xine);
void _x_alphablend_free(alphablend_t *extra_data);

/* _MSC_VER port changes */
#undef ATTRIBUTE_PACKED
#undef PRAGMA_PACK_BEGIN
#undef PRAGMA_PACK_END

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95) || defined(__ICC)
#define ATTRIBUTE_PACKED __attribute__ ((packed))
#define PRAGMA_PACK 0
#endif

#if !defined(ATTRIBUTE_PACKED)
#define ATTRIBUTE_PACKED
#define PRAGMA_PACK 1
#endif

#if PRAGMA_PACK
#pragma pack(8)
#endif

typedef struct {         /* CLUT == Color LookUp Table */
  uint8_t cb    : 8;
  uint8_t cr    : 8;
  uint8_t y     : 8;
  uint8_t foo   : 8;
} ATTRIBUTE_PACKED clut_t;


#if PRAGMA_PACK
#pragma pack()
#endif

#define XX44_PALETTE_SIZE 32

typedef struct {
  unsigned size;
  unsigned max_used;
  uint32_t cluts[XX44_PALETTE_SIZE];
  /* cache palette entries for both colors and clip_colors */
  int lookup_cache[OVL_PALETTE_SIZE*2];
} xx44_palette_t;


void blend_rgb16 (uint8_t * img, vo_overlay_t * img_overl,
		  int img_width, int img_height,
		  int dst_width, int dst_height,
                  alphablend_t *extra_data);

void blend_rgb24 (uint8_t * img, vo_overlay_t * img_overl,
		  int img_width, int img_height,
		  int dst_width, int dst_height,
                  alphablend_t *extra_data);

void blend_rgb32 (uint8_t * img, vo_overlay_t * img_overl,
		  int img_width, int img_height,
		  int dst_width, int dst_height,
                  alphablend_t *extra_data);

void blend_yuv (uint8_t *dst_base[3], vo_overlay_t * img_overl,
                int dst_width, int dst_height, int dst_pitches[3],
                alphablend_t *extra_data);

void blend_yuy2 (uint8_t * dst_img, vo_overlay_t * img_overl,
                 int dst_width, int dst_height, int dst_pitch,
                 alphablend_t *extra_data);

/*
 * This function isn't too smart about blending. We want to avoid creating new
 * colors in the palette as a result from two non-zero colors needed to be
 * blended. Instead we choose the color with the highest alpha value to be
 * visible. Some parts of the code taken from the "VeXP" project.
 */

void blend_xx44 (uint8_t *dst_img, vo_overlay_t *img_overl,
		int dst_width, int dst_height, int dst_pitch,
                alphablend_t *extra_data,
		xx44_palette_t *palette,int ia44);

/*
 * Functions to handle the xine-specific palette.
 */

void clear_xx44_palette(xx44_palette_t *p);
void init_xx44_palette(xx44_palette_t *p, unsigned num_entries);
void dispose_xx44_palette(xx44_palette_t *p);

/*
 * Convert the xine-specific palette to something useful.
 */

void xx44_to_xvmc_palette(const xx44_palette_t *p,unsigned char *xvmc_palette,
			  unsigned first_xx44_entry, unsigned num_xx44_entries,
			  unsigned num_xvmc_components, char *xvmc_components);


#endif
