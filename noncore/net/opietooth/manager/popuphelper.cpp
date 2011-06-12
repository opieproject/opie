
#include "stdpopups.h"
#include "popuphelper.h"

#include <opie2/obluetoothdevicehandler.h>

using namespace OpieTooth;
using namespace Opie::Bluez;

PopupHelper::PopupHelper( DeviceHandlerPool *devHandlerPool )
{
    m_devHandlerPool = devHandlerPool;
    init();
}

PopupHelper::~PopupHelper() {
}

void PopupHelper::insert( int id, popupFactory fact ) {
    m_map.insert(id, fact );
}

QPopupMenu* PopupHelper::find( int id, const Opie::Bluez::OBluetoothServices& ser, BTDeviceItem* item ) {
    //owarn << "find" << oendl;
    FactoryMap::Iterator it = m_map.find(id );
    if ( it != m_map.end() ) {
        //owarn << "found" << oendl;
        popupFactory fact = it.data();
        return (*fact)(ser, item, m_devHandlerPool);
    }
    return 0l;
}

void PopupHelper::init() {
    insert( 0x1101, newRfCommPopup );   // SerialPort
    insert( 0x1102, newDunPopup );      // LanAccessUsingPPP
    insert( 0x1103, newRfCommPopup );   // DialupNetworking
    insert( 0x1105, newObexPushPopup ); // OBEXObjectPush
    insert( 0x1106, newObexFtpPopup );  // OBEXFileTransfer
    insert( 0x1116, newPanPopup );      // NAP
    insert( 0x1117, newPanPopup );      // GN
    insert( 0x1124, newHidPopup );      // HumanInterfaceDeviceService
}
