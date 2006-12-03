//  7-Jul-2005 mbh@sdgsystems.com: replace hand coded form with one
//             generated via QT2 Designer.  The new form supports
//             selection of target devices, as opposed to sending to 
//             all.

#include "obex.h"
#ifdef BLUETOOTH
#include "btobex.h"
#endif
#include "obexsend.h"
using namespace OpieObex;
#ifdef BLUETOOTH
using namespace OpieTooth;
#endif

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie2/oresource.h>
#include <qpe/version.h>
#ifdef BLUETOOTH
#include <devicehandler.h>
#include "remotedevice.h"
#endif

using namespace Opie::Core;

/* QT */
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qlistview.h>

#include <unistd.h>
/* TRANSLATOR OpieObex::SendWidget */

/* Just for backward compatibility */
#if OPIE_VERSION < 102010
    #define OResource Resource
#endif

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

#ifdef BLUETOOTH
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
#endif
}

/*
 * in send we'll first set everything up
 * and then wait for a list of devices.
 */
void SendWidget::send( const QString& file, const QString& desc ) {
    m_file = file;
    m_irDa.clear();
#ifdef BLUETOOTH
    m_bt.clear();
#endif
    m_start = 0;

    fileToSend->setText(desc.isEmpty() ? file : desc );

    if ( !QCopChannel::isRegistered("QPE/IrDaApplet") )
    {
        irdaStatus->setText(tr("not enabled."));
    }
    else
    {
        QCopEnvelope e1("QPE/IrDaApplet", "enableIrda()");
        irdaStatus->setText(tr("ready"));
        sendButton->setEnabled( true );
    }
#ifdef BLUETOOTH
    if ( !QCopChannel::isRegistered("QPE/Bluetooth") )
    {
        btStatus->setText(tr("not enabled."));
    }
    else
    {
        QCopEnvelope e1("QPE/Bluetooth", "enableBluetooth()");
	btStatus->setText(tr("ready."));
	sendButton->setEnabled( true );
    }
    read_receivers();
#endif
}

int SendWidget::addReceiver(const QString& str, const char *icon)
{
    QListViewItem * item = new QListViewItem( receiverList, 0 );
    item->setText( 0, str );
    item->setPixmap( 1, OResource::loadPixmap( icon ) );

    int id = receivers.count();
    receivers[id] = item;
    return id;
}

bool SendWidget::receiverSelected(int id)
{
    return (bool)(receivers[id]->pixmap(2) != NULL);
}

void SendWidget::setReceiverStatus( int id, const QString& status ) {
    if ( !receivers.contains(id) ) return;
    receivers[id]->setText(3, status );
}

void SendWidget::slotIrDaDevices( const QStringList& list) {
    for (QStringList::ConstIterator it = list.begin(); 
        it != list.end(); ++it ) {
        int id = addReceiver(*it, "obex/irda.png");
        m_irDa.insert( id, (*it) );
    }
    irdaStatus->setText( tr("ready."));
    m_irDaIt = m_irDa.begin();

}

void SendWidget::slotBTDevices( const QMap<QString, QString>& str ) {
#ifdef BLUETOOTH
    for(QMap<QString, QString>::ConstIterator it = str.begin(); 
        it != str.end(); ++it ) {
        int id = addReceiver(it.key(), "obex/bt.png");
        m_bt.insert( id, Pair( it.key(), it.data() ) );
    }
    btStatus->setText(tr("ready."));
    m_btIt = m_bt.begin();

#else
    (void)str;
#endif
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
#ifdef BLUETOOTH
    QString text = b ? tr("Sent") : tr("Failure");
    setReceiverStatus( m_irDaIt.key(), text );
    ++m_irDaIt;
    slotStartIrda();
#else
    (void)b;
#endif
}
void SendWidget::slotIrTry(unsigned int trI) {
    setReceiverStatus(m_irDaIt.key(), tr("Try %1").arg( QString::number( trI ) ));
}
void SendWidget::slotStartIrda() {
    if ( !m_irDa.count() ) 
        return;
    if ( m_irDaIt == m_irDa.end() || !receiverSelected(m_irDaIt.key())) {
        irdaStatus->setText(tr("complete."));
	m_irDaIt = m_irDa.begin();
        return;
    }
    setReceiverStatus( m_irDaIt.key(), tr("Start sending") );
    irdaStatus->setText(tr("sending."));
    m_obex->send( m_file, tr("noaddress") );
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
#ifdef BLUETOOTH
    QString text = b ? tr("Sent") : tr("Failure");
    setReceiverStatus( m_btIt.key(), text );
    ++m_btIt;
    slotStartBt();
#else
    (void)b;
#endif
}
void SendWidget::slotBtTry(unsigned int trI) {
#ifdef BLUETOOTH
    setReceiverStatus( m_btIt.key(), tr("Try %1").arg( QString::number( trI ) ) );
#else
    (void)trI;
#endif
}
void SendWidget::slotStartBt() {
#ifdef BLUETOOTH
    // skip past unselected receivers
    if ( !m_bt.count() ) 
        return;
    while((m_btIt != m_bt.end()) && !receiverSelected(m_btIt.key()))
        ++m_btIt;
    if (m_btIt == m_bt.end() ) {
        btStatus->setText(tr("complete."));
	m_btIt = m_bt.begin();
        return;
    }
    setReceiverStatus( m_btIt.key(), tr("Start sending") );
    btStatus->setText(tr("sending."));
    m_btobex->send( m_file, m_btIt.data().second() );
#endif
}

void SendWidget::send_to_receivers() {
#ifdef BLUETOOTH
    slotStartBt();
#endif
    slotStartIrda();
}

#ifdef BLUETOOTH
/**
 * Read receivers saved by bluetooth manager
 */
void SendWidget::read_receivers()
{
    QValueList<RemoteDevice> devices;
    DeviceHandler handler;
    QValueList<RemoteDevice>::ConstIterator it;

    receiverList->clear();
    receivers.clear();
    sendButton->setDisabled( true );
    btStatus->setText(tr("load."));
    m_bt.clear();

    if ( QCopChannel::isRegistered("QPE/Bluetooth") )
    {
	devices = handler.load();
	for( it = devices.begin(); it != devices.end() ; ++it )
	{
	    int id = addReceiver((*it).name(), "obex/bt.png");
	    m_bt.insert(id, Pair((*it).name(), (*it).mac()));
	}
	btStatus->setText(tr("ready."));
	m_btIt = m_bt.begin();
	sendButton->setEnabled( true );
    }
}
#endif

void SendWidget::scan_for_receivers()
{
    sendButton->setDisabled( true );
    receiverList->clear();
    receivers.clear();
    m_irDa.clear();
#ifdef BLUETOOTH
    m_bt.clear();
#endif
    if ( QCopChannel::isRegistered("QPE/IrDaApplet") )
    {
        irdaStatus->setText(tr("searching..."));
        sendButton->setEnabled( true );
        QCopEnvelope e2("QPE/IrDaApplet", "listDevices()");
    }

#ifdef BLUETOOTH
    if ( QCopChannel::isRegistered("QPE/Bluetooth") )
    {
        btStatus->setText(tr("searching..."));
        sendButton->setEnabled( true );
        QCopEnvelope e3("QPE/Bluetooth", "listDevices()");
    }
#endif
}

void SendWidget::toggle_receiver(QListViewItem* item)
{
    if (!item)
	return;
    // toggle the state of an individual receiver.
    if (item->pixmap(2))
        item->setPixmap(2, QPixmap());
    else
        item->setPixmap(2, OResource::loadPixmap("obex/check.png"));
}


void SendWidget::closeEvent( QCloseEvent* evt) {
    delete m_obex;
    m_obex = NULL;
#ifdef BLUETOOTH
    delete m_btobex;
    m_btobex = NULL;
#endif
    obexSendBase::closeEvent(evt);
    {
        QCopEnvelope e("QPE/IrDaApplet", "disableIrda()");
    }
#ifdef BLUETOOTH
    {
        QCopEnvelope e("QPE/Bluetooth", "disableBluetooth()");
    }
#endif
}

void SendWidget::userDone() {
    close();
}

QString SendWidget::file()const {
    return m_file;
}
