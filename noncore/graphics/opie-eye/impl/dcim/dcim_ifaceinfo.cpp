/*
 * GPLv2
 * zecke@handhelds.org
 */


#include "dcim_ifaceinfo.h"

#include <qobject.h>

DCIM_InterfaceInfo::DCIM_InterfaceInfo()  {}
DCIM_InterfaceInfo::~DCIM_InterfaceInfo() {}

QString DCIM_InterfaceInfo::name()const {
    return QObject::tr( "Digital Camera View" );
}

QWidget* DCIM_InterfaceInfo::configWidget( const Config& ) {
    return 0l;
}

void DCIM_InterfaceInfo::writeConfig( QWidget*, Config& ) {}
