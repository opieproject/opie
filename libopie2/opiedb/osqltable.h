#ifndef OSQL_TABLE_H
#define OSQL_TABLE_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qvariant.h>

/**
 * OSQLTableItem saves one column of a complete
 * table
 */
class OSQLTableItem {
public:
    typedef QValueList<OSQLTableItem> ValueList;
    /**
     * Type kinds ( to be extended )
     */
    enum Type { Undefined=-1, Integer=0, BigInteger =1,
                Float = 2, VarChar = 4 };
    /**
     * A constructor
     * @param type the Type of the Column
     * @param fieldName the Name of the Column
     * @param var a Variant
     */
    OSQLTableItem();
    OSQLTableItem( enum Type type,
                   const QString& fieldName,
                   const QVariant& var= QVariant() );

    /**
     * copy c'tor
     */
    OSQLTableItem( const OSQLTableItem& );

    /**
     * d'tor
     */
    ~OSQLTableItem();

    OSQLTableItem& operator=( const OSQLTableItem& );

    /**
     * the fieldName
     */
    QString fieldName() const;

    /**
     * the field Type
     */
    Type type() const;
    QVariant more() const;
private:
    class OSQLTableItemPrivate;
    OSQLTableItemPrivate* d;
    Type m_type;
    QString m_field;
    QVariant m_var;
};

/**
 * A OSQLTable consists of OSQLTableItems
 */
class OSQLTable  {
public:
    typedef QValueList<OSQLTable> ValueList;

    /**
     * @param tableName the Name of the Table
     */
    OSQLTable(const QString& tableName);

    /**
     * d'tor
     */
    ~OSQLTable();

    /**
     * setColumns sets the Columns of the Table
     */
    void setColumns( const OSQLTableItem::ValueList& );

    /**
     * returns all columns of the table
     */
    OSQLTableItem::ValueList columns() const;

    QString tableName()const;

private:
    QString m_table;
    OSQLTableItem::ValueList m_list;
};

#endif
