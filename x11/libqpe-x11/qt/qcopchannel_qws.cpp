
#include "../../ipc/client/ocopclient.h"

#include <qcopchannel_qws.h>

QList<QCopChannel>* QCopChannel::m_list = 0;
QMap<QCString, int> QCopChannel::m_refCount;

QCopChannel::QCopChannel( const QCString& channel, QObject* parent,
                          const char* name )
    : QObject( parent, name ),m_chan(channel) {
    if (!m_list ) {
        m_list = new QList<QCopChannel>;
        /* only connect once */
        connect(OCOPClient::self(), SIGNAL(called(const QCString&,const QCString&,const QByteArray&) ),
                this, SLOT(rev(const QCString&,const QCString&,const QByteArray&) ) );
    }
    /* first registration  or ref count is 0 for m_chan*/
    if (!m_refCount.contains( m_chan ) || !m_refCount[m_chan]  ) {
        qWarning("adding channel %s", m_chan.data() );
        m_refCount[m_chan] = 1;
        OCOPClient::self()->addChannel( m_chan );
    }else{
        qWarning("reffing up for %s %d", m_chan.data(), m_refCount[m_chan] );
        m_refCount[m_chan]++;
    }

    m_list->append(this);
}
void QCopChannel::receive( const QCString& msg, const QByteArray& ar ) {
    emit received( msg, ar );
}
QCopChannel::~QCopChannel() {
    if (m_refCount[m_chan] == 1 ) {
        OCOPClient::self()->delChannel( m_chan );
        m_refCount[m_chan] = 0;
    }else
        m_refCount[m_chan]--;


    m_list->remove(this);
    if (m_list->count() == 0 ) {
        delete m_list;
        m_list = 0;
    }

}
QCString QCopChannel::channel()const {
    return m_chan;
}
bool QCopChannel::isRegistered( const QCString& chan) {
    if (m_refCount.contains(chan) ) {
            qDebug("Client:locally contains");
            return true;
    }
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
    qWarning("Client:sendLocally %s %s", chann.data(), msg.data() );
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
    sendLocally( chan, msg, ar );
}
