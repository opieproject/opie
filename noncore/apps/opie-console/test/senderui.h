#ifndef SENDER_UI_H
#define SENDER_UI_H

#include <qcstring.h>

#include "sender.h"

class IOSerial;
class SenderUI : public Sender {
    Q_OBJECT
public:
    SenderUI();
    ~SenderUI();

public slots:
    void slotSend();
    void got(const QByteArray& );
private:
    IOSerial* ser;
};


#endif
