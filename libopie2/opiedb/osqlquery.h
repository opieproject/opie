
#ifndef OSQL_QUERY_H
#define OSQL_QUERY_H

#include <qmap.h>
#include <qvaluelist.h>
#include <qstring.h>


/** I'm not happy with them
class OSQLQueryOrder {
public:
    typedef QValueList<OSQLQueryOrder> ValueList;
    OSQLQueryOrder(const QString& table = QString::null );
    OSQLQueryOrder( const OSQLQueryOrder& );
    ~OSQLQueryOrder();
    void setOrderFields( const QStringList& list );
    void setValue( const QString& fieldName, const QString& Value );
    void setValues( const QMap<QString, QString>& );
    QMap<QString, QString> orders() const;
    QString orderStatement()const;
    OSQLQueryOrder &operator=(const OSQLQueryOrder& );
private:
    QMap<QString, QString> m_fields;
    class OSQLQueryOrderPrivate;
    OSQLQueryOrderPrivate* d;
};
class OSQLWhere {
public:
    typedef QValueList<OSQLWhere> ValueList;
    enum Vergleiche { Equal = 0, Like, Greater, GreaterEqual,
                      Smaller, SmallerEqual };
    OSQLWhere(const QString& table = QString::null );
    ~OSQLWhere();
    void setExpression(const QString& key, enum Vergleiche, const QString& );
    QString statement()const;
private:
    int m_vergleich;
    const QString& m_left;
    const QString& m_right;

};
*/
class OSQLQuery {
public:
    OSQLQuery();
    virtual ~OSQLQuery();

    virtual QString query()const = 0;
};

class OSQLRawQuery : public OSQLQuery {
public:
    OSQLRawQuery( const QString& query );
    ~OSQLRawQuery();
    QString query() const;
private:
    class OSQLRawQueryPrivate;
    OSQLRawQueryPrivate* d;
    QString m_query;

};
/* I'm not happy with them again
class OSQLSelectQuery : public OSQLQuery {
public:
    OSQLSelectQuery();
    ~OSQLSelectQuery();
    void setTables( const QStringList& allTablesToQuery );
    void setValues( const QString& table, const QStringList& columns );
    void setOrder( const OSQLSelectQuery& );
    void setOrderList( const OSQLQueryOrder::ValueList& );
    void setWhereList( const OSQLWhere& );
    void setWhereList( const OSQLWhere::ValueList& );
    QString query()const;
private:
    QStringList m_tables;
    QMap<QString, QStringList> m_values;
    OSQLQueryOrder::ValueList m_order;
    OSQLWhere::ValueList m_where;
    class OSQLSelectQueryPrivate;
    OSQLSelectQueryPrivate* d;
};
class OSQLInsertQuery : public OSQLQuery  {
public:
    OSQLInsertQuery(const QString& table);
    ~OSQLInsertQuery();
    void setInserFields( const QStringList& );
    void setValue( const QString& field,  const QString& value );
    void setValues(const QMap<QString, QString>& );
    QString query()const;
private:
    QString m_table;
    QStringList m_fields;
    QMap<QString, QString> m_values;
};
class OSQLDeleteQuery : public OSQLQuery {
public:
    OSQLDeleteQuery(const QString& table);
    ~OSQLDeleteQuery();
    void setWhere( const OSQLWhere& );
    void setWheres( const OSQLWhere::ValueList& );
    QString query()const;
private:
    QString m_table;
    OSQLWhere::ValueList m_where;

};
class OSQLUpdateQuery : public OSQLQuery {
public:
    OSQLUpdateQuery( const QString& table );
    ~OSQLUpdateQuery();
    void setWhere( const OSQLWhere& );
    void setWheres( const OSQLWhere::ValueList& );
    */
    /* replaces all previous set Values */
    /*
    void setValue( const QString& field, const QString& value );
    void setValue( const QMap<QString, QString> &fields );
    QString query() const;
};
*/
#endif
