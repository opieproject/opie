#include "osqltable.h"

OSQLTableItem::OSQLTableItem() {}
OSQLTableItem::OSQLTableItem( enum Type type,
                              const QString& field,
                              const QVariant& var)
    : m_type( type ), m_field( field ), m_var( var )
{

}
OSQLTableItem::~OSQLTableItem() {}
OSQLTableItem::OSQLTableItem( const OSQLTableItem& item) {
    *this = item;
}
OSQLTableItem &OSQLTableItem::operator=(const OSQLTableItem& other) {
    m_var = other.m_var;
    m_field = other.m_field;
    m_type = other.m_type;
    return *this;
}
QString OSQLTableItem::fieldName()const{
    return m_field;
}
OSQLTableItem::Type OSQLTableItem::type()const {
    return m_type;
}
QVariant OSQLTableItem::more()const {
    return m_var;
}

OSQLTable::OSQLTable( const QString& tableName )
    : m_table( tableName )
{

}
OSQLTable::~OSQLTable() {
}
void OSQLTable::setColumns( const OSQLTableItem::ValueList& list) {
    m_list = list;
}
OSQLTableItem::ValueList OSQLTable::columns()const {
    return m_list;
}
QString OSQLTable::tableName()const {
    return m_table;
}
