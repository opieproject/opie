#ifndef DCCPROGRESS_H
#define DCCPROGRESS_H


#include <qwidget.h>

#include "dcctransfer.h"

class QProgressBar;
class QLabel;
class QVBox;

class DCCProgress: public QWidget {

    Q_OBJECT
public:
    DCCProgress(DCCTransfer::Type type, Q_UINT32 ip4Addr, Q_UINT16 port,
            const QString &filename, const QString &nickname, unsigned int size,
            QWidget *parent = 0, char *name = 0, WFlags f = 0);

    bool finished();
    void cancel();

public slots:
    void slotSetProgress(int progress);
    void slotFinished(DCCTransfer *transfer, DCCTransfer::EndCode code);

private:
    QVBox *m_vbox;
    QLabel *m_label;
    QProgressBar *m_bar;
    DCCTransfer *m_transfer;
};

#endif
