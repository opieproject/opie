
/*#include <xine.h>*/
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include <xine/video_out.h>
#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include <xine/configfile.h>

#include <pthread.h>
#include "alphablend.h"
#include "yuv2rgb.h"

#define printf(x,...)

/* the caller for our event draw handler */
typedef void (*display_xine_frame_t) (void *user_data, uint8_t* frame,
				      int  width, int height,int bytes );

typedef struct null_driver_s null_driver_t;

struct null_driver_s {
    vo_driver_t vo_driver;
    uint32_t m_capabilities;
    int m_show_video;
    int m_video_fullscreen;
    int m_is_scaling;
    int depth, bpp, bytes_per_pixel;
    int yuv2rgb_mode;
    int yuv2rgb_swap;
    int zuv2rgb_gamma;
    uint8_t *yuv2rgb_cmap;
    yuv2rgb_factory_t *yuv2rgb_factory;
    vo_overlay_t *overlay;
    int user_ratio;
    double output_scale_factor;
    int last_frame_output_width;
    int last_frame_output_height;
    int gui_width;
    int gui_height;
    int gui_changed;
    double display_ratio;
    void* caller;
    display_xine_frame_t frameDis;


};
typedef struct opie_frame_s opie_frame_t;
struct opie_frame_s {
    vo_frame_t frame;
    char* name;
    int version;
    int width;
    int height;
    int ratio_code;
    int format;
    int flags;
    int user_ratio;

    double ratio_factor;
    int ideal_width;
    int ideal_height;
    int output_width, output_height;
    int gui_width, gui_height;
    uint8_t *chunk[3];

    yuv2rgb_t *yuv2rgb;
    uint8_t *rgb_dst;
    int yuv_stride;
    int stripe_height, stripe_inc;
  
    int bytes_per_line;
    uint8_t *data;

//    int show_video;
    null_driver_t *output;
};

static uint32_t null_get_capabilities(vo_driver_t *self ){   
    null_driver_t* this = (null_driver_t*)self;
    printf("capabilities\n");
    return this->m_capabilities;
}

static void null_frame_copy (vo_frame_t *vo_img, uint8_t **src) {
  opie_frame_t  *frame = (opie_frame_t *) vo_img ;
  printf("frame copy\n");
  if(!frame->output->m_show_video ){ printf("no video\n"); return; } // no video

  if (frame->format == IMGFMT_YV12) {
      frame->yuv2rgb->yuv2rgb_fun (frame->yuv2rgb, frame->rgb_dst,
				   src[0], src[1], src[2]);
  } else {

      frame->yuv2rgb->yuy22rgb_fun (frame->yuv2rgb, frame->rgb_dst,
				    src[0]);
  }	
  
  frame->rgb_dst += frame->stripe_inc; 
  printf("returning\n");
}

static void null_frame_field (vo_frame_t *vo_img, int which_field) {

  opie_frame_t  *frame = (opie_frame_t *) vo_img ;
  printf("field\n\n");

  switch (which_field) {
  case VO_TOP_FIELD:
    frame->rgb_dst    = (uint8_t *)frame->data;
    frame->stripe_inc = 2*frame->stripe_height * frame->bytes_per_line;
    break;
  case VO_BOTTOM_FIELD:
    frame->rgb_dst    = (uint8_t *)frame->data + frame->bytes_per_line ;
    frame->stripe_inc = 2*frame->stripe_height * frame->bytes_per_line;
    break;
  case VO_BOTH_FIELDS:
    frame->rgb_dst    = (uint8_t *)frame->data;
    break;
  }
}


/* take care of the frame*/
static void null_frame_dispose( vo_frame_t* vo_img){    
    opie_frame_t* frame = (opie_frame_t*)vo_img;
    printf("frame_dispose\n");
    if( frame->data )
	free( frame->data );
    free (frame);
}

/* end take care of frames*/

static vo_frame_t* null_alloc_frame( vo_driver_t* self ){    
    null_driver_t* this = (null_driver_t*)self;
    opie_frame_t* frame;
    frame = (opie_frame_t*)malloc ( sizeof(opie_frame_t) );
    
    memset( frame, 0, sizeof( opie_frame_t) );
    pthread_mutex_init (&frame->frame.mutex, NULL);

    printf("alloc_frame\n");
    frame->name = "opie\0";
    frame->version = 1;
    frame->output = this;
//    frame->show_video = this->m_show_video;
    /* initialize the frame*/
    frame->frame.driver = self;
    /*frame.frame.free = null_frame_free;*/
    frame->frame.copy = null_frame_copy;
    frame->frame.field = null_frame_field;
    frame->frame.dispose = null_frame_dispose;
    frame->yuv2rgb = 0;
    /*
     *    colorspace converter for this frame
     */   
    frame->yuv2rgb = this->yuv2rgb_factory->create_converter (this->yuv2rgb_factory);
  

    return (vo_frame_t*) frame;
}

// size specific
static void null_compute_ideal_size (null_driver_t *this, opie_frame_t *frame) {

    if (!this->m_is_scaling /*|| !this->m_show_video*/) {
        printf("Not scaling\n");
	frame->ideal_width   = frame->width;
	frame->ideal_height  = frame->height;
	frame->ratio_factor  = 1.0;

  } else {
    
      double image_ratio, desired_ratio, corr_factor;

      image_ratio = (double) frame->width / (double) frame->height;

      switch (frame->user_ratio) {
	  case ASPECT_AUTO:
	      switch (frame->ratio_code) {
		  case XINE_ASPECT_RATIO_ANAMORPHIC:  /* anamorphic     */
		      desired_ratio = 16.0 /9.0;
		      break;
		  case XINE_ASPECT_RATIO_211_1:       /* 2.11:1 */
		      desired_ratio = 2.11/1.0;
		      break;
		  case XINE_ASPECT_RATIO_SQUARE:      /* square pels */
		  case XINE_ASPECT_RATIO_DONT_TOUCH:  /* probably non-mpeg stream => don't touch aspect ratio */
		      desired_ratio = image_ratio;
		      break;
		  case 0:                             /* forbidden -> 4:3 */
		      printf ("video_out_fb: invalid ratio, using 4:3\n");
		  default:
		      printf ("video_out_fb: unknown aspect ratio (%d) in stream => using 4:3\n",
			      frame->ratio_code);
		  case XINE_ASPECT_RATIO_4_3:         /* 4:3             */
		      desired_ratio = 4.0 / 3.0;
		      break;
	      }
	      break;
	  case ASPECT_ANAMORPHIC:
	      desired_ratio = 16.0 / 9.0;
	      break;
	  case ASPECT_DVB:
	      desired_ratio = 2.0 / 1.0;
	      break;
	  case ASPECT_SQUARE:
	      desired_ratio = image_ratio;
	      break;
	  case ASPECT_FULL:
	  default:
	      desired_ratio = 4.0 / 3.0;
      }
      
      frame->ratio_factor = this->display_ratio * desired_ratio;
      
      corr_factor = frame->ratio_factor / image_ratio ;

      if (fabs(corr_factor - 1.0) < 0.005) {
	  frame->ideal_width  = frame->width;
	  frame->ideal_height = frame->height;
	  
      } else {
	  
	  if (corr_factor >= 1.0) {
	      frame->ideal_width  = frame->width * corr_factor + 0.5;
	      frame->ideal_height = frame->height;
	  } else {
	      frame->ideal_width  = frame->width;
	      frame->ideal_height = frame->height / corr_factor + 0.5;
	  }
	  
      }
  }
    printf("return from helper\n");
}

static void null_compute_rgb_size (null_driver_t *this, opie_frame_t *frame) {

    double x_factor, y_factor;
    
    /*
     * make the frame fit into the given destination area
     */
  
    x_factor = (double) this->gui_width  / (double) frame->ideal_width;
    y_factor = (double) this->gui_height / (double) frame->ideal_height;
  
    if ( x_factor < y_factor ) {
	frame->output_width   = (double) frame->ideal_width  * x_factor ;
	frame->output_height  = (double) frame->ideal_height * x_factor ;
    } else {
	frame->output_width   = (double) frame->ideal_width  * y_factor ;
	frame->output_height  = (double) frame->ideal_height * y_factor ;
    }

#define LOG 1
#ifdef LOG
    printf("video_out_fb: frame source %d x %d => screen output %d x %d%s\n",
	   frame->width, frame->height,
	   frame->output_width, frame->output_height,
	   ( frame->width != frame->output_width
	     || frame->height != frame->output_height
	     ? ", software scaling"
	     : "" )
	   );
#endif
}


// size specific


static void null_update_frame_format( vo_driver_t* self, vo_frame_t* img,
				      uint32_t width, uint32_t height,
				      int ratio_code, int format, int flags ){
    null_driver_t* this = (null_driver_t*) self;
    opie_frame_t*  frame = (opie_frame_t*)img;
    /* not needed now */
    printf("update_frame_format\n");
    printf("al crash aye?\n");

    flags &= VO_BOTH_FIELDS;

    /* find out if we need to adapt this frame */

    if ((width != frame->width)
	|| (height != frame->height)
	|| (ratio_code != frame->ratio_code)
	|| (flags != frame->flags)
	|| (format != frame->format)
	|| (this->user_ratio != frame->user_ratio)
	|| (this->gui_width != frame-> gui_width )
	|| (this-> gui_height != frame-> gui_height)) {

	frame->width      = width;
	frame->height     = height;
	frame->ratio_code = ratio_code;
	frame->flags      = flags;
	frame->format     = format;
	frame->user_ratio = this->user_ratio;
	this->gui_changed = 0;
//	frame->show_video = this->m_show_video;
    frame->gui_width  = this->gui_width;
    frame->gui_height = this->gui_height;
        
	    null_compute_ideal_size (this, frame);
	    null_compute_rgb_size (this, frame);
	
	/*
	 * (re-) allocate 
	 */
	if( frame->data ) {
	    if(frame->chunk[0] ){
		free( frame->chunk[0] );
		frame->chunk[0] = NULL;	
	    }
	    if(frame->chunk[1] ){
		free ( frame->chunk[1] );
		frame->chunk[1] = NULL;
	    }	
	    if(frame->chunk[2] ){
		free ( frame->chunk[2] );
		frame->chunk[2] = NULL;
	    }
	    free ( frame->data );
	}
	printf("after freeing\n");
	frame->data = xine_xmalloc (frame->output_width * frame->output_height *
				    this->bytes_per_pixel );

	if( format == IMGFMT_YV12 ) {
	    int image_size = width * height; /* cast ouch*/
	    frame->frame.base[0] = xine_xmalloc_aligned(16, image_size,
							(void **)&frame->chunk[0] );
	    frame->frame.base[1] = xine_xmalloc_aligned(16, image_size,
							(void **)&frame->chunk[1] );
	    frame->frame.base[2] = xine_xmalloc_aligned(16, image_size,
							(void **)&frame->chunk[2] );
	}else{
	    int image_size = width * height; /* cast ouch*/
	    frame->frame.base[0] = xine_xmalloc_aligned(16, image_size,
							(void **)&frame->chunk[0] );
	    frame->chunk[1] = NULL;
	    frame->chunk[2] = NULL;
	}

	frame->format = format;
	frame->width  = width;
	frame->height = height;

	frame->stripe_height = 16 * frame->output_height / frame->height;
	frame->bytes_per_line = frame->output_width * this->bytes_per_pixel;
  
	/* 
	 * set up colorspace converter
	 */
	if(1 /*this->m_show_video*/ ){
	    printf("showing video\n");	    	

	switch (flags) {
	    case VO_TOP_FIELD:
	    case VO_BOTTOM_FIELD:
		frame->yuv2rgb->configure (frame->yuv2rgb,
					   frame->width,
					   16,	
					   frame->width*2,
					   frame->width,
					   frame->output_width,
					   frame->stripe_height,
					   frame->bytes_per_line*2);
		frame->yuv_stride = frame->bytes_per_line*2;
		break;
	    case VO_BOTH_FIELDS:
		frame->yuv2rgb->configure (frame->yuv2rgb,
					   frame->width,
					   16,
					   frame->width,
					   frame->width/2,
					   frame->output_width,
					   frame->stripe_height,
					   frame->bytes_per_line);
		frame->yuv_stride = frame->bytes_per_line;
		break;
	}
	}
    }
    printf("after gui changed\n");
    /*
     * reset dest pointers
     */

    if (frame->data) {
	switch (flags) {
	    case VO_TOP_FIELD:
		frame->rgb_dst    = (uint8_t *)frame->data;
		frame->stripe_inc = 2 * frame->stripe_height * frame->bytes_per_line;
		break;
	    case VO_BOTTOM_FIELD:
		frame->rgb_dst    = (uint8_t *)frame->data + frame->bytes_per_line ;
		frame->stripe_inc = 2 * frame->stripe_height * frame->bytes_per_line;
		break;
	    case VO_BOTH_FIELDS:
		frame->rgb_dst    = (uint8_t *)frame->data;
		frame->stripe_inc = frame->stripe_height * frame->bytes_per_line;
		break;
	}
    }    
    printf("done\n");
}
static void null_display_frame( vo_driver_t* self, vo_frame_t *frame_gen ){   
    null_driver_t* this = (null_driver_t*) self;
    opie_frame_t* frame = (opie_frame_t*)frame_gen;
    display_xine_frame_t display = this->frameDis;

    printf("display frame\n");
//       if( this->m_show_video ) { // return if not displaying	
	printf("calling home aye\n" );
	if( display != NULL ) {
	(*display)(this->caller, frame->data,
		   frame->output_width, frame->output_height,
		   frame->bytes_per_line );
	printf("display done hope you enyoyed the frame");
	}
//    }
    
    frame->frame.displayed (&frame->frame);
}


// blending related


static void null_overlay_clut_yuv2rgb(null_driver_t  *this, vo_overlay_t *overlay,
				    opie_frame_t *frame) {
  int i;
  clut_t* clut = (clut_t*) overlay->color;
  if (!overlay->rgb_clut) {
    for (i = 0; i < sizeof(overlay->color)/sizeof(overlay->color[0]); i++) {
      *((uint32_t *)&clut[i]) =
	frame->yuv2rgb->yuv2rgb_single_pixel_fun (frame->yuv2rgb,
						  clut[i].y, clut[i].cb, clut[i].cr);
    }
    overlay->rgb_clut++;
  }
  if (!overlay->clip_rgb_clut) {
    clut = (clut_t*) overlay->clip_color;
    for (i = 0; i < sizeof(overlay->color)/sizeof(overlay->color[0]); i++) {
      *((uint32_t *)&clut[i]) =
	frame->yuv2rgb->yuv2rgb_single_pixel_fun(frame->yuv2rgb,
						 clut[i].y, clut[i].cb, clut[i].cr);
    }
    overlay->clip_rgb_clut++;
  }
}

static void null_overlay_blend (vo_driver_t *this_gen, vo_frame_t *frame_gen, vo_overlay_t *overlay) {
  null_driver_t  *this = (null_driver_t *) this_gen;
  opie_frame_t   *frame = (opie_frame_t *) frame_gen;

  printf("overlay blend\n");
  if(!this->m_show_video || frame->output_width == 0 || frame->output_height== 0)
      return;

  /* Alpha Blend here */
   if (overlay->rle) {
     if( !overlay->rgb_clut || !overlay->clip_rgb_clut)
       null_overlay_clut_yuv2rgb(this,overlay,frame);

     switch(this->bpp) {
       case 16:
        blend_rgb16( (uint8_t *)frame->data, overlay,
		     frame->output_width, frame->output_height,
		     frame->width, frame->height);
        break;
       case 24:
        blend_rgb24( (uint8_t *)frame->data, overlay,
		     frame->output_width, frame->output_height,
		     frame->width, frame->height);
        break;
       case 32:
        blend_rgb32( (uint8_t *)frame->data, overlay,
		     frame->output_width, frame->output_height,
		     frame->width, frame->height);
        break;
       default:
	/* It should never get here */
	break;
     }        
   }
}


static int null_get_property( vo_driver_t* self,
			      int property ){
    printf("property get\n");			     
    return 0;
}
static int null_set_property( vo_driver_t* self,
			      int property,
			      int value ){
    printf("set property\n");			     
    return value;
}
static void null_get_property_min_max( vo_driver_t* self,
				       int property, int *min,
				       int *max ){
    printf("min max\n");				       
    *max = 0;
    *min = 0;
}
static int null_gui_data_exchange( vo_driver_t* self,
				   int data_type,
				   void *data ){
    return 0;
}
static void null_exit( vo_driver_t* self ){
    null_driver_t* this = (null_driver_t*)self;
    free ( this );
}
static int null_redraw_needed( vo_driver_t* self ){
    return 0;
}
				       

vo_driver_t* init_video_out_plugin( config_values_t* conf,
				    void* video ){
    null_driver_t *vo;
    vo = (null_driver_t*)malloc( sizeof(null_driver_t ) );
    
    /* memset? */
    memset(vo,0, sizeof(null_driver_t ) );
    vo->m_show_video = 0; // false
    vo->m_video_fullscreen = 0;
    vo->m_is_scaling = 0;
    vo->user_ratio = ASPECT_AUTO;
    vo->display_ratio = 1.0;
    vo->gui_width = 16;
    vo->gui_height = 8;
    vo->frameDis = NULL;

    /* install callback handlers*/
    vo->vo_driver.get_capabilities      = null_get_capabilities;
    vo->vo_driver.alloc_frame           = null_alloc_frame;
    vo->vo_driver.update_frame_format   = null_update_frame_format;
    vo->vo_driver.display_frame         = null_display_frame;
    vo->vo_driver.overlay_blend         = null_overlay_blend;
    vo->vo_driver.get_property          = null_get_property;
    vo->vo_driver.set_property          = null_set_property;
    vo->vo_driver.get_property_min_max  = null_get_property_min_max;
    vo->vo_driver.gui_data_exchange     = null_gui_data_exchange;
    vo->vo_driver.exit                  = null_exit;
    vo->vo_driver.redraw_needed         = null_redraw_needed;
    

    /* capabilities */
    vo->m_capabilities = VO_CAP_COPIES_IMAGE | VO_CAP_YUY2 | VO_CAP_YV12;
    vo->yuv2rgb_factory = yuv2rgb_factory_init (MODE_16_RGB, vo->yuv2rgb_swap, 
						vo->yuv2rgb_cmap);
    printf("done initialisation\n");
    return (vo_driver_t*) vo;
}

static vo_info_t vo_info_null = {
    5,
    "null plugin",
    NULL,
    VISUAL_TYPE_FB,
    5
};

vo_info_t *get_video_out_plugin_info(){
    vo_info_null.description = _("xine video output plugin using null device");
    return &vo_info_null;
}

/* this is special for this device */
/**
 * We know that we will be controled by the XINE LIB++
 */

/**
 *
 */
int null_is_showing_video( vo_driver_t* self ){
    null_driver_t* this = (null_driver_t*)self;
    return this->m_show_video;
}
void null_set_show_video( vo_driver_t* self, int show ) {
    ((null_driver_t*)self)->m_show_video = show;
}

int null_is_fullscreen( vo_driver_t* self ){
    return ((null_driver_t*)self)->m_video_fullscreen;
}
void null_set_fullscreen( vo_driver_t* self, int screen ){
    ((null_driver_t*)self)->m_video_fullscreen = screen;
}
int null_is_scaling( vo_driver_t* self ){
    return ((null_driver_t*)self)->m_is_scaling;
}
void null_set_scaling( vo_driver_t* self, int scale ){
    ((null_driver_t*)self)->m_is_scaling = scale;
}
     
void null_set_gui_width( vo_driver_t* self, int width ){
    ((null_driver_t*)self)->gui_width = width;
}
void null_set_gui_height( vo_driver_t* self, int height ){
    ((null_driver_t*)self)->gui_height = height;
}
void null_set_mode( vo_driver_t* self, int depth,  int rgb  ){
    null_driver_t* this = (null_driver_t*)self;
    
    this->bytes_per_pixel = (depth + 7 ) / 8;
    this->bpp = this->bytes_per_pixel * 8;
    this->depth = depth;
    printf("depth %d %d\n", depth, this->bpp);
    printf("pixeltype %d\n", rgb );
    switch ( this->depth ){
	case 32:
	    if( rgb == 0 )
		this->yuv2rgb_mode = MODE_32_RGB;
	    else
		this->yuv2rgb_mode = MODE_32_BGR;
	case 24:
	    if( this->bpp == 32 ) {
		if(rgb == 0 ) 
		    this->yuv2rgb_mode = MODE_32_RGB;
		else
		    this->yuv2rgb_mode = MODE_32_BGR;
	    }else{
		if( rgb == 0 )
		    this->yuv2rgb_mode = MODE_24_RGB;
		else
		    this->yuv2rgb_mode = MODE_24_BGR;
	    };
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
    };
    //free(this->yuv2rgb_factory );
    // this->yuv2rgb_factory = yuv2rgb_factory_init (this->yuv2rgb_mode, this->yuv2rgb_swap, 
    //						this->yuv2rgb_cmap);
};
void null_display_handler(vo_driver_t* self, display_xine_frame_t t, void* user_data) {
    null_driver_t* this = (null_driver_t*) self;
    this->caller = user_data;
    this->frameDis = t;
}
