/* GPL!*/

#ifndef OPIE_OCOP_PACKET_H
#define OPIE_OCOP_PACKET_H

#include <qcstring.h>
#include <qstring.h>

/**
 * This is the head which will be sent
 * in advance to every packet
 */
struct OCOPHead {
    int magic;
    int type;
    int chlen;
    int funclen;
    int datalen;
};

/**
 * This is the basic packet we will
 * use for the communication between server
 * and client
 */
class OCOPPacket {
public:
    enum Type {
        Register = 0, Unregister, Call,
        Method,  Reply, RegisterChannel,
        UnregisterChannel, Return, Signal,
        IsRegistered
    };
    /**
     * the c'tor
     * type the Type of this packet
     * the Channel
     * the header of the function
     * the data inside a QByteArray
     */
    OCOPPacket( int type, const QCString& channel = QCString(),
                const QCString& header = QCString(),
                const QByteArray& array = QByteArray() );

    QByteArray toByteArray()const;
    int type()const;
    QCString channel()const;
    QCString header()const;
    QByteArray content()const;
    OCOPHead head()const;

    void setType( int type );
    void setChannel( const QCString& );
    void setHeader(const QCString& );
    void setContent( const QByteArray& );

private:
    int m_type;
    QCString m_channel;
    QCString m_header;
    QByteArray m_content;
};

#endif
