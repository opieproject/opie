#include <GUIUtils.h>
#include "usbedit.h"

USBEdit::USBEdit( QWidget * Parent ) : USBGUI( Parent ){

}

QString USBEdit::acceptable( void ) {
    return QString();
}

void USBEdit::showData( USBData & Data ) {
}

bool USBEdit::commit( USBData & Data ) {
    return 1;
}
