#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>


#include <qpe/qcopenvelope_qws.h>

#include "obex.h"
#include "obexsend.h"

using namespace OpieObex;

/* TRANSLATOR OpieObex::SendWidget */


SendWidget::SendWidget( QWidget* parent, const char* name )
    : QWidget( parent, name ) {
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
    connect(m_obex, SIGNAL(currentTry(unsigned int ) ),
            this, SLOT(slotIrTry(unsigned int ) ) );

    QCopChannel* chan = new QCopChannel("QPE/IrDaAppletBack", this );
    connect(chan, SIGNAL(received(const QCString&, const QByteArray& ) ),
            this, SLOT(dispatchIrda(const QCString&, const QByteArray& ) ) );

    chan = new QCopChannel("QPE/BluetoothBack", this );
    connect(chan, SIGNAL(received(const QCString&, const QByteArray& ) ),
            this, SLOT(dispatchBt(const QCString&, const QByteArray& ) ) );

    QVBoxLayout* lay = new QVBoxLayout(this);

    QHBox* nameBox = new QHBox(this);
    QLabel* name = new QLabel(nameBox);
    name->setText( tr("<qt><h1>Sending:</h1></qt>") );
    name->setAlignment( AlignLeft | AlignTop );
    m_lblFile = new QLabel(nameBox);
    lay->addWidget(nameBox, 0);

    QFrame* frame = new QFrame(this);
    frame->setFrameShape( QFrame::HLine );
    frame->setFrameShadow( QFrame::Sunken );
    lay->addWidget(frame, 10);

    QLabel* devices = new QLabel(this);
    devices->setText("<qt><b>Devices:</b></qt>");
    devices->setAlignment( AlignLeft | AlignTop );
    lay->addWidget( devices,10 );

    m_devBox = new DeviceBox(this);
    lay->addWidget( m_devBox, 50 );
    connect(m_devBox, SIGNAL(selectedDevice(int, int ) ),
            this, SLOT(slotSelectedDevice(int, int) ) );

    QPushButton *but = new QPushButton(this);
    but->setText(tr("Done") );
    connect(but, SIGNAL(clicked() ),
            this, SLOT(slotDone() ) );

    lay->addWidget( but );
    m_lay = lay;

    // QT does not like if you add items to an layout which already exits....
    // and was layouted invalidate() does not help too
    // so we use RichText....
}

/*
 * in send we'll first set everything up
 * and then wait for a list of devices.
 */
void SendWidget::send( const QString& file, const QString& desc ) {
    m_file = file;
    m_irDa.clear();
    m_start = 0;
    m_lblFile->setText(desc.isEmpty() ? file : desc );

    if ( !QCopChannel::isRegistered("QPE/IrDaApplet") ) {
        m_irDeSearch = m_devBox->addDevice( tr("IrDa is not enabled!"), DeviceBox::Error );
        m_start++;
    }else
        m_irDeSearch = m_devBox->addDevice( tr("Searching for IrDa Devices."), DeviceBox::Search );

    if ( !QCopChannel::isRegistered("QPE/Bluetooth") ) {
        m_btDeSearch = m_devBox->addDevice( tr("Bluetooth is not available"), DeviceBox::Error );
        m_start++;
    }else
        m_btDeSearch = m_devBox->addDevice( tr("Searching for bluetooth Devices."), DeviceBox::Search );

    if (m_start != 2 ) {
        QCopEnvelope e0("QPE/IrDaApplet", "enableIrda()");
        QCopEnvelope e1("QPE/Bluetooth", "enableBluetooth()");
        QCopEnvelope e2("QPE/IrDaApplet", "listDevices()");
        QCopEnvelope e3("QPE/Bluetooth", "listDevices()");
    }
}
void SendWidget::slotIrDaDevices( const QStringList& list) {
    qWarning("slot it irda devices ");
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
        int id = m_devBox->addDevice( (*it), DeviceBox::IrDa, tr("Scheduling for beam.") );
        m_irDa.insert( id, (*it) );
    }
    m_devBox->removeDevice( m_irDeSearch );
    m_irDaIt = m_irDa.begin();

    slotStartIrda();
}

void SendWidget::slotBTDevices( const QMap<QString, QString>& str ) {
    for(QMap<QString, QString>::ConstIterator it = str.begin(); it != str.end(); ++it ) {
        int id = m_devBox->addDevice( it.key(), DeviceBox::BT, tr("Click to beam") );
        m_bt.insert( id, Pair( it.key(), it.data() ) );
    }
    m_devBox->removeDevice( m_btDeSearch );
}
void SendWidget::slotSelectedDevice( int name, int dev ) {
    qWarning("Start beam? %d %d", name, dev );
    if ( name ==  m_irDeSearch ) {
        for (QMap<int, QString>::Iterator it= m_irDa.begin(); it != m_irDa.end(); ++it )
            m_devBox->removeDevice( it.key() );

        QCopEnvelope e2("QPE/IrDaApplet", "listDevices()");
    }
}
void SendWidget::dispatchIrda( const QCString& str, const QByteArray& ar ) {
    qWarning("dispatch irda %s", str.data() );
    if ( str == "devices(QStringList)" ) {
        QDataStream stream( ar, IO_ReadOnly );
        QStringList list;
        stream >> list;
        slotIrDaDevices( list );
    }
}
void SendWidget::dispatchBt( const QCString&, const QByteArray&  ) {

}
void SendWidget::slotIrError( int ) {

}
void SendWidget::slotIrSent( bool b) {
    qWarning("irda sent!!");
    QString text = b ? tr("Sent") : tr("Failure");
    m_devBox->setStatus( m_irDaIt.key(), text );
    ++m_irDaIt;
    slotStartIrda();
}
void SendWidget::slotIrTry(unsigned int trI) {
    m_devBox->setStatus( m_irDaIt.key(), tr("Try %1").arg( QString::number( trI ) ) );
}
void SendWidget::slotStartIrda() {
    if (m_irDaIt == m_irDa.end() ) {
        m_irDeSearch = m_devBox->addDevice(tr("Search again for IrDa."), DeviceBox::Search );
        return;
    }
    m_devBox->setStatus( m_irDaIt.key(), tr("Start sending") );
    m_obex->send( m_file );
}
void SendWidget::closeEvent( QCloseEvent* e) {
    e->accept(); // make sure
    QTimer::singleShot(0, this, SLOT(slotDone() ) );
}
void SendWidget::slotDone() {
    QCopEnvelope e0("QPE/IrDaApplet", "disableIrda()");
    QCopEnvelope e1("QPE/Bluetooth", "disableBluetooth()");
    emit done();
}
QString SendWidget::file()const {
    return m_file;
}
DeviceBox::DeviceBox( QWidget* parent )
    : QTextBrowser( parent ) {

}
DeviceBox::~DeviceBox() {

}
int DeviceBox::addDevice( const QString& name, int dev, const QString& status ) {
    /* return a id for a range of devices */
    int id = idFor ( dev );
    DeviceItem item( name, status, dev,id );
    m_dev.insert( id, item );
    setText( allText() );

    return id;
}
void DeviceBox::removeDevice( int id ) {
    if (!m_dev.contains(id) ) return;

    m_dev.remove( id );
    setText( allText() );
}
void DeviceBox::setStatus( int id, const QString& status ) {
    if ( !m_dev.contains(id) ) return;
    m_dev[id].setStatus(status );
    setText( allText() );
}
void DeviceBox::setSource( const QString& str ) {
    qWarning("SetSource:%d", str.toInt() );
    int id = str.toInt();
    emit selectedDevice( id, m_dev[id].device() );
}
int DeviceBox::idFor ( int id ) {
    static int irId = 1501;
    static int irBT = 1001;
    static int irSr = 501;
    static int irEr = 0;

    int ret = -1;
    switch(id ) {
    case IrDa:
        ret = irId;
        irId++;
        break;
    case BT:
        ret = irBT;
        irBT++;
        break;
    case Search:
        ret = irSr;
        irSr++;
        break;
    case Error:
        ret = irEr;
        irEr++;
        break;
    }
    return ret;
}
QString DeviceBox::allText() {
    QString str;
    typedef QMap<int, DeviceItem> DeviceMap;

    for (QMap<int, DeviceItem>::Iterator it = m_dev.begin(); it != m_dev.end(); ++it ) {
        str += it.data().toString() + "<br>";
    }
    return str;
}

DeviceItem::DeviceItem( const QString& name,
                        const QString& status, int dev,  int id)
{
    m_name = name;
    m_status = status;
    m_dev = dev;
    m_id = id;
}
int DeviceItem::id()const {
    return m_id;
}
QString DeviceItem::name()const {
    return m_name;
}
QString DeviceItem::status()const {
    return m_status;
}
int DeviceItem::device()const {
    return m_dev;
}
QString DeviceItem::pixmap()const{
    QString str;
    switch(m_dev) {
    case DeviceBox::IrDa:
        str ="obex/irda";
        break;
    case DeviceBox::BT:
        str ="obex/bt";
        break;
    case DeviceBox::Search:
        str = "mag";
        break;
    case DeviceBox::Error:
        str = "editdelete";
        break;
    };
    return str;
}
DeviceItem::~DeviceItem() {
}
void DeviceItem::setStatus(const QString& status ) {
    m_status = status;
}
QString DeviceItem::toString()const {
    return "<p><a href=\""+QString::number(m_id) +"\" ><img src=\""+pixmap()+"\" >"+m_name+" "+m_status+" </a></p>" ;
}
