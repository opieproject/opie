#ifndef OPIE_IO_LAYER_H
#define OPIE_IO_LAYER_H

#include <qbitarray.h>
#include <qobject.h>


#include <qpe/config.h>

#include "profile.h"

/**
 * This is the base class for IO Layers
 * It will used to sent and recv data( QByteArray )
 * it
 */
class IOLayer : public QObject {
    Q_OBJECT
public:
    enum Error {
        NoError = -1,
        Refuse = 0,
        CouldNotOpen =1,
        ClosedUnexpected =2,
        ClosedError =3,
        Terminate = 4
        /* add more errors here */
    };
    enum Feature {
        AutoConnect = 0,
        TransferFile =1,
        Close =2
    };
    /**
     * a small c'tor
     */
    IOLayer();

    /**
     * create an IOLayer instance from a config file
     * the currently set group stores the profile/session
     * information
     */
    IOLayer( const Profile& );

    /**
     * destructor
     */
    virtual ~IOLayer();

    /**
     * a small internal identifier
     */
    virtual QString identifier() const = 0;

    /**
     * a short name
     */
    virtual QString name() const = 0;

    /**
     * a file descriptor which opens
     * the device for io but does not
     * do any ioctling on it...
     * and it'll stop listening to the before opened
     * device
     */
    virtual int rawIO()const;

    /**
     * will close the rawIO stuff
     * and will listen to it's data again...
     */
    virtual void closeRawIO(int);

    /**
     * What does the IOLayer support?
     * Bits are related to features
     */
    virtual QBitArray supports()const = 0;

signals:
    /**
     * received input as QCString
     */
    virtual void received( const QByteArray& );

    /**
     * an error occured
     * int for the error number
     * and QString for a text
     */
    virtual void error( int, const QString& );

    virtual void closed();
public slots:
    /**
     * send a QCString to the device
     */
    virtual void send( const QByteArray& ) = 0;

    /**
     * bool open
     */
    virtual bool open() = 0;

    /**
     * close the io
     */
    virtual void close() = 0;

    /**
     * closes and reloads the settings
     */
    virtual void reload( const Profile& ) = 0;

    /**
     * set the size
     * needed for pty
     */
    virtual void setSize(int rows, int cols );
};

#endif
