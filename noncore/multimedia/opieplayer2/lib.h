

#ifndef ZECKEXINELIB_H
#define ZECKEXINELIB_H

#include <qcstring.h>
#include <qstring.h>
#include <xine.h>

namespace XINE {

    /**
     * Lib wrapps the simple interface
     * of libxine for easy every day use
     * This will become a full C++ Wrapper
     * It supports playing, pausing, info,
     * stooping, seeking.
     */
    class Frame;
    class Lib {
    public:
        Lib();
        ~Lib();
        QCString version();
        int majorVersion()/*const*/;
        int minorVersion()/*const*/;
        int subVersion()/*const*/;


        int play( const QString& fileName,
                  int startPos = 0,
                  int start_time = 0 );
        void stop() /*const*/;
        void pause()/*const*/;

        int speed() /*const*/;
        void setSpeed( int speed = SPEED_PAUSE );

        int status() /*const*/;

        int currentPosition()/*const*/;
        //in seconds
        int currentTime()/*const*/;
        int length() /*const*/;

        bool isSeekable()/*const*/;

        Frame currentFrame()/*const*/;
        int error() /*const*/;
    private:
        xine_t *m_xine;
        config_values_t *m_config;
        vo_driver_t *m_videoOutput;
        ao_driver_t* m_audioOutput;

    };
};


#endif
