#ifndef DCCTRANSFER_H
#define DCCTRANSFER_H

#include <qobject.h>

class QSocket;
class QFile;
class QString;

class DCCTransfer: public QObject {
    Q_OBJECT
public:
    enum Type { Send, Recv };
    enum EndCode { Successfull, SelfAborted, PeerAborted, Timeout };

    DCCTransfer(Q_UINT32 ip4Addr, Q_UINT16 port, const QString &filename, unsigned int size);
    virtual ~DCCTransfer();

    void cancel();

    QString filename();

signals:
    virtual void finished(DCCTransfer *transfer, EndCode code);
    virtual void progress(int progress);

protected slots:
    virtual void slotProcess() = 0;
    virtual void slotFinished() = 0;

protected:
    QSocket *m_socket;
    QFile *m_file;
    unsigned int m_bufSize;
    char *m_buffer;
    Q_UINT32 m_ip4Addr;
    Q_UINT16 m_port;
    unsigned int m_totalSize;
    unsigned int m_processedSize;
    bool m_cancel;
    bool m_timeout;
};

#endif
