#include "io_serial.h"
#include "io_irda.h"
#include "io_bt.h"
#include "io_modem.h"
#include "filetransfer.h"
#include "filereceive.h"
#include "serialconfigwidget.h"
#include "irdaconfigwidget.h"
#include "btconfigwidget.h"
#include "modemconfigwidget.h"
#include "terminalwidget.h"
#include "function_keyboard.h"
#include "MyPty.h"

#include "default.h"

extern "C" {
    // FILE Transfer Stuff
    FileTransferLayer* newSZTransfer(IOLayer* lay) {
        return new FileTransfer( FileTransfer::SZ, lay );
    }
    FileTransferLayer* newSYTransfer(IOLayer* lay) {
        return new FileTransfer( FileTransfer::SY, lay );
    }
    FileTransferLayer* newSXTransfer(IOLayer* lay) {
        return new FileTransfer(FileTransfer ::SX, lay );
    }

    // FILE Transfer Receive Stuff
    ReceiveLayer* newSZReceive(IOLayer* lay) {
        return new FileReceive( FileReceive::SZ, lay );
    }
    ReceiveLayer* newSYReceive(IOLayer* lay) {
        return new FileReceive( FileReceive::SY, lay );
    }
    ReceiveLayer* newSXReceive(IOLayer* lay) {
        return new FileReceive(FileReceive::SX, lay );
    }

    // Layer stuff
    IOLayer* newSerialLayer( const Profile& prof) {
        return new IOSerial( prof );
    }
    IOLayer* newBTLayer( const Profile& prof ) {
        return new IOBt( prof );
    }
    IOLayer* newIrDaLayer( const Profile& prof ) {
        return new IOIrda( prof );
    }
    IOLayer* newModemLayer( const Profile& prof ) {
        return new IOModem( prof );
    }
    IOLayer* newConsole( const Profile& prof ) {
        return new MyPty( prof );
    }

    // Connection Widgets
    ProfileDialogWidget* newSerialWidget( const QString& str, QWidget* wid ) {
        return new SerialConfigWidget( str, wid );
    }
    ProfileDialogWidget* newIrDaWidget( const QString& str, QWidget* wid ) {
        return new IrdaConfigWidget( str, wid );
    }
    ProfileDialogWidget* newModemWidget( const QString& str, QWidget* wid ) {
        return new ModemConfigWidget(str, wid );
    }
    ProfileDialogWidget* newBTWidget( const QString& str, QWidget* wid ) {
        return new BTConfigWidget(str, wid );
    }
    ProfileDialogWidget* newConsoleWid( const QString& , QWidget*  ) {
        return 0l;
    }


    // Terminal Widget(s)
    ProfileDialogWidget* newTerminalWidget(const QString& na, QWidget* wid) {
        return new TerminalWidget(na, wid,0 );
    }

    // Function Keyboard Widget
    ProfileDialogWidget* newKeyboardWidget(const QString& na, QWidget *wid) {
        return new FunctionKeyboardConfig(na, wid);
    }

/*    // VT Emulations
    EmulationLayer* newVT102( WidgetLayer* wid ) {
        return new Vt102Emulation( wid );
    }
*/
};

Default::Default( MetaFactory* fact ) {
    fact->addFileTransferLayer( "SZ", QObject::tr("Z-Modem"), newSZTransfer );
    fact->addFileTransferLayer( "SY", QObject::tr("Y-Modem"), newSYTransfer );
    fact->addFileTransferLayer( "SX", QObject::tr("X-Modem"), newSXTransfer );

    fact->addReceiveLayer( "SZ", QObject::tr("Z-Modem"), newSZReceive );
    fact->addReceiveLayer( "SY", QObject::tr("Y-Modem"), newSYReceive );
    fact->addReceiveLayer( "SX", QObject::tr("X-Modem"), newSXReceive );

    fact->addIOLayerFactory( "serial", QObject::tr("Serial"), newSerialLayer );
    fact->addIOLayerFactory( "irda", QObject::tr("Infrared"), newIrDaLayer   );
    fact->addIOLayerFactory( "bt", QObject::tr("Bluetooth"),  newBTLayer     );
    fact->addIOLayerFactory( "modem", QObject::tr("Modem"),   newModemLayer  );
    fact->addIOLayerFactory( "console", QObject::tr("Console"), newConsole   );

    fact->addConnectionWidgetFactory( "serial", QObject::tr("Serial"), newSerialWidget );
    fact->addConnectionWidgetFactory( "irda", QObject::tr("Infrared"), newIrDaWidget );
    fact->addConnectionWidgetFactory( "modem", QObject::tr("Modem"), newModemWidget );
    fact->addConnectionWidgetFactory( "bt", QObject::tr("Bluetooth"), newBTWidget );
    fact->addConnectionWidgetFactory( "console", QObject::tr("Console"), newConsoleWid );

    fact->addTerminalWidgetFactory( "default", QObject::tr("Default Terminal"),  newTerminalWidget );
    fact->addKeyboardWidgetFactory( "defaultKeys", QObject::tr("Default Keyboard"),
 newKeyboardWidget );

//    fact->addEmulationLayer( "default", QObject::tr("Default Terminal"), newVT102 );
}
Default::~Default() {

}
