#include <GUIUtils.h>
#include "irdaedit.h"

IRDAEdit::IRDAEdit( QWidget * Parent ) : IRDAGUI( Parent ){

}

QString IRDAEdit::acceptable( void ) {
    return QString();
}

bool IRDAEdit::commit( IRDAData & Data ) {
    return 0;
}

void IRDAEdit::showData( IRDAData & Data ) {
}
