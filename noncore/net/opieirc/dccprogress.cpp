#include <qprogressbar.h>
#include <qlabel.h>
#include <qvbox.h>

#include "dcctransfer.h"
#include "dcctransferrecv.h"
#include "dccprogress.h"

DCCProgress::DCCProgress(DCCTransfer::Type type, Q_UINT32 ip4Addr, Q_UINT16 port,
    const QString &filename, const QString &nickname, unsigned int size, QWidget *parent, char *name, WFlags f)
    : QWidget(parent, name, f), m_vbox(new QVBox(this)),
    m_label(new QLabel(m_vbox)),
    m_bar(new QProgressBar(m_vbox))
{

    if (DCCTransfer::Recv == type)
       m_transfer = new DCCTransferRecv(ip4Addr, port, filename, size);


    connect(m_transfer, SIGNAL(progress(int)), this, SLOT(slotSetProgress(int)));
    connect(m_transfer, SIGNAL(finished(DCCTransfer *, DCCTransfer::EndCode)),
            this, SLOT(slotFinished(DCCTransfer *, DCCTransfer::EndCode)));

    m_label->setText(tr("Receiving file %1 from %2...").arg(m_transfer->filename()).arg(nickname));

    m_label->show();
    m_bar->show();
    show();

}

bool DCCProgress::finished()
{
    return ( m_transfer == 0);
}

void DCCProgress::cancel()
{
    if (m_transfer)
        m_transfer->cancel();
}

void DCCProgress::slotSetProgress(int progress)
{
    m_bar->setProgress(progress);
}

void DCCProgress::slotFinished(DCCTransfer *transfer, DCCTransfer::EndCode code)
{
    if(transfer != m_transfer)
        // WTF!!
        return;

    QString msg;

    switch(code) {
        case DCCTransfer::Successfull:
            msg = tr("Successfully received %1").arg(m_transfer->filename());
            break;
        case DCCTransfer::SelfAborted:
            msg = tr("Aborted");
            break;
        case DCCTransfer::PeerAborted:
            msg = tr("Peer Aborted");
            break;
        case DCCTransfer::Timeout:
            msg = tr("Timeout");
            break;
    }

    m_label->setText(msg);
    delete m_transfer;
    m_transfer = 0;
}

