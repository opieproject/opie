#include <GUIUtils.h>
#include "vpnedit.h"


VPNEdit::VPNEdit( QWidget * Parent ) : VPNGUI( Parent ){

}

QString VPNEdit::acceptable( void ) {
    return QString();
}

void VPNEdit::showData( VPNData_t & Data ) {
}

bool VPNEdit::commit( VPNData_t & Data ) {
    return 0;
}
