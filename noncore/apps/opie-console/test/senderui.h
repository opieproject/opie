#ifndef SENDER_UI_H
#define SENDER_UI_H

#include <qcstring.h>

#include "sender.h"

class IOSerial;
class FileTransfer;
class QSocketNotifier;
class OProcess;
class SenderUI : public Sender {
    Q_OBJECT
public:
    SenderUI();
    ~SenderUI();

public slots:
    void send();
    void slotSendFile();
    void slotSend();
    void got(const QByteArray& );
    void fileTransComplete();
private:
    IOSerial* ser;
    FileTransfer* sz;
    int m_fd;
    QSocketNotifier* m_sock;
    OProcess* m_proc;
};


#endif
