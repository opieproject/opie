#include "io_serial.h"
#include "sz_transfer.h"
#include "serialconfigwidget.h"
#include "irdaconfigwidget.h"
#include "btconfigwidget.h"
#include "terminalwidget.h"
#include "vt102emulation.h"

#include "default.h"

extern "C" {
    // FILE Transfer Stuff
    FileTransferLayer* newSZTransfer(IOLayer* lay) {
        return new SzTransfer( SzTransfer::SZ, lay );
    }
    FileTransferLayer* newSYTransfer(IOLayer* lay) {
        return new SzTransfer( SzTransfer::SY, lay );
    }
    FileTransferLayer* newSXTransfer(IOLayer* lay) {
        return new SzTransfer( SzTransfer::SX, lay );
    }

    // Layer stuff
    IOLayer* newSerialLayer( const Profile& prof) {
        return new IOSerial( prof );
    }
    IOLayer* newBTLayer( const Profile&  ) {
        return 0l;
    }
    IOLayer* newIrDaLayer( const Profile& ) {
        return 0l;
    }

    // Connection Widgets
    ProfileDialogWidget* newSerialWidget( const QString& str, QWidget* wid ) {
        return new SerialConfigWidget( str, wid );
    }
    ProfileDialogWidget* newIrDaWidget( const QString& str, QWidget* wid ) {
        return new IrdaConfigWidget( str, wid );
    }
    ProfileDialogWidget* newBTWidget( const QString& str, QWidget* wid ) {
        return new BTConfigWidget(str, wid );
    }

    // Terminal Widget(s)
    ProfileDialogWidget* newTerminalWidget(const QString& na, QWidget* wid) {
        return new TerminalWidget(na, wid,0 );
    }

    // VT Emulations
    EmulationLayer* newVT102( Widget* wid ) {
        return new Vt102Emulation( wid );
    }

};

Default::Default( MetaFactory* fact ) {
    fact->addFileTransferLayer( "SZ", QObject::tr("Z-Modem"), newSZTransfer );
    fact->addFileTransferLayer( "SY", QObject::tr("Y-Modem"), newSYTransfer );
    fact->addFileTransferLayer( "SX", QObject::tr("X-Modem"), newSXTransfer );

    fact->addIOLayerFactory( "serial", QObject::tr("Serial"), newSerialLayer );
    fact->addIOLayerFactory( "irda", QObject::tr("Infrared"), newIrDaLayer   );
    fact->addIOLayerFactory( "bt", QObject::tr("Bluetooth"),  newBTLayer     );

    fact->addConnectionWidgetFactory( "serial", QObject::tr("Serial"), newSerialWidget );
    fact->addConnectionWidgetFactory( "irda", QObject::tr("Infrared"), newIrDaWidget );
    fact->addConnectionWidgetFactory( "bt", QObject::tr("Bluetooth"), newBTWidget );

    fact->addTerminalWidgetFactory( "default", QObject::tr("Default Terminal"),  newTerminalWidget );

    fact->addEmulationLayer( "default", QObject::tr("Default Terminal"), newVT102 );
}
Default::~Default() {

}
