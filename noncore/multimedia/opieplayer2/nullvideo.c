
/*#include <xine.h>*/
#include <stdlib.h>
#include <stdio.h>

#include <xine/video_out.h>
#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include <xine/configfile.h>

typedef struct null_driver_s null_driver_t;

struct null_driver_s {
    vo_driver_t vo_driver;
    uint32_t m_capabilities;

};
typedef struct opie_frame_s opie_frame_t;
struct opie_frame_s {
    vo_frame_t frame;
    char* name;
    int version;
    null_driver_t *output;
};

static uint32_t null_get_capabilities(vo_driver_t *self ){
    null_driver_t* this = (null_driver_t*)self;
    return this->m_capabilities;
}

/* take care of the frame*/
static void null_frame_dispose( vo_frame_t* vo_img){
    opie_frame_t* frame = (opie_frame_t*)vo_img;
    free (frame);
}
static void null_frame_field( vo_frame_t* frame, int inti ){
    /* not needed */
}

/* end take care of frames*/

static vo_frame_t* null_alloc_frame( vo_driver_t* self ){
    null_driver_t* this = (null_driver_t*)self;
    opie_frame_t* frame = (opie_frame_t*)malloc ( sizeof(opie_frame_t) );
    memset( frame, 0, sizeof( opie_frame_t) );
    frame->name = "opie\0";
    frame->version = 1;
    frame->output = this;

    /* initialize the frame*/
    frame->frame.driver = self;
    /*frame.frame.free = null_frame_free;*/
    frame->frame.copy = NULL;
    frame->frame.field = null_frame_field;
    frame->frame.dispose = null_frame_dispose;
 
    return (vo_frame_t*) frame;
}
static void null_update_frame_format( vo_driver_t* self, vo_frame_t* img,
				      uint32_t width, uint32_t height,
				      int ratio_code, int format, int flags ){
    /* not needed now */
}
static void null_display_frame( vo_driver_t* self, vo_frame_t *frame ){
    printf("display frame");
}
static void null_overlay_blend( vo_driver_t* self, vo_frame_t* frame,
			       vo_overlay_t* overlay ){
    /* sure */
}
static int null_get_property( vo_driver_t* self,
			      int property ){
    return 0;
}
static int null_set_property( vo_driver_t* self,
			      int property,
			      int value ){
    return value;
}
static void null_get_property_min_max( vo_driver_t* self,
				       int property, int *min,
				       int *max ){
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

    /* install callback handlers*/
    vo->vo_driver.get_capabilities = null_get_capabilities;
    vo->vo_driver.alloc_frame = null_alloc_frame;
    vo->vo_driver.update_frame_format = null_update_frame_format;
    vo->vo_driver.display_frame = null_display_frame;
    vo->vo_driver.overlay_blend = null_overlay_blend;
    vo->vo_driver.get_property = null_get_property;
    vo->vo_driver.set_property = null_set_property;
    vo->vo_driver.get_property_min_max = null_get_property_min_max;
    vo->vo_driver.gui_data_exchange = null_gui_data_exchange;
    vo->vo_driver.exit = null_exit;
    vo->vo_driver.redraw_needed = null_redraw_needed;
    

    /* capabilities */
    vo->m_capabilities = /*VO_CAP_COPIES_IMAGE |*/ VO_CAP_YV12 | VO_CAP_BRIGHTNESS;
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
