#ifndef SENDER_UI_H
#define SENDER_UI_H

#include <qcstring.h>

#include "sender.h"

class IOSerial;
class FileTransfer;
class QSocketNotifier;
namespace Opie {namespace Core {class Opie::Core::OProcess;}}
class SenderUI : public Sender {
    Q_OBJECT
public:
    SenderUI();
    ~SenderUI();

public slots:
    void send();
    void slotSendFile();
    void slotSend();
    void slotRev();
    void got(const QByteArray& );
    void fileTransComplete();
private:
    IOSerial* ser;
    FileTransfer* sz;
    int m_fd;
    QSocketNotifier* m_sock;
    Opie::Core::OProcess* m_proc;
};


#endif
