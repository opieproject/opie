#include <stdio.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <GUIUtils.h>
#include "PPPRunedit.h"

PPPRunEdit::PPPRunEdit( QWidget * Parent ) : PPPRunGUI( Parent ){
}

QString PPPRunEdit::acceptable( void ) {
    return QString();
}

bool PPPRunEdit::commit( PPPData & D ) {
    bool SM = 0;

    TXTM( D.Run.PreConnect, PreConnect_LE, SM );
    TXTM( D.Run.PostConnect, PostConnect_LE, SM );
    TXTM( D.Run.PreDisconnect, PreDisconnect_LE, SM );
    TXTM( D.Run.PostDisconnect, PostDisconnect_LE, SM );
    return SM;
}

void PPPRunEdit::showData( PPPData & D ) {
    STXT( D.Run.PreConnect, PreConnect_LE);
    STXT( D.Run.PostConnect, PostConnect_LE);
    STXT( D.Run.PreDisconnect, PreDisconnect_LE );
    STXT( D.Run.PostDisconnect, PostDisconnect_LE);
}
