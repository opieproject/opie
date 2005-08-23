//  7-Jul-2005 mbh@sdgsystems.com: replace hand coded form with one
//             generated via QT2 Designer.  The new form supports
//             selection of target devices, as opposed to sending to 
//             all.

#include "obex.h"
#include "btobex.h"
#include "obexsend.h"
using namespace OpieObex;

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>

using namespace Opie::Core;

/* QT */
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qlistview.h>
#include <qtimer.h>

/* TRANSLATOR OpieObex::SendWidget */

SendWidget::SendWidget( QWidget* parent, const char* name )
    : obexSendBase( parent, name ) {
    initUI();
}
SendWidget::~SendWidget() {
}
void SendWidget::initUI() {
    m_obex = new Obex(this, "obex");
    connect(m_obex, SIGNAL(error(int) ),
            this, SLOT(slotIrError(int) ) );
    connect(m_obex, SIGNAL(sent(bool) ),
            this, SLOT(slotIrSent(bool) ) );
    connect(m_obex, SIGNAL(currentTry(unsigned int) ),
            this, SLOT(slotIrTry(unsigned int) ) );

    QCopChannel* chan = new QCopChannel("QPE/IrDaAppletBack", this );
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(dispatchIrda(const QCString&,const QByteArray&) ) );

    m_btobex = new BtObex(this, "btobex");
    connect(m_btobex, SIGNAL(error(int) ),
            this, SLOT(slotBtError(int) ) );
    connect(m_btobex, SIGNAL(sent(bool) ),
            this, SLOT(slotBtSent(bool) ) );
    connect(m_btobex, SIGNAL(currentTry(unsigned int) ),
            this, SLOT(slotBtTry(unsigned int) ) );

    chan = new QCopChannel("QPE/BluetoothBack", this );
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(dispatchBt(const QCString&,const QByteArray&) ) );

}

/*
 * in send we'll first set everything up
 * and then wait for a list of devices.
 */
void SendWidget::send( const QString& file, const QString& desc ) {
    m_file = file;
    m_irDa.clear();
    m_start = 0;

    fileToSend->setText(desc.isEmpty() ? file : desc );
    scan_for_receivers();
}

int SendWidget::addReceiver(const char *r, const char *icon)
{
    QListViewItem * item = new QListViewItem( receiverList, 0 );
    item->setText( 0, r);
    item->setPixmap( 1, Resource::loadPixmap( icon ) );

    int id=receivers.count();
    receivers[id]=item;
    return id;
}

bool SendWidget::receiverSelected(int id)
{
    return receivers[id]->pixmap(2);
}

void SendWidget::setReceiverStatus( int id, const QString& status ) {
    if ( !receivers.contains(id) ) return;
    receivers[id]->setText(3, status );
}

void SendWidget::slotIrDaDevices( const QStringList& list) {
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
	int id = addReceiver(*it, "obex/irda.png");
	m_irDa.insert( id, (*it) );
    }
    irdaStatus->setText( tr("ready."));
    m_irDaIt = m_irDa.begin();

}

void SendWidget::slotBTDevices( const QMap<QString, QString>& str ) {
    for(QMap<QString, QString>::ConstIterator it = str.begin(); it != str.end(); ++it ) {
	int id = addReceiver(it.key(), "obex/bt.png");
        m_bt.insert( id, Pair( it.key(), it.data() ) );
    }
    btStatus->setText(tr("ready."));
    m_btIt = m_bt.begin();

}
void SendWidget::slotSelectedDevice( int, int ) {
/*    if ( name ==  m_irDeSearch ) {
        for (QMap<int, QString>::Iterator it= m_irDa.begin(); it != m_irDa.end(); ++it )
            m_devBox->removeDevice( it.key() );

        QCopEnvelope e2("QPE/IrDaApplet", "listDevices()");
	}*/
}
void SendWidget::dispatchIrda( const QCString& str, const QByteArray& ar ) {
    if ( str == "devices(QStringList)" ) {
        QDataStream stream( ar, IO_ReadOnly );
        QStringList list;
        stream >> list;
        slotIrDaDevices( list );
    }
}
void SendWidget::slotIrError( int ) {
    irdaStatus->setText(tr("error :("));
}
void SendWidget::slotIrSent( bool b) {
    QString text = b ? tr("Sent") : tr("Failure");
    setReceiverStatus( m_irDaIt.key(), text );
    ++m_irDaIt;
    slotStartIrda();
}
void SendWidget::slotIrTry(unsigned int trI) {
    setReceiverStatus(m_irDaIt.key(), tr("Try %1").arg( QString::number( trI ) ));
}
void SendWidget::slotStartIrda() {
	if ( !m_irDa.count() ) return;
    if ( m_irDaIt == m_irDa.end() ) {
	irdaStatus->setText(tr("complete."));
        return;
    }
    setReceiverStatus( m_irDaIt.key(), tr("Start sending") );
    m_obex->send( m_file );
}

void SendWidget::dispatchBt( const QCString& str, const QByteArray& ar ) {
    if ( str == "devices(QStringMap)" ) {
        QDataStream stream( ar, IO_ReadOnly );
	QMap<QString, QString> btmap;
        stream >> btmap;
        slotBTDevices( btmap );
    }
}
void SendWidget::slotBtError( int ) {
    btStatus->setText(tr("error :("));    
}
void SendWidget::slotBtSent( bool b) {
    QString text = b ? tr("Sent") : tr("Failure");
    setReceiverStatus( m_btIt.key(), text );
    ++m_btIt;
    slotStartBt();
}
void SendWidget::slotBtTry(unsigned int trI) {
    setReceiverStatus( m_btIt.key(), tr("Try %1").arg( QString::number( trI ) ) );
}
void SendWidget::slotStartBt() {
    // skip past unselected receivers
    while((m_btIt != m_bt.end()) && !receiverSelected(m_btIt.key()))
	  ++m_btIt;
    if (m_btIt == m_bt.end() ) {
	btStatus->setText(tr("complete."));
        return;
    }
    setReceiverStatus( m_btIt.key(), tr("Start sending") );
    m_btobex->send( m_file, m_btIt.data().second() );
}

void SendWidget::send_to_receivers() {
    slotStartIrda();
    slotStartBt();
}

void SendWidget::scan_for_receivers()
{
    //FIXME: Clean ListBox prior to (re)scan
    sendButton->setDisabled( true );

    if ( !QCopChannel::isRegistered("QPE/IrDaApplet") )
    {
        irdaStatus->setText(tr("not enabled."));
    }
    else
    {
        QCopEnvelope e1("QPE/IrDaApplet", "enableIrda()");
        irdaStatus->setText(tr("searching..."));
        sendButton->setEnabled( true );
        QCopEnvelope e2("QPE/IrDaApplet", "listDevices()");
    }

    if ( !QCopChannel::isRegistered("QPE/Bluetooth") )
    {
        btStatus->setText(tr("not enabled."));
    }
    else
    {
        QCopEnvelope e1("QPE/Bluetooth", "enableBluetooth()");
        btStatus->setText(tr("searching..."));
        sendButton->setEnabled( true );
        QCopEnvelope e3("QPE/Bluetooth", "listDevices()");
    }
}

void SendWidget::toggle_receiver(QListViewItem* item)
{
    // toggle the state of an individual receiver.
    if(item->pixmap(2))
	item->setPixmap(2,QPixmap());
    else
	item->setPixmap(2,Resource::loadPixmap("backup/check.png"));
}


void SendWidget::closeEvent( QCloseEvent* e) {
    e->accept(); // make sure
    QTimer::singleShot(0, this, SLOT(userDone() ) );
}
void SendWidget::userDone() {
    QCopEnvelope e0("QPE/IrDaApplet", "disableIrda()");
    QCopEnvelope e1("QPE/Bluetooth", "disableBluetooth()");
    emit done();
}
QString SendWidget::file()const {
    return m_file;
}
