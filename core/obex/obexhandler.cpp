#include <qcopchannel_qws.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include "obexsend.h"
#include "receiver.h"
#include "obexhandler.h"

using namespace OpieObex;

/* TRANSLATOR OpieObex::ObexHandler */

ObexHandler::ObexHandler() {
    m_wasRec = false;
    m_sender = 0l;
    m_receiver = 0l;
    QCopChannel* chan = new QCopChannel("QPE/Obex");
    connect(chan, SIGNAL(received(const QCString&, const QByteArray& ) ),
            this, SLOT(irdaMessage(const QCString&, const QByteArray& ) ) );
}
ObexHandler::~ObexHandler() {
    delete m_sender;
    delete m_receiver;
}
void ObexHandler::doSend(const QString& str, const QString& desc) {
    delete m_sender;
    m_sender = new SendWidget;
    m_sender->raise();
    QPEApplication::showWidget( m_sender );
    connect(m_sender, SIGNAL(done() ),
            this, SLOT(slotSent() ) );
    m_sender->send( str, desc );
}
void ObexHandler::doReceive(bool b) {
    if (m_receiver && b ) return; // we should enable receiver and it is on
    else if (!m_receiver && !b ) return; // we should disbale receiver and it is off
    else if (m_receiver && !b ) {
        delete m_receiver;
        m_receiver=0;
    }else if (!m_receiver && b ) {
        m_receiver= new Receiver;
    }
}
void ObexHandler::slotSent() {
    QString file = m_sender->file();
    delete m_sender;
    m_sender = 0;
    QCopEnvelope e ("QPE/Obex", "done(QString)" );
    e << file;
    doReceive(m_wasRec );
    m_wasRec = false;
}
void ObexHandler::irdaMessage( const QCString& msg, const QByteArray& data) {
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "send(QString,QString,QString)" ) {
        QString name,  desc;
        stream >> desc;
        stream >> name;
        m_wasRec = (m_receiver != 0 );
        doReceive( false );
        doSend(name, desc);
    }else if (msg == "receive(int)") {
        int rec;
        stream >> rec;
        doReceive(rec);
    }
}
