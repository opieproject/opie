#ifndef OPIE_OCOP_CLIENT_H
#define OPIE_OCOP_CLIENT_H


#include <qobject.h>
#include <qcstring.h>
#include <qmap.h>
#include <qsignal.h>
#include <qstring.h>
#include <qsocketnotifier.h>


/**
 * This is the OCOP client
 * It currently only supports
 * asking if a Channel is registered,
 * calling and receiving calls
 */
class OCOPPacket;
class OCOPClient : public QObject{
    Q_OBJECT
public:

    /**
     * Occasionally I decide to start a Server from here
     */
    OCOPClient(const QString& pathToServer = QString::null, QObject* obj = 0l);
    ~OCOPClient();

    bool isRegistered( const QCString& )const;
    void send( const QCString& chan, const QCString&, const QByteArray& msg );

    /**
     * add a channel and does connect to a signal
     * callback is the object
     * slot is the SLOT()
     */
    void addChannel( const QCString& channel );
    void delChannel( const QCString& channel );

    /* make it singleton? */
    //static OCOPClient* self();
/* no direct signals due the design */
signals:
    void called(const QCString&, const QCString&, const QByteArray& );
private slots:
    void init(const QCString& pa);
    void newData();
private:
    OCOPPacket packet();
    void call( const OCOPPacket& );

    QSocketNotifier* m_notify;
    int m_socket;
private slots:

};

#endif
