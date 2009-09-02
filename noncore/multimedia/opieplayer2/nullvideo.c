/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
			     Copyright (c)  2002-2003 Miguel Freitas of xine
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include <xine.h>
#include <xine/video_out.h>
#include <xine/xine_internal.h>
#include <xine/vo_scale.h>
#include <xine/buffer.h>

#include <pthread.h>
#include "alphablend.h"
#include "yuv2rgb.h"

#define printf(x,...)

/*
 * #define LOG
 */

/* the caller for our event draw handler */
typedef void (*display_xine_frame_t) (void *user_data, uint8_t* frame,
				      int  width, int height,int bytes );
typedef void (*vo_scale_cb) (void*, int, int, double,
			     int*, int*, int*, double*, int*, int* );
typedef void (*dest_size_cb) (void*, int, int, double, int*, int*, double*);


typedef struct null_driver_s null_driver_t;

struct null_driver_s {
  vo_driver_t     vo_driver;

  int                  m_show_video;
  int                  m_video_fullscreen;

  int                  depth, bpp, bytes_per_pixel;
  int                  yuv2rgb_mode;
  int                  yuv2rgb_swap;
  int                  yuv2rgb_brightness;
  uint8_t             *yuv2rgb_cmap;
  yuv2rgb_factory_t   *yuv2rgb_factory;

  vo_overlay_t        *overlay;
  vo_scale_t           sc;

  /*
   * DISPLAY to widget...
   */
  display_xine_frame_t frameDis;
  void *userData;

  xine_t              *xine;
  alphablend_t         alphablend_extra_data;
};

typedef struct opie_frame_s opie_frame_t;
struct opie_frame_s {
  vo_frame_t         frame;

  int                format;
  int                flags;

  vo_scale_t         sc;

  uint8_t           *chunk[3];

  uint8_t           *data; /* rgb */
  int                bytes_per_line;

  yuv2rgb_t         *yuv2rgb;
  uint8_t           *rgb_dst;
  int                yuv_stride;
  int                stripe_height, stripe_inc;

  null_driver_t     *output;
};

static uint32_t null_get_capabilities( vo_driver_t *self ){
    self = self;
    return VO_CAP_YUY2 | VO_CAP_YV12;
}

static void null_frame_proc_slice (vo_frame_t *vo_img, uint8_t **src) {
  opie_frame_t  *frame = (opie_frame_t *) vo_img ;

  vo_img->proc_called = 1;

  /*
   * drop hard if we don't show the video
   */
  if (!frame->output->m_show_video)
    return;

  if( frame->frame.crop_left  || frame->frame.crop_top ||
      frame->frame.crop_right || frame->frame.crop_bottom )
  {
    /* we don't support crop, so don't even waste cpu cycles.
     * cropping will be performed by video_out.c
     */
    return;
  }

  if (frame->format == XINE_IMGFMT_YV12) {
    frame->yuv2rgb->yuv2rgb_fun (frame->yuv2rgb, frame->rgb_dst,
				 src[0], src[1], src[2]);
  } else {

    frame->yuv2rgb->yuy22rgb_fun (frame->yuv2rgb, frame->rgb_dst,
				  src[0]);
  }

  frame->rgb_dst += frame->stripe_inc;
}

static void null_frame_field (vo_frame_t *vo_img, int which_field) {

  opie_frame_t  *frame = (opie_frame_t *) vo_img ;

  switch (which_field) {
  case VO_TOP_FIELD:
    frame->rgb_dst    = frame->data;
    frame->stripe_inc = 2*frame->stripe_height * frame->bytes_per_line;
    break;
  case VO_BOTTOM_FIELD:
    frame->rgb_dst    = frame->data + frame->bytes_per_line ;
    frame->stripe_inc = 2*frame->stripe_height * frame->bytes_per_line;
    break;
  case VO_BOTH_FIELDS:
    frame->rgb_dst    = frame->data;
    break;
  }
}


/* take care of the frame*/
static void null_frame_dispose( vo_frame_t* vo_img){
  opie_frame_t* frame = (opie_frame_t*)vo_img;

  if (frame->data)
    free( frame->data );
  free (frame);
}

/* end take care of frames*/

static vo_frame_t* null_alloc_frame( vo_driver_t* self ){

  null_driver_t* this = (null_driver_t*)self;
  opie_frame_t* frame;

#ifdef LOG
  fprintf (stderr, "nullvideo: alloc_frame\n");
#endif

  frame = (opie_frame_t*)xine_xmalloc ( sizeof(opie_frame_t) );
  if(!frame)
      return NULL;

  memcpy (&frame->sc, &this->sc, sizeof(vo_scale_t));

  pthread_mutex_init (&frame->frame.mutex, NULL);

  frame->output = this;

  /* initialize the frame*/
  frame->frame.driver = self;
  frame->frame.proc_slice = null_frame_proc_slice;
  frame->frame.proc_frame = NULL;
  frame->frame.field = null_frame_field;
  frame->frame.dispose = null_frame_dispose;

  /*
   *    colorspace converter for this frame
   */
  frame->yuv2rgb = this->yuv2rgb_factory->create_converter (this->yuv2rgb_factory);

  return (vo_frame_t*) frame;
}


static void null_frame_compute_ideal_size( null_driver_t *this,
					   opie_frame_t *frame ) {
    this = this;

    _x_vo_scale_compute_ideal_size(&frame->sc);
}

static void null_frame_compute_rgb_size( null_driver_t *this,
					 opie_frame_t  *frame ){
    this = this;

    _x_vo_scale_compute_output_size(&frame->sc);

    /* avoid problems in yuv2rgb */
    if(frame->sc.output_height < (frame->sc.delivered_height+15) >> 4)
	frame->sc.output_height = (frame->sc.delivered_height+15) >> 4;

    if (frame->sc.output_width < 8)
	frame->sc.output_width = 8;

    /* yuv2rgb_mlib needs an even YUV2 width */
    if (frame->sc.output_width & 1)
	frame->sc.output_width++;
}

static void null_frame_reallocate( null_driver_t *this, opie_frame_t *frame,
				   uint32_t width, uint32_t height, int format){
    /*
     * (re-) allocate
     */
    if( frame->chunk[0] ){
	free( frame->chunk[0] );
	frame->chunk[0] = NULL;
    }
    if( frame->chunk[1] ){
	free ( frame->chunk[1] );
	frame->chunk[1] = NULL;
    }
    if( frame->chunk[2] ){
	free ( frame->chunk[2] );
	frame->chunk[2] = NULL;
    }
    if(frame->data)
	free ( frame->data );

    frame->data = xine_xmalloc (frame->sc.output_width
				* frame->sc.output_height
				* this->bytes_per_pixel );

    if( format == XINE_IMGFMT_YV12 ) {
      frame->frame.pitches[0] = 8*((width + 7) / 8);
      frame->frame.pitches[1] = 8*((width + 15) / 16);
      frame->frame.pitches[2] = 8*((width + 15) / 16);
      frame->frame.base[0] = xine_xmalloc_aligned (16, frame->frame.pitches[0] * height,(void **)&frame->chunk[0]);
      frame->frame.base[1] = xine_xmalloc_aligned (16, frame->frame.pitches[1] * ((height+ 1)/2), (void **)&frame->chunk[1]);
      frame->frame.base[2] = xine_xmalloc_aligned (16, frame->frame.pitches[2] * ((height+ 1)/2), (void **)&frame->chunk[2]);

    }else{
      frame->frame.pitches[0] = 8*((width + 3) / 4);

      frame->frame.base[0] = xine_xmalloc_aligned (16, frame->frame.pitches[0] * height,
						   (void **)&frame->chunk[0]);
      frame->chunk[1] = NULL;
      frame->chunk[2] = NULL;
    }

}

static void null_setup_colorspace_converter(opie_frame_t *frame, int flags ) {
    switch (flags) {
	case VO_TOP_FIELD:
	case VO_BOTTOM_FIELD:
	    frame->yuv2rgb->configure (frame->yuv2rgb,
				       frame->sc.delivered_width,
				       16,
				       2*frame->frame.pitches[0],
				       2*frame->frame.pitches[1],
				       frame->sc.output_width,
				       frame->stripe_height,
				       frame->bytes_per_line*2);
	    frame->yuv_stride = frame->bytes_per_line*2;
	    break;
	case VO_BOTH_FIELDS:
	    frame->yuv2rgb->configure (frame->yuv2rgb,
				       frame->sc.delivered_width,
				       16,
				       frame->frame.pitches[0],
				       frame->frame.pitches[1],
				       frame->sc.output_width,
				       frame->stripe_height,
				       frame->bytes_per_line);
	    frame->yuv_stride = frame->bytes_per_line;
	    break;
    }
#ifdef LOG
    fprintf (stderr, "nullvideo: colorspace converter configured.\n");
#endif
}

static void null_update_frame_format( vo_driver_t* self, vo_frame_t* img,
				      uint32_t width, uint32_t height,
				      double ratio_code,  int format,
				      int flags ){
  null_driver_t* this = (null_driver_t*) self;
  opie_frame_t*  frame = (opie_frame_t*)img;

#ifdef LOG
  fprintf (stderr, "nullvideo: update_frame_format\n");
#endif

  int gui_width  = 0;
  int gui_height = 0;
  double gui_pixel_aspect = 0.0;

  flags &= VO_BOTH_FIELDS;

  frame->sc.dest_size_cb(frame->sc.user_data, width, height,
			 frame->sc.video_pixel_aspect,
			 &gui_width, &gui_height, &gui_pixel_aspect);

  /* find out if we need to adapt this frame */
  if ((width != frame->sc.delivered_width)
      || (height != frame->sc.delivered_height)
      || (ratio_code != frame->sc.delivered_ratio)
      || (flags != frame->flags)
      || (format != frame->format)
      || (this->sc.user_ratio != frame->sc.user_ratio)
      || (gui_width  != frame->sc.gui_width)
      || (gui_height != frame->sc.gui_height)) {
    frame->sc.delivered_width      = width;
    frame->sc.delivered_height     = height;
    frame->sc.delivered_ratio      = ratio_code;
    frame->flags                   = flags;
    frame->format                  = format;
    frame->sc.user_ratio           = this->sc.user_ratio;
    frame->sc.gui_width  = gui_width;
    frame->sc.gui_height = gui_height;


    null_frame_compute_ideal_size(this, frame);
    null_frame_compute_rgb_size(this, frame);
    null_frame_reallocate(this, frame, width, height, format);

#ifdef LOG
    fprintf (stderr, "nullvideo: gui %dx%d delivered %dx%d output %dx%d\n",
       frame->sc.gui_width, frame->sc.gui_height,
       frame->sc.delivered_width, frame->sc.delivered_height,
       frame->sc.output_width, frame->sc.output_height);
#endif



    frame->stripe_height = 16 * frame->sc.output_height / frame->sc.delivered_height;
    frame->bytes_per_line = frame->sc.output_width * this->bytes_per_pixel;

    /*
     * set up colorspace converter
     */
    null_setup_colorspace_converter(frame, flags);

  }
  /*
   * reset dest pointers
   */

    if (frame->data) {
	switch (flags) {
	    case VO_TOP_FIELD:
		frame->rgb_dst    = frame->data;
		frame->stripe_inc = 2 * frame->stripe_height * frame->bytes_per_line;
		break;
	    case VO_BOTTOM_FIELD:
		frame->rgb_dst    = frame->data + frame->bytes_per_line ;
		frame->stripe_inc = 2 * frame->stripe_height * frame->bytes_per_line;
		break;
	    case VO_BOTH_FIELDS:
		frame->rgb_dst    = frame->data;
		frame->stripe_inc = frame->stripe_height * frame->bytes_per_line;
		break;
	}
    }
}

static void null_display_frame( vo_driver_t* self, vo_frame_t *frame_gen ){
  null_driver_t* this = (null_driver_t*) self;
  opie_frame_t* frame = (opie_frame_t*)frame_gen;
  display_xine_frame_t display = this->frameDis;

  if (!this->m_show_video)
    return;

  if( display != NULL ) {
    (*display)(this->userData, frame->data,
	       frame->sc.output_width, frame->sc.output_height,
	       frame->bytes_per_line );
  }

  frame->frame.free(&frame->frame);
}


/* blending related */
static void null_overlay_clut_yuv2rgb (null_driver_t  *this,
				       vo_overlay_t *overlay,
				       opie_frame_t *frame) {
  this = this;


  int i;
  clut_t* clut = (clut_t*) overlay->color;
  if (!overlay->rgb_clut) {
    for (i = 0; i < sizeof(overlay->color)/sizeof(overlay->color[0]); i++) {
      *((uint32_t *)&clut[i]) =
	  frame->yuv2rgb->
	  yuv2rgb_single_pixel_fun (frame->yuv2rgb,
				    clut[i].y, clut[i].cb,
				    clut[i].cr);
    }
    overlay->rgb_clut++;
  }
  if (!overlay->rgb_clut) {
    clut = (clut_t*) overlay->hili_color;
    for (i = 0; i < sizeof(overlay->color)/sizeof(overlay->color[0]); i++) {
      *((uint32_t *)&clut[i]) =
        frame->yuv2rgb->yuv2rgb_single_pixel_fun(frame->yuv2rgb,
						 clut[i].y, clut[i].cb, clut[i].cr);
    }
    overlay->rgb_clut++;
  }
}

static void null_overlay_blend ( vo_driver_t *this_gen, vo_frame_t *frame_gen,
				 vo_overlay_t *overlay) {
  null_driver_t  *this = (null_driver_t *) this_gen;
  opie_frame_t   *frame = (opie_frame_t *) frame_gen;

  if(!this->m_show_video || frame->sc.output_width == 0
     || frame->sc.output_height== 0)
    return;

  /* Alpha Blend here */
  if (overlay->rle) {
    if( !overlay->rgb_clut)
      null_overlay_clut_yuv2rgb(this,overlay,frame);

    switch(this->bpp) {
      case 16:
        blend_rgb16(frame->data, overlay, frame->sc.output_width,
		    frame->sc.output_height, frame->sc.delivered_width,
		    frame->sc.delivered_height, &this->alphablend_extra_data);
	break;
	case 24:
	  blend_rgb24(frame->data, overlay, frame->sc.output_width,
		      frame->sc.output_height, frame->sc.delivered_width,
		      frame->sc.delivered_height, &this->alphablend_extra_data);
	break;
	case 32:
	  blend_rgb32(frame->data, overlay, frame->sc.output_width,
		      frame->sc.output_height, frame->sc.delivered_width,
		      frame->sc.delivered_height, &this->alphablend_extra_data);
	break;
	default:
          /* It should never get here */
	  break;
    }
  }
}


static int null_get_property( vo_driver_t* self, int property ){
  null_driver_t *this = (null_driver_t *)self;

  switch(property)
  {
      case VO_PROP_ASPECT_RATIO:
	  return this->sc.user_ratio;
      case VO_PROP_BRIGHTNESS:
	  return this->yuv2rgb_brightness;
      case VO_PROP_WINDOW_WIDTH:
	  return this->sc.gui_width;
      case VO_PROP_WINDOW_HEIGHT:
	  return this->sc.gui_height;
    default:
	break;
  }

  return 0;
}
static int null_set_property( vo_driver_t* self, int property,
			      int value ){
  null_driver_t *this = (null_driver_t *)self;

  switch(property)
  {
      case VO_PROP_ASPECT_RATIO:
	  if(value>=XINE_VO_ASPECT_NUM_RATIOS)
	      value = XINE_VO_ASPECT_AUTO;
	  this->sc.user_ratio = value;
	  break;
      case VO_PROP_BRIGHTNESS:
	  this->yuv2rgb_brightness = value;
	  this->yuv2rgb_factory->
	      set_csc_levels(this->yuv2rgb_factory, value, 128, 128);
	  break;
      default:
	  break;
  }

  return value;
}
static void null_get_property_min_max( vo_driver_t* self,
				       int property, int *min,
				       int *max ){
 if(property == VO_PROP_BRIGHTNESS) {
   *min = -100;
   *max = +100;
 } else {
   *min = 0;
   *max = 0;
 }
}
static int null_gui_data_exchange( vo_driver_t* self,
				   int data_type,
				   void *data ){
  return 0;
}

static void null_dispose ( vo_driver_t* self ){
  null_driver_t* this = (null_driver_t*)self;
  _x_alphablend_free(&this->alphablend_extra_data);
  free ( this );
}
static int null_redraw_needed( vo_driver_t* self ){
    null_driver_t *this = (null_driver_t*)self;
    return _x_vo_scale_redraw_needed(&this->sc);
}


xine_video_port_t* init_video_out_plugin( xine_t *xine,
					  void* video,
					  display_xine_frame_t frameDisplayFunc,
					  void *userData, vo_scale_cb frame_cb,
					  dest_size_cb dest_cb)
{
  null_driver_t *vo;
  vo = (null_driver_t*)xine_xmalloc(sizeof(null_driver_t ));
  vo->xine = xine;

  _x_alphablend_init(&vo->alphablend_extra_data, xine);
  _x_vo_scale_init (&vo->sc, 0, 0, xine->config);
  vo->sc.gui_width  = 18;
  vo->sc.gui_height = 6;
  vo->sc.user_ratio = XINE_VO_ASPECT_AUTO;
  vo->sc.user_data  = userData;
  vo->sc.frame_output_cb = frame_cb;
  vo->sc.dest_size_cb    = dest_cb;


  vo->m_show_video = 0; // false

  /* install callback handlers*/
  vo->vo_driver.get_capabilities     = null_get_capabilities;
  vo->vo_driver.alloc_frame          = null_alloc_frame;
  vo->vo_driver.update_frame_format  = null_update_frame_format;
  vo->vo_driver.display_frame        = null_display_frame;
  vo->vo_driver.overlay_begin        = 0;
  vo->vo_driver.overlay_blend        = null_overlay_blend;
  vo->vo_driver.overlay_end          = 0;
  vo->vo_driver.get_property         = null_get_property;
  vo->vo_driver.set_property         = null_set_property;
  vo->vo_driver.get_property_min_max = null_get_property_min_max;
  vo->vo_driver.gui_data_exchange    = null_gui_data_exchange;
  vo->vo_driver.dispose              = null_dispose;
  vo->vo_driver.redraw_needed        = null_redraw_needed;

  /* capabilities */
  vo->yuv2rgb_mode       = MODE_16_RGB;
  vo->yuv2rgb_brightness = 0;
  vo->yuv2rgb_factory = yuv2rgb_factory_init(vo->yuv2rgb_mode,
					     vo->yuv2rgb_swap,
					     vo->yuv2rgb_cmap);
  vo->yuv2rgb_factory->set_csc_levels(vo->yuv2rgb_factory,
				      vo->yuv2rgb_brightness,
				      128,128);

  vo->userData = userData;
  vo->frameDis = frameDisplayFunc;

  return _x_vo_new_port(xine, &vo->vo_driver, 0);
}


/* this is special for this device */
/**
 * We know that we will be controled by the XINE LIB++
 */
int null_is_showing_video( xine_vo_driver_t* self ){
  null_driver_t* this = (null_driver_t*)self->driver;
  return this->m_show_video;
}
void null_set_show_video( xine_vo_driver_t* self, int show ) {
  ((null_driver_t*)self->driver)->m_show_video = show;
}



void null_set_mode( xine_vo_driver_t* self, int depth,  int rgb  ) {
  null_driver_t* this = (null_driver_t*)self->driver;

  this->bytes_per_pixel = (depth + 7 ) / 8;
  this->bpp = this->bytes_per_pixel * 8;
  this->depth = depth;

  switch ( this->depth ) {
  case 32:
  case 24:
    if( this->bpp == 32 ) {
      if( rgb == 0 )
        this->yuv2rgb_mode = MODE_32_RGB;
      else
        this->yuv2rgb_mode = MODE_32_BGR;

      if( rgb == 0 )
        this->yuv2rgb_mode = MODE_32_RGB;
      else
        this->yuv2rgb_mode = MODE_32_BGR;
    } else {
      if( rgb == 0 )
        this->yuv2rgb_mode = MODE_24_RGB;
      else
        this->yuv2rgb_mode = MODE_24_BGR;
    }
    break;
  case 16:
    if( rgb == 0 )
      this->yuv2rgb_mode = MODE_16_RGB;
    else
      this->yuv2rgb_mode = MODE_16_BGR;

    break;
  case 15:
    if( rgb == 0 )
      this->yuv2rgb_mode = MODE_15_RGB;
    else
      this->yuv2rgb_mode = MODE_15_BGR;

    break;
  case 8:
    if( rgb == 0 )
      this->yuv2rgb_mode = MODE_8_RGB;
    else
      this->yuv2rgb_mode = MODE_8_BGR;

    break;
  }
}

void null_display_handler( xine_vo_driver_t* self, display_xine_frame_t t,
			   void* user_data )
{
  null_driver_t* this = (null_driver_t*) self->driver;
  this->userData = user_data;
  this->frameDis = t;
}
