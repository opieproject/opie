#ifndef SENDER_UI_H
#define SENDER_UI_H

#include <qcstring.h>

#include "sender.h"

class IOSerial;
class SzTransfer;
class SenderUI : public Sender {
    Q_OBJECT
public:
    SenderUI();
    ~SenderUI();

public slots:
    void slotSendFile();
    void slotSend();
    void got(const QByteArray& );
    void fileTransComplete();
private:
    IOSerial* ser;
    SzTransfer* sz;
};


#endif
