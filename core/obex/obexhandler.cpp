
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include "obexsend.h"
#include "receiver.h"
#include "obexhandler.h"

using namespace OpieObex;

/* TRANSLATOR OpieObex::ObexHandler */

ObexHandler::ObexHandler() {
    m_wasRec[REC_IRDA] = false;
    m_receiver[REC_IRDA] = 0l;
    m_wasRec[REC_BLUETOOTH] = false;
    m_receiver[REC_BLUETOOTH] = 0l;
    m_sender = 0l;
    m_type = REC_IRDA; //FIXME: Just to init to something
    QCopChannel* chan = new QCopChannel("QPE/Obex");
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(irdaMessage(const QCString&,const QByteArray&) ) );
}
ObexHandler::~ObexHandler() {
    delete m_sender;
    delete m_receiver[REC_IRDA];
    delete m_receiver[REC_BLUETOOTH];
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
void ObexHandler::doReceive(RecType type, bool b) {
    if (m_receiver[type] && b ) return; // we should enable receiver and it is on
    else if (!m_receiver[type] && !b ) return; // we should disbale receiver and it is off
    else if (m_receiver[type] && !b ) {
        delete m_receiver[type];
        m_receiver[type] = 0;
    }else if (!m_receiver[type] && b ) {
        m_receiver[type] = new Receiver(type);
    }
}
void ObexHandler::slotSent() {
    QString file = m_sender->file();
    delete m_sender;
    m_sender = 0;
    QCopEnvelope e ("QPE/Obex", "done(QString)" );
    e << file;
    doReceive(REC_IRDA, m_wasRec[REC_IRDA]);
    doReceive(REC_BLUETOOTH, m_wasRec[REC_BLUETOOTH]);
    m_wasRec[REC_IRDA] = false;
    m_wasRec[REC_BLUETOOTH] = false;
}
void ObexHandler::irdaMessage( const QCString& msg, const QByteArray& data) {
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "send(QString,QString,QString)" ) {
        QString name,  desc;
        stream >> desc;
        stream >> name;
        m_wasRec[REC_IRDA] = (m_receiver[REC_IRDA] != 0 );
        m_wasRec[REC_BLUETOOTH] = (m_receiver[REC_BLUETOOTH] != 0 );
        doReceive(REC_IRDA, false);
        doReceive(REC_BLUETOOTH, false);
        doSend(name, desc);
    }else if (msg == "receive(int)") {
        int rec;
        stream >> rec;
        doReceive(REC_IRDA, rec);
    }else if (msg == "btreceive(int)") {
        int rec;
        stream >> rec;
        doReceive(REC_BLUETOOTH, rec);
    }
}
