
#include "osqlquery.h"
#include "osqlresult.h"

OSQLResultItem::OSQLResultItem( const TableString& string,
                                const TableInt& Int)
    : m_string( string ), m_int( Int )
{

}
OSQLResultItem::~OSQLResultItem() {
}
OSQLResultItem::OSQLResultItem( const OSQLResultItem& item) {
    *this = item;
}
OSQLResultItem &OSQLResultItem::operator=( const OSQLResultItem& other) {
    m_string = other.m_string;
    m_int = other.m_int;
    return *this;
}
OSQLResultItem::TableString OSQLResultItem::tableString()const{
    return m_string;
}
OSQLResultItem::TableInt OSQLResultItem::tableInt()const {
    return m_int;
}
QString OSQLResultItem::data( const QString& columnName, bool *ok ) {
    TableString::Iterator it = m_string.find( columnName );

    /* if found */
    if ( it != m_string.end() ) {
        if ( ok ) *ok = true;
        return it.data();
    }else{
        if ( ok )  *ok = false;
        return QString::null;
    }

}
QString OSQLResultItem::data( int column, bool *ok ) {
    TableInt::Iterator it = m_int.find( column );

    /* if found */
    if ( it != m_int.end() ) {
        if ( ok ) *ok = true;
        return it.data();
    }else{
        if ( ok )  *ok = false;
        return QString::null;
    }
}
/*
 * DateFormat is 'YYYY-MM-DD'
 */
QDate OSQLResultItem::dataToDate( const QString& column, bool *ok ) {
    QDate date = QDate::currentDate();
    QString str = data( column,  ok );
    if (!str.isEmpty() ) {
      ;// convert
    }
    return date;
}
QDate OSQLResultItem::dataToDate( int column,  bool *ok ) {
    QDate date = QDate::currentDate();
    QString str = data( column,  ok );
    if (!str.isEmpty() ) {
      ;// convert
    }
    return date;

}
QDateTime OSQLResultItem::dataToDateTime( const QString& column, bool *ok ) {
    QDateTime time = QDateTime::currentDateTime();
    return time;
}
QDateTime OSQLResultItem::dataToDateTime( int column, bool *ok ) {
    QDateTime time = QDateTime::currentDateTime();
    return time;
}

OSQLResult::OSQLResult( enum State state,
                        const OSQLResultItem::ValueList& list,
                        const OSQLError::ValueList& error )
    : m_state( state ), m_list( list ), m_error( error )
{

}
OSQLResult::~OSQLResult() {

}
OSQLResult::State OSQLResult::state()const {
    return m_state;
}
void OSQLResult::setState( OSQLResult::State state ) {
    m_state = state;
}
OSQLError::ValueList OSQLResult::errors()const {
    return m_error;
}
void OSQLResult::setErrors( const OSQLError::ValueList& err ) {
    m_error = err;
}
OSQLResultItem::ValueList OSQLResult::results()const {
    return m_list;
}
void OSQLResult::setResults( const OSQLResultItem::ValueList& result ) {
    m_list = result;
}
OSQLResultItem OSQLResult::first() {
    it = m_list.begin();
    return (*it);
}
OSQLResultItem OSQLResult::next(){
    ++it;
    return (*it);
}
bool OSQLResult::atEnd(){
    if ( it == m_list.end() )
        return true;

    return false;
}
OSQLResultItem::ValueList::ConstIterator OSQLResult::iterator()const {
    OSQLResultItem::ValueList::ConstIterator it;
    it = m_list.begin();
    return it;
}
