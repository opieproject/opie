#ifndef OPIE_QCOP_CHANNEL_QWS_H
#define OPIE_QCOP_CHANNEL_QWS_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QMap>

class OCOPClient;
class QCopChannel : public QObject {
    Q_OBJECT
public:
    QCopChannel( const QByteArray& channel, QObject* parent = 0,
                 const char* name = 0);
    virtual ~QCopChannel();

    QByteArray channel()const;

    static bool isRegistered( const QByteArray& channel );
    static bool send( const QByteArray& channel, const QByteArray& msg );
    static bool send( const QByteArray& channel, const QByteArray& msg,
                      const QByteArray& );
    static bool sendLocally( const QByteArray& chan, const QByteArray& msg,
                             const QByteArray& data );
    void receive( const QByteArray& msg, const QByteArray& ar );

signals:
    void received( const QByteArray& msg, const QByteArray& );

private slots:
    void rev( const QByteArray& chan, const QByteArray&, const QByteArray& );

private:
    bool isRegisteredLocally( const QByteArray& str);
    static QList<QCopChannel> *m_list;
    static QMap<QByteArray, int> m_refCount;
    /* the channel */
    QByteArray m_chan;
    class Private;
    Private *d;

};

#endif
