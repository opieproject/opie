#ifndef OPIE_QCOP_CHANNEL_QWS_H
#define OPIE_QCOP_CHANNEL_QWS_H

#include <qobject.h>
#include <qcstring.h>
#include <qlist.h>
#include <qmap.h>

class OCOPClient;
class QCopChannel : public QObject {
    Q_OBJECT
public:
    QCopChannel( const QCString& channel, QObject* parent = 0,
                 const char* name = 0);
    virtual ~QCopChannel();

    QCString channel()const;

    static bool isRegistered( const QCString& channel );
    static bool send( const QCString& channel, const QCString& msg );
    static bool send( const QCString& channel, const QCString& msg,
                      const QByteArray& );
    static bool sendLocally( const QCString& chan, const QCString& msg,
                             const QByteArray& data );
    void receive( const QCString& msg, const QByteArray& ar );

signals:
    void received( const QCString& msg, const QByteArray& );

private slots:
    void rev( const QCString& chan, const QCString&, const QByteArray& );

private:
    bool isRegisteredLocally( const QCString& str);
    static QList<QCopChannel> *m_list;
    static QMap<QCString, int> m_refCount;
    /* the channel */
    QCString m_chan;
    class Private;
    Private *d;

};

#endif
