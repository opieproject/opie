#include <qpushbutton.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qtimer.h>

#include <qcopchannel_qws.h>

#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include "obex.h"
#include "obexsend.h"

using namespace OpieObex;


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
    connect(m_devBox, SIGNAL(selectedDevice(const QString&, int ) ),
            this, SLOT(slotSelectedDevice(const QString&, int) ) );

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
        m_devBox->addDevice( tr("IrDa is not enabled!"), DeviceBox::Error );
        m_start++;
    }else
        m_devBox->addDevice( tr("Searching for IrDa Devices."), DeviceBox::Search );

    if ( !QCopChannel::isRegistered("QPE/Bluetooth") ) {
        m_devBox->addDevice( tr("Bluetooth is not available"), DeviceBox::Error );
        m_start++;
    }else
        m_devBox->addDevice( tr("Searching for bluetooth Devices."), DeviceBox::Search );

    if (m_start != 2 ) {
        QCopEnvelope e0("QPE/IrDaApplet", "enableIrda()");
        QCopEnvelope e1("QPE/Bluetooth", "enableBluetooth()");
        QCopEnvelope e2("QPE/IrDaApplet", "listDevices()");
        QCopEnvelope e3("QPE/Bluetooth", "listDevices()");
    }
}
void SendWidget::slotIrDaDevices( const QStringList& list) {
    qWarning("slot it irda devices ");
    m_irDa = list;
    m_start = 0;
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it )
        m_devBox->addDevice( (*it), DeviceBox::IrDa, tr("Scheduling for beam.") );

    m_devBox->removeDevice( tr("Searching for IrDa Devices.") );

    slotStartIrda();
}
void SendWidget::slotBTDevices( const QMap<QString, QString>& str ) {
    m_bt = str;
    for(QMap<QString, QString>::ConstIterator it = str.begin(); it != str.end(); ++it ) {
        m_devBox->addDevice( it.key(), DeviceBox::BT, tr("Click to beam") );
    }
    m_devBox->removeDevice( tr("Searching for bluetooth Devices.") );
}
void SendWidget::slotSelectedDevice( const QString& name, int dev ) {
    qWarning("Start beam? %s %d", name.latin1(), dev );
    if ( name == tr("Search again for IrDa.") ) {
        for (QStringList::Iterator it= m_irDa.begin(); it != m_irDa.end(); ++it )
            m_devBox->removeDevice( (*it) );

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
void SendWidget::dispatchBt( const QCString& str, const QByteArray& ar ) {

}
void SendWidget::slotIrError( int ) {

}
void SendWidget::slotIrSent( bool b) {
    qWarning("irda sent!!");
    QString text = b ? tr("Sent") : tr("Failure");
//    m_devBox->setStatus( m_irDa[m_start], text );
    m_start++;
    slotStartIrda();
}
void SendWidget::slotIrTry(unsigned int trI) {
//    m_devBox->setStatus( m_irDa[m_start], tr("Try %1").arg( QString::number( trI ) ) );
}
void SendWidget::slotStartIrda() {
    if (m_start >= m_irDa.count() ) {
        m_devBox->addDevice(tr("Search again for IrDa."), DeviceBox::Search );
        return;
    }
//    m_devBox->setStatus( m_irDa[m_start], tr("Start sending") );
    m_obex->send( m_file );
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
void DeviceBox::addDevice( const QString& name, int dev, const QString& status ) {
    QString tex;
    DeviceItem item( name, status, dev );
    m_dev.insert( name, item );
    tex = item.toString();
    m_devices.prepend(tex);
    setText( text()+ "<br>"+tex );
}
void DeviceBox::removeDevice( const QString& name ) {
    if (!m_dev.contains(name) ) return;
    m_devices.remove( m_dev[name].toString() );

    m_dev.remove(name);
    setText( m_devices.join("<br>") );

}
void DeviceBox::setStatus( const QString& name, const QString& status ) {
    if ( !m_dev.contains(name) ) return;
    DeviceItem dev = m_dev[name];
    QString ole = dev.toString();
    dev.setStatus( status );
    int index = m_devices.findIndex( ole );
    m_devices[index] = dev.toString();
    setText( m_devices.join("<br>") );
}
void DeviceBox::setSource( const QString& str ) {
    qWarning("SetSource:%s", str.latin1() );
    emit selectedDevice( str, m_dev[str].device() );
}


DeviceItem::DeviceItem( const QString& name,
                        const QString& status, int dev)
{
    m_name = name;
    m_status = status;
    m_dev = dev;
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
        str = "obex/search";
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
    return "<p><a href=\""+m_name +"\" ><img src=\""+pixmap()+"\" >"+m_name+" "+m_status+" </a></p>" ;
}
