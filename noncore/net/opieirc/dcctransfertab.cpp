#include <qlist.h>
#include <qlabel.h>
#include <qstring.h>
#include <qvbox.h>
#include <qmessagebox.h>

#include "dcctransfer.h"
#include "dccprogress.h"
#include "mainwindow.h"
#include "dcctransfertab.h"

#include <stdio.h>

DCCTransferTab::DCCTransferTab(QWidget *parent, const char *name, WFlags f)
    :IRCTab(parent, name, f), m_hbox(new QHBox(this)), m_parent(static_cast<MainWindow*>(parent))
{
    m_description->setText("");
    m_layout->add(m_hbox);
    m_hbox->show();
}

DCCTransferTab::~DCCTransferTab()
{
    if(m_hbox)
        delete m_hbox;
}

QString DCCTransferTab::title()
{
    return "DCC";
}

void DCCTransferTab::remove()
{
    //Clean finished transfers
    for(QListIterator <DCCProgress> it(m_progressList); it.current(); ++it) {
        DCCProgress *current = it.current();
        if (current->finished()) {
            m_progressList.remove(current);
            current->hide();
            delete current;
        }
    }

    if (m_progressList.count() > 0) {
        int retval = QMessageBox::information( parentWidget() , tr("DCC Transfers in Progress"),
                            tr( "There are transfers in progress. <br>If you close this tab, they will be canceled."
                                "<br>Do you want to close it anyway?"),
                            tr("&Close"), tr("&Don't Close"));
        if ( retval != 0 ) {
            return;
        }
        //Cancel active transfers (user accepted)
        for(QListIterator <DCCProgress> itr(m_progressList); itr.current(); ++itr) {
            DCCProgress *current = itr.current();
            m_progressList.remove(current);
            current->hide();
            current->cancel();
            delete current;
        }
    }

    //Remove
    m_parent->killTab(this);
}

bool DCCTransferTab::confirm(QWidget *parent, const QString &nickname, const QString &filename, unsigned int size)
{
    int retval = QMessageBox::information(parent, tr("DCC Transfer from %1").arg(nickname),
            tr( "%1 is trying to send you the file %2\n(%3 bytes)").arg(nickname).arg(filename).arg(size),
            tr("&Accept"), tr("&Reject"));

    return ( 0 == retval);
}

void DCCTransferTab::addTransfer(DCCTransfer::Type type, Q_UINT32 ip4Addr, Q_UINT16 port,
    const QString &filename, const QString &nickname, unsigned int size)
{
    m_progressList.append(new DCCProgress(type, ip4Addr, port, filename, nickname, size, this));
}
