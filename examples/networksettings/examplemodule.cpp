#include "exampleiface.h"
#include "examplemodule.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

#include <interfaces/interfaceinformationimp.h>

#include <qwidget.h>

VirtualModule::VirtualModule() {
    Interface* iface = new VirtualInterface( 0 );
    iface->setHardwareName( "vpn" );
    iface->setInterfaceName( "Test VPN" );
    m_interfaces.append( iface );

//  If we set up VPN via pptp
//  and networksettins was closed and now opened
//  we need to hide the ppp device behind us
//  One can do  this by calling setHandledInterfaceNames
//    setHandledInterfaceNames();
}

VirtualModule::~VirtualModule() {
    m_interfaces.setAutoDelete( true );
    m_interfaces.clear();
}


/*
 * We're a VPN module
 */
bool VirtualModule::isOwner(  Interface* iface ) {
 /* check if it is our device */
    return  m_interfaces.find(  iface ) != -1;
}

QWidget*  VirtualModule::configure( Interface* ) {
/* We don't have any Config for now */
    return 0l;
}

QWidget* VirtualModule::information(  Interface* iface ) {
    return  new InterfaceInformationImp(0, "Interface info", iface );
}

QList<Interface> VirtualModule::getInterfaces() {
    return m_interfaces;
}

void VirtualModule::possibleNewInterfaces( QMap<QString, QString>& map) {
    map.insert( QObject::tr("VPN PPTP"),
                QObject::tr("Add new Point to Point Tunnel Protocol connection" ) );
}


Interface* VirtualModule::addNewInterface( const QString& ) {
 /* check the str if we support more interfaces */
/*
    Interface* iface = new VirtualInterface( 0 );
    iface->setModuleOwner( this );
    return iface;*/

// if we would support saving interfaces we could add
// them here

    return 0;
}


bool VirtualModule::remove( Interface* ) {
/* we do not support removing our interface */
    return false;
}
