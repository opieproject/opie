#include <GUIUtils.h>
#include "usbedit.h"

USBEdit::USBEdit( QWidget * Parent ) : USBGUI( Parent ){

}

QString USBEdit::acceptable( void ) {
    return QString();
}

void USBEdit::showData( USBData_t & Data ) {
}

bool USBEdit::commit( USBData_t & Data ) {
    return 1;
}
