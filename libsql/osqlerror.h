#ifndef OSQL_ERROR_H
#define OSQL_ERROR_H

#include <qstring.h>
#include <qvaluelist.h>

/**
 * OSQLError is the base class of all errors
 */
class OSQLError {
public:
    typedef QValueList<OSQLError> ValueList;
    enum Type { None = 0, // NoError
                Internal, // Internal Error in OSQL
                Unknown, // Unknown Error
                Transaction, // Transaction Error
                Statement, // Wrong Statement
                Connection, // Connection Error( lost )
                Driver // Driver Specefic error
    };
    enum DriverError {
        DriverInternal=0, // internal DriverError
        Permission, // Permission Problem
        Abort, // Abort of the SQL
        Busy, // Busy Error
        Locked, // Locked
        NoMem, // No Memory
        ReadOnly, // Database is read only
        Interrupt, // Interrupt
        IOErr, // IO Error
        Corrupt, // Database Corruption
        NotFound, // Table not Found
        Full, // Full
        CantOpen, // Can not open Table/Database
        Protocol, // internal protocol error
        Schema, // schema changed
        TooBig, // Data too big
        Mismatch, // Type mismatch
        Misuse // misuse
    };
    OSQLError( const QString& driverText = QString::null,
               const QString& driverDatabaseText = QString::null,
               int type = None, int subNumber = -1 );
    ~OSQLError();

    QString driverText()const;
    QString databaseText()const;
    int type()const;
    int subNumber()const;
private:
    QString m_drvText;
    QString m_drvDBText;
    int m_type;
    int m_number;
    class OSQLErrorPrivate;
    OSQLErrorPrivate* d;
};

#endif
