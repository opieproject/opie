#include <stdio.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <GUIUtils.h>
#include "PPPDialingedit.h"

PPPDialingEdit::PPPDialingEdit( QWidget * Parent ) : 
                              PPPDialingGUI( Parent ){

      // populate widget stack
}

QString PPPDialingEdit::acceptable( void ) {
    return QString();
}

bool PPPDialingEdit::commit( PPPData & D ) {
    bool SM;
    return SM;
}

void PPPDialingEdit::showData( PPPData & D ) {
}
