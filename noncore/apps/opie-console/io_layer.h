#ifndef OPIE_IO_LAYER_H
#define OPIE_IO_LAYER_H

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
signals:
    /**
     * received input as QCString
     */
    virtual void received( const QByteArray& ) = 0;

    /**
     * an error occured
     * int for the error number
     * and QString for a text
     */
    virtual void error( int, const QString& ) = 0;

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
    virtual void reload( const Config& ) = 0;
};

#endif
