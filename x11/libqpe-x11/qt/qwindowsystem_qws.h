#ifndef OPIE_QWINDOWSYSTEM_H
#define OPIE_QWINDOWSYSTEM_H

class QWSServer {
public:
QWSServer(){}
~QWSServer(){}
    static void sendKeyEvent(int, int, int, bool, bool ) { }
   struct KeyboardFilter {
   };
    static void setKeyboardFilter( KeyboardFilter* ) {

    }

};


#endif
