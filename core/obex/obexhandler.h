#ifndef OPIE_OBEX_HANDLER_H
#define OPIE_OBEX_HANDLER_H

#include <qobject.h>
#include <qstring.h>
#include "receiver.h"

namespace OpieObex {
    /*
     * The handler is responsible for handling receiving
     * and sending
     * It will connect to the IrDa QCOP channel and then
     * wait for activation...
     */
    class SendWidget;
    class Receiver;
    class ObexHandler : public QObject {
        Q_OBJECT
    public:
        ObexHandler();
        ~ObexHandler();

    private slots:
        void doSend(const QString&,const QString& );
        void doReceive(RecType type, bool b);
        void slotSent();

    private slots: // QCOP message
        void irdaMessage( const QCString&, const QByteArray& );

    private:
        SendWidget* m_sender;
        Receiver* m_receiver[2]; //For IRDA and Bluetooth
        bool m_wasRec[2];
        RecType m_type; //receiver type (IRDA or Bluetooth)
    };
}


#endif
