#include <GUIUtils.h>
#include "vpnedit.h"


VPNEdit::VPNEdit( QWidget * Parent ) : VPNGUI( Parent ){

}

QString VPNEdit::acceptable( void ) {
    return QString();
}

void VPNEdit::showData( VPNData & Data ) {
}

bool VPNEdit::commit( VPNData & Data ) {
    return 0;
}
