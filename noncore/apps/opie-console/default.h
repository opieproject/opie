#ifndef OPIE_DEFAULT_H
#define OPIE_DEFAULT_H

#include "metafactory.h"

class Widget;

extern "C" {
    FileTransferLayer* newSZTransfer(IOLayer*);
    FileTransferLayer* newSYTransfer(IOLayer*);
    FileTransferLayer* newSXTransfer(IOLayer*);

    ReceiveLayer* newSZReceive(IOLayer*);
    ReceiveLayer* newSYReceive(IOLayer*);
    ReceiveLayer* newSXReceive(IOLayer*);

    IOLayer* newSerialLayer(const Profile&);
    IOLayer* newBTLayer(const Profile& );
    IOLayer* newIrDaLayer(const Profile& );
    IOLayer* newConsole(const Profile& );

    ProfileDialogWidget* newSerialWidget(const QString&, QWidget* );
    ProfileDialogWidget* newIrDaWidget  (const QString&, QWidget* );
    ProfileDialogWidget* newBTWidget    (const QString&, QWidget* );
    ProfileDialogWidget* newConsoleWid     (const QString&, QWidget* );

    ProfileDialogWidget* newTerminalWidget(const QString&, QWidget* );

//    EmulationLayer* newVT102( WidgetLayer* );
};

class MetaFactory;
struct Default {
public:
    Default(MetaFactory* );
    ~Default();
};


#endif
