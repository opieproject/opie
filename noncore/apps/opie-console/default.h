#ifndef OPIE_DEFAULT_H
#define OPIE_DEFAULT_H

#include "metafactory.h"

extern "C" {
    FileTransferLayer* newSZTransfer(IOLayer*);
    FileTransferLayer* newSYTransfer(IOLayer*);
    FileTransferLayer* newSXTransfer(IOLayer*);

    IOLayer* newSerialLayer(const Profile&);
    IOLayer* newBTLayer(const Profile& );
    IOLayer* newIrDaLayer(const Profile& );

    ProfileDialogWidget* newSerialWidget(const QString&, QWidget* );
    ProfileDialogWidget* newIrDaWidget  (const QString&, QWidget* );
    ProfileDialogWidget* newBTWidget    (const QString&, QWidget* );

    ProfileDialogWidget* newTerminalWidget(const QString&, QWidget* );
};

class MetaFactory;
struct Default {
public:
    Default(MetaFactory* );
    ~Default();
};


#endif
