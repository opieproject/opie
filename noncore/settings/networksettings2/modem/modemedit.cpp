#include <GUIUtils.h>
#include "modemedit.h"

ModemEdit::ModemEdit( QWidget * Parent ) : ModemGUI( Parent ){

}

QString ModemEdit::acceptable( void ) {
    return QString();
}

bool ModemEdit::commit( ModemData_t & Data ) {
    return 0;
}

void ModemEdit::showData( ModemData_t & Data ) {
}

