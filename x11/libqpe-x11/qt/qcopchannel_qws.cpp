
#include "../../ipc/client/ocopclient.h"

#include <qcopchannel_qws.h>

QList<QCopChannel>* QCopChannel::m_list = 0;

QCopChannel::QCopChannel( const QCString& channel, QObject* parent,
                          const char* name )
    : QObject( parent, name ),m_chan(channel) {
    init();
    if (!m_list ) {
        m_list = new QList<QCopChannel>;
    }
    m_list->append(this);
}
void QCopChannel::receive( const QCString& msg, const QByteArray& ar ) {
    emit received( msg, ar );
}
QCopChannel::~QCopChannel() {
    m_list->remove(this);
    if (m_list->count() == 0 ) {
        delete m_list;
        m_list = 0;
    }
    OCOPClient::self()->delChannel( m_chan );
}
void QCopChannel::init() {
    OCOPClient::self()->addChannel( m_chan );
    connect(OCOPClient::self(), SIGNAL(called(const QCString&, const QCString&, const QByteArray& ) ),
            this, SLOT(rev(const QCString&, const QCString&, const QByteArray&) ) );
}
QCString QCopChannel::channel()const {
    return m_chan;
}
bool QCopChannel::isRegistered( const QCString& chan) {;
    return OCOPClient::self()->isRegistered( chan );
}
bool QCopChannel::send( const QCString& chan, const QCString& msg ) {
    QByteArray ar;
    return send(chan, msg, ar );
}
bool QCopChannel::send( const QCString& chan, const QCString& msg,
                     const QByteArray& ar ) {
    OCOPClient::self()->send( chan, msg, ar );
    return true;
}
bool QCopChannel::sendLocally( const QCString& chann, const QCString& msg,
                               const QByteArray& ar ) {
    if (!m_list )
        return true;
    QCopChannel* chan;
    for ( chan = m_list->first(); chan; chan = m_list->next() ) {
        if ( chan->channel() == chann )
            chan->receive( msg, ar );
    }

    return true;
}
void QCopChannel::rev( const QCString& chan, const QCString& msg, const QByteArray& ar ) {
    if (chan == m_chan )
        emit received(msg, ar );
}
