#include <GUIUtils.h>
#include "wlanedit.h"

WLanEdit::WLanEdit( QWidget * Parent ) : WLanGUI( Parent ){
}

QString WLanEdit::acceptable( void ) {
    return QString();
}

void WLanEdit::showData( WLanData_t & Data ) {
}

bool WLanEdit::commit( WLanData_t & Data ) {
    return 0;
}
