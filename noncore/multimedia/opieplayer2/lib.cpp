
#include <stdio.h>
#include <stdlib.h>
//#include <qpe/qpeapplication.h>
#include <qimage.h>
#include <qpe/resource.h>

#include <qfile.h>

#include <qgfx_qws.h>
#include <qdirectpainter_qws.h>

#include "xinevideowidget.h"
#include "frame.h"
#include "lib.h"

typedef void (*display_xine_frame_t) (void *user_data, uint8_t* frame,
				      int  width, int height,int bytes );

extern "C" {
    vo_driver_t* init_video_out_plugin( config_values_t* conf, void* video);
    int null_is_showing_video( vo_driver_t* self );
    void null_set_show_video( vo_driver_t* self, int show );
    int null_is_fullscreen( vo_driver_t* self );
    void null_set_fullscreen( vo_driver_t* self, int screen );
    int null_is_scaling( vo_driver_t* self );
    void null_set_scaling( vo_driver_t* self, int scale );
    void null_set_gui_width( vo_driver_t* self, int width );
    void null_set_gui_height( vo_driver_t* self, int height );
    void null_set_mode( vo_driver_t* self, int depth,  int rgb  );
    void null_display_handler(vo_driver_t* self, display_xine_frame_t t, void* user_data);
}

using namespace XINE;

Lib::Lib(XineVideoWidget* widget) {
    m_video = false;
    m_wid = widget;
    printf("Lib");
    QCString str( getenv("HOME") );
    str += "/Settings/opiexine.cf";
    // get the configuration
    m_config = xine_config_file_init( str.data() );

    // allocate oss for sound
    // and fb for framebuffer
    m_audioOutput= xine_load_audio_output_plugin( m_config, "oss") ;
    if (m_audioOutput == NULL )
        printf("Failure\n");
    else
        printf("Success\n");


    // test code
/*    m_videoOutput = xine_load_video_output_plugin(m_config, "fb",
                                                  VISUAL_TYPE_FB,
                                                  0 );
*/

    char** files = xine_list_video_output_plugins(3);
    char* out;
    int i = 0;
    while ( ( out = files[i] ) != 0 ) {
        printf("Video %s\n", out );
        i++;
    }
//    m_xine = xine_init( m_videoOutput,
//                      m_audioOutput,
//                      m_config );
    // test loading
    m_videoOutput = ::init_video_out_plugin( m_config, NULL );
    if (m_wid != 0 ) {
        printf("!0\n" );
        ::null_set_gui_width( m_videoOutput,  m_wid->image()->width() );
        ::null_set_gui_height(m_videoOutput,  m_wid->image()->height() );
        ::null_set_mode( m_videoOutput, qt_screen->depth(), qt_screen->pixelType() );
        m_bytes_per_pixel = ( qt_screen->depth() + 7 ) / 8;
        QImage image = Resource::loadImage("SoundPlayer");
        image = image.smoothScale( m_wid->width(), m_wid->height() );
        QImage* img = new QImage( image );
        m_wid->setImage( img );
        m_wid->repaint();
    }
    null_display_handler( m_videoOutput,
                          xine_display_frame,
                          this );

    m_xine = xine_init( m_videoOutput,
			m_audioOutput, m_config );
    // install the event handler
    xine_register_event_listener( m_xine, xine_event_handler, this );
}

Lib::~Lib() {
    delete m_config;
    xine_remove_event_listener( m_xine, xine_event_handler );
    xine_exit( m_xine );
    delete m_videoOutput;
    //delete m_audioOutput;

}

QCString Lib::version() {
    QCString str( xine_get_str_version() );
    return str;
};

int Lib::majorVersion() {
    return xine_get_major_version();
}
int Lib::minorVersion() {
    return xine_get_minor_version();
};

int Lib::subVersion() {
    return xine_get_sub_version();
}
int Lib::play( const QString& fileName,
               int startPos,
               int start_time ) {
    QString str = fileName.stripWhiteSpace();
    //workaround OpiePlayer bug
    if (str.right(1) == QString::fromLatin1("/") )
        str = str.mid( str.length() -1 );
    return xine_play( m_xine, QFile::encodeName(str.utf8() ).data(),
               startPos, start_time);
}
void Lib::stop() {
    xine_stop(m_xine );
}
void Lib::pause(){
    xine_set_speed( m_xine, SPEED_PAUSE );
}
int Lib::speed() {
    return xine_get_speed( m_xine );
}
void Lib::setSpeed( int speed ) {
    xine_set_speed( m_xine, speed );
}
int Lib::status(){
    return xine_get_status( m_xine );
}
int Lib::currentPosition(){
    return xine_get_current_position( m_xine );
}
int Lib::currentTime()  {
    return xine_get_current_time( m_xine );
};
int Lib::length() {
    return xine_get_stream_length( m_xine );
}
bool Lib::isSeekable() {
    return xine_is_stream_seekable(m_xine);
}
Frame Lib::currentFrame() {
    Frame frame;
    return frame;
};
int Lib::error() {
    return xine_get_error( m_xine );
};
void Lib::handleXineEvent( xine_event_t* t ) {
    if ( t->type == XINE_EVENT_PLAYBACK_FINISHED )
        emit stopped();
}
void Lib::setShowVideo( bool video ) {
    m_video = video;
    ::null_set_show_video( m_videoOutput,  video );
}
bool Lib::isShowingVideo() {
    return ::null_is_showing_video( m_videoOutput );
}
void Lib::showVideoFullScreen( bool fullScreen ) {
    ::null_set_fullscreen( m_videoOutput, fullScreen );
}
bool Lib::isVideoFullScreen() {
    return ::null_is_fullscreen( m_videoOutput );
}
void Lib::setScaling( bool scale ) {
    ::null_set_scaling( m_videoOutput, scale  );
}
bool Lib::isScaling() {
    return ::null_is_scaling( m_videoOutput );
}
void Lib::xine_event_handler( void* user_data, xine_event_t* t ) {
    ((Lib*)user_data)->handleXineEvent( t );
}
void Lib::xine_display_frame( void* user_data, uint8_t *frame,
                              int width,  int height,  int bytes ) {
//    printf("display x frame");
    ((Lib*)user_data)->drawFrame( frame,  width, height,  bytes );
//    printf("displayed x frame\n");
}
void Lib::drawFrame( uint8_t* frame,  int width,  int height,  int bytes ) {
    if (!m_video ) {
        qWarning("not showing video now");
        return;
    }
//    qWarning("called draw frame %d %d",  width,  height);

    QSize size = m_wid->size();
    int xoffset  = (size.width() - width) / 2;
    int yoffset  = (size.height() - height) / 2;
    int linestep = qt_screen->linestep();

    m_wid->setImage( frame, yoffset, xoffset, width, height, linestep, bytes, m_bytes_per_pixel );
//    m_wid->repaint(false);

}
