#ifndef OSQL_RESULT_H
#define OSQL_RESULT_H

#include <qdatetime.h>
#include <qmap.h>
#include <qvaluelist.h>


#include "osqlerror.h"
/**
 * ResultItem  represents one row of the resulting answer
 */
class OSQLResultItem {
public:
    typedef QValueList<OSQLResultItem> ValueList;
    /**
     * TableString is used to establish the relations
     * between the column name and the real item
     */
    typedef QMap<QString, QString> TableString;

    /**
     * TableInt is used to establish a relation between a
     * position of a column and the row value
     */
    typedef QMap<int, QString> TableInt;

    /**
     * Default c'tor. It has a TableString and a TableInt
     */
    OSQLResultItem(const TableString& = TableString(),
                   const TableInt& = TableInt() );
    OSQLResultItem( const OSQLResultItem& );
    ~OSQLResultItem();
    OSQLResultItem &operator=( const OSQLResultItem& );
    /**
     * returns the TableString
     */
    TableString tableString()const;

    /**
     * returns the TableInt
     */
    TableInt tableInt() const;

    /**
     * retrieves the Data from columnName
     *
     */
    QString data( const QString& columnName, bool *ok = 0);

    /**
     * QString for column number
     */
    QString data(int columnNumber, bool *ok = 0);

    /**
     * Date conversion from columnName
     */
    QDate dataToDate( const QString& columnName, bool *ok = 0 );

    /**
     * Date conversion from column-number
     */
    QDate dataToDate( int columnNumber, bool *ok = 0 );

    QDateTime dataToDateTime( const QString& columName, bool *ok = 0 );
    QDateTime dataToDateTime( int columnNumber, bool *ok = 0 );
private:
    TableString m_string;
    TableInt m_int;
};

/**
 * the OSQLResult
 * either a SQL statement failed or succeeded
 */
class OSQLResult {
public:
    /** The State of a Result */
    enum State{ Success = 0, Failure,Undefined };

    /**
     * default c'tor
     * @param state The State of the Result
     * @param r ResultItems
     * @prarm errors the Errors a OSQLResult created
     */
    OSQLResult( enum State state = Undefined,
                const OSQLResultItem::ValueList& r= OSQLResultItem::ValueList(),
                const OSQLError::ValueList& errors = OSQLError::ValueList() );
    ~OSQLResult();
    State state()const;
    OSQLError::ValueList errors()const;
    OSQLResultItem::ValueList results()const;

    void setState( enum State state );
    void setErrors( const OSQLError::ValueList& error );
    void setResults( const OSQLResultItem::ValueList& result );

    OSQLResultItem first();
    OSQLResultItem next();
    bool atEnd();
    OSQLResultItem::ValueList::ConstIterator iterator()const;
private:
    enum State m_state;
    OSQLResultItem::ValueList m_list;
    OSQLError::ValueList m_error;
    OSQLResultItem::ValueList::Iterator it;
};

#endif
