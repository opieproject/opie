#ifndef DCCTRANSFERRECV_H
#define DCCTRANSFERRECV_H

#include "dcctransfer.h"


class DCCTransferRecv: public DCCTransfer {
    Q_OBJECT
public:
    DCCTransferRecv(Q_UINT32 ip4Addr, Q_UINT16 port, const QString &filename, unsigned int size);

signals:
    void finished(DCCTransfer *transfer, DCCTransfer::EndCode code);

public slots:
    void slotProcess();
    void slotFinished();
};

#endif

