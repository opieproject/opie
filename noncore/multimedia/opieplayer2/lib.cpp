
#include <stdio.h>
#include <stdlib.h>
//#include <qpe/qpeapplication.h>

#include <qfile.h>

#include "frame.h"
#include "xinelib.h"




using namespace XINE;

Lib::Lib() {
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
    m_videoOutput = xine_load_video_output_plugin(m_config, "fb",
                                                  VISUAL_TYPE_FB,
                                                  0 );

    char** files = xine_list_video_output_plugins(3);
    char* out;
    int i = 0;
    while ( ( out = files[i] ) != 0 ) {
        printf("Audio %s\n", out );
        i++;
    }
    m_xine = xine_init( m_videoOutput,
                      m_audioOutput,
                      m_config );
}

Lib::~Lib() {
    delete m_config;
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
    QString str = fileName;
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

