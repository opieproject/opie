#ifndef OPIE_QWINDOWSYSTEM_H
#define OPIE_QWINDOWSYSTEM_H

#include <qimage.h>

class QWSScreenSaver;
class QWSServer {
public:
QWSServer(){}
~QWSServer(){}
    static void sendKeyEvent(int, int, int, bool, bool ) { }
   struct KeyboardFilter {
   };
    static void setKeyboardFilter( KeyboardFilter* ) {

    }
    static void setScreenSaver( QWSScreenSaver* ) {

    }
    static void setScreenSaverInterval( int ) {

    }
    static void setScreenSaverIntervals( int[] ) {

    }
    static void screenSaverActivate( bool ) {

    }
    static void setDesktopBackground( const QImage& ) {

    }

};
struct QWSScreenSaver {
};

#endif
