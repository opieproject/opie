
#include "../../ipc/client/ocopclient.h"

#include <qcopchannel_qws.h>

QCopChannel::QCopChannel( const QCString& channel, QObject* parent,
                          const char* name )
    : QObject( parent, name ),m_chan(channel) {
    init();
}
QCopChannel::~QCopChannel() {
    m_client->delChannel( m_chan );
    delete m_client;
}
void QCopChannel::init() {
    m_client = new OCOPClient(QString::null, this );
    m_client->addChannel(m_chan );
    connect(m_client, SIGNAL(called(const QCString&, const QCString&, const QByteArray& ) ),
            this, SLOT(rev(const QCString&, const QCString&, const QByteArray&) ) );
}
QCString QCopChannel::channel()const {
    return m_chan;
}
bool QCopChannel::isRegistered( const QCString& chan) {
    OCOPClient client;
    return client.isRegistered( chan );
}
bool QCopChannel::send( const QCString& chan, const QCString& msg ) {
    QByteArray ar(0);
    return sendLocally(chan, msg, ar );
}
bool QCopChannel::send( const QCString& chan, const QCString& msg,
                     const QByteArray& ar ) {
    return sendLocally( chan, msg, ar );
}
bool QCopChannel::sendLocally( const QCString& chan, const QCString& msg,
                               const QByteArray& ar ) {
    OCOPClient client;
    client.send( chan, msg, ar );

    return true;
}
void QCopChannel::rev( const QCString& chan, const QCString& msg, const QByteArray& ar ) {
    if (chan == m_chan )
        emit received(msg, ar );
}
