#ifndef OSQL_DRIVER_H
#define OSQL_DRIVER_H

#include <qobject.h>
#include <qstring.h>

#include "osqltable.h"

class QLibrary;
class OSQLResult;
class OSQLQuery;
class OSQLError;

/**
 * A OSQLDriver implements the communication with
 * a database.
 * After you queried and loaded a driver you can
 * set some informations and finally try to open
 * the database
 *
 */
class OSQLDriver : public QObject{
    Q_OBJECT
public:
    enum Capabilities { RowID=0 };
    /**
     * OSQLDriver constructor. It takes the QLibrary
     * as parent.
     *
     */
    OSQLDriver( QLibrary* lib=0 );

    virtual ~OSQLDriver();
    /**
     * Id returns the identifier of the OSQLDriver
     */
    virtual QString id()const = 0;

    /**
     * set the UserName to the database
     */
    virtual void setUserName( const QString& ) = 0;

    /**
     * set the PassWord to the database
     */
    virtual void setPassword( const QString& )= 0;

    /**
     * set the Url
     */
    virtual void setUrl( const QString& ) = 0;

    /**
     * setOptions
     */
    virtual void setOptions( const QStringList& ) = 0;

    /**
     * tries to open a connection to the database
     */
    virtual bool open() = 0;
    virtual bool close() = 0;

    virtual OSQLError lastError() = 0;

    /**
     * Query the Database with a OSQLQuery
     * OSQLResult holds the result
     */
    virtual OSQLResult query( OSQLQuery* ) = 0;

    /**
     * Get a list of tables
     */
    virtual OSQLTable::ValueList tables() const = 0l;
    virtual bool sync();


private:
    QLibrary* m_lib;
    class OSQLDriverPrivate;
    OSQLDriverPrivate *d;

};

#endif
