/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/ 
#include <stdlib.h>
#include <qstring.h>
#include <qheader.h>
#include <qvector.h>
#include <qdatetime.h>
#include <qpe/timestring.h>
#include "common.h"
#include "datacache.h"
#include <assert.h>

static const int del_flag = 0x1;
static const int new_flag = 0x2;

/* Helper function */

int parseNextNumber(QString *q) {
    QChar c;
    uint i;
    int result = 0;

    bool found_digits = FALSE;
    for(i = 0; i < q->length(); i++) {
        c = q->at(i);
        if (c.isDigit()) {
            if (found_digits)
                result *= 10;
            found_digits = TRUE;
            result += c.digitValue();
        } else {
            if (found_digits)
                break;
            /* just skip this char */
        }
    }
    /* now truncate q */
    if (found_digits)
        q->remove(0, i);
    return result;
}

/*!
    \class QStringVector
    \brief A Vector of QStrings that can be sorted and searched

    Implmented in order to allow reverse lookup on the string name

*/

/*!
    This function implements the compare function in order to allow the
    searching and sorting of the QStringVector to occur

    \returns an int which is either
    <UL>
    <LI> < 0 if the first string is smaller than the second,</LI>
    <LI> > 0 if the first string is bigger then the second,</LI>
    <LI> == 0 if the first string is equal to the second.</LI>
    </UL>
*/
int QStringVector::compareItems(Item a, Item b)
{
    QString *qa = (QString *)a;
    QString *qb = (QString *)b;
         
    return QString::compare(*qa, *qb);
}

/*!
    \class TVVariant
    A way of abstracting void * and keeping information on 
    the keytypes and behaviours in one place
*/

TVVariantPrivate::TVVariantPrivate()
{
    typ = TVVariant::Invalid;
}

TVVariantPrivate::TVVariantPrivate( TVVariantPrivate *d)
{
    switch(d->typ)
    {
        case TVVariant::Invalid:
            break;
        case TVVariant::String:
            value.ptr = new QString(*((QString *)d->value.ptr));
            break;
        case TVVariant::Date:
            value.ptr = new QDate(*((QDate *)d->value.ptr));
            break;
        case TVVariant::Time:
            value.ptr = new QTime(*((QTime *)d->value.ptr));
            break;
        case TVVariant::Int:
            value.i = d->value.i;
            break;
        default:
            ASSERT( 0 );
    }

    typ = d->typ;
}

TVVariantPrivate::~TVVariantPrivate()
{
    clear();
}

void TVVariantPrivate::clear()
{
    switch( typ )
    {
        case TVVariant::String:
            delete (QString *)value.ptr;
            break;
        case TVVariant::Date:
            delete (QDate *)value.ptr;
            break;
        case TVVariant::Time:
            delete (QTime *)value.ptr;
            break;
        case TVVariant::Invalid:
        case TVVariant::Int:
            break;
    }

    typ = TVVariant::Invalid;
}

/*!
    \class TVVariant
    blah
*/

TVVariant::TVVariant()
{
    d = new TVVariantPrivate;
}

TVVariant::~TVVariant()
{
    if (d->deref())
        delete d;
}

TVVariant::TVVariant(const TVVariant& p)
{
    d = new TVVariantPrivate;
    *this = p;
}

TVVariant::TVVariant(QDataStream& s)
{
    d = new TVVariantPrivate;
    s >> *this;
}

TVVariant::TVVariant(const QString &val)
{
    d = new TVVariantPrivate;
    d->typ = String;
    d->value.ptr = new QString(val);
}

TVVariant::TVVariant(const QDate &val)
{
    d = new TVVariantPrivate;
    d->typ = Date;
    d->value.ptr = new QDate(val);
}

TVVariant::TVVariant(const QTime &val)
{
    d = new TVVariantPrivate;
    d->typ = Time;
    d->value.ptr = new QTime(val);
}

TVVariant::TVVariant( int val )
{
    d = new TVVariantPrivate;
    d->typ = Int;
    d->value.i = val;
}

TVVariant& TVVariant::operator=(const TVVariant& variant )
{
    TVVariant& other = (TVVariant&) variant;

    other.d->ref();
    if ( d->deref() )
    delete d;

    d = other.d;

    return *this;
}

void TVVariant::detach()
{
    if (d->count == 1)
    return;

    d->deref();
    d = new TVVariantPrivate(d);
}

const QString TVVariant::typeName() const
{
    return typeToName(d->typ);
}

void TVVariant::clear()
{
    if (d->count > 1)
    {
        d->deref();
        d = new TVVariantPrivate;
        return;
    }

    d->clear();
}

const QString TVVariant::typeToName(KeyType typ)
{
    switch(typ) {
        case String:
            return QString("String");
        case Date:
            return QString("Date");
        case Time:
            return QString("Time");
        case Int:
            return QString("Int");
        case Invalid:
        default:
            return QString("Invalid");
    }
    return QString("Invalid");
}

TVVariant::KeyType TVVariant::nameToType(const QString &name)
{
    if(!qstrcmp("String", name))
        return String;
    if(!qstrcmp("Date", name))
        return Date;
    if(!qstrcmp("Time", name))
        return Time;
    if(!qstrcmp("Int", name))
        return Int;

    return Invalid;
}

void TVVariant::load(QDataStream &s )
{
    KeyType t;
    s >> t;

    d->typ = t;
    switch(t) {
        case Invalid:
            d->typ = t;
            break;
        case String:
            {
                QString *x = new QString;
                s >> *x;
                d->value.ptr = x;
            }
            break;
        case Time:
            {
                QTime *x = new QTime;
                s >> *x;
                d->value.ptr = x;
            }
            break;
        case Date:
            {
                QDate *x = new QDate;
                s >> *x;
                d->value.ptr = x;
            }
            break;
        case Int:
            {
                int x;
                s >> x;
                d->value.i = x;
            }
      break;
        default:
      qFatal("Unrecognized data type");
    }
}

void TVVariant::save( QDataStream &s ) const
{
    s << type();

    switch( d->typ ) {
        case String:
            s << *((QString *)d->value.ptr);
            break;
        case Date:
            s << *((QDate *)d->value.ptr);
            break;
        case Time:
            s << *((QTime *)d->value.ptr);
            break;
        case Int:
            s << d->value.i;
            break;
        case Invalid:
            break;
    }
}

QDataStream& operator>>(QDataStream& s, TVVariant& p)
{
    p.load( s );
    return s;
}

QDataStream& operator<<(QDataStream &s, const TVVariant& p)
{
    p.save( s );
    return s;
}

QDataStream& operator>> (QDataStream &s, TVVariant::KeyType& p)
{
    Q_UINT8 u = 0;
    s >> u;
    p = (TVVariant::KeyType) u;

    return s;
}

QDataStream& operator<< (QDataStream& s, const TVVariant::KeyType& p)
{
    s << (Q_UINT8)p;
    return s;
}

const QString TVVariant::toString() const
{
    switch(d->typ) {
        case String:
            return *((QString*)d->value.ptr);
        case Date:
            return ((QDate*)d->value.ptr)->toString();
        case Time:
            return ((QTime*)d->value.ptr)->toString();
        case Int: 
                return QString::number(d->value.i);
        case Invalid:
        default:
            return QString::null;
    }
    return QString::null;
}

// TODO DO, this properly, */
int TVVariant::toInt() const
{
    if(d->typ == Int)
        return d->value.i;

    if(d->typ == String) {
      QString tmpq(*(QString *)d->value.ptr);
        return parseNextNumber(&tmpq);
    }

    return 0;
}

const QDate TVVariant::toDate() const
{
    if(d->typ == Date)
        return *((QDate *)d->value.ptr);

    if(d->typ == String) {
        QString q = toString();

        /* date format is day mon d yyyy */
        /* ignore the first three letters, read the next
           three for month.. etc */

        int day = parseNextNumber(&q);
        int month = parseNextNumber(&q);
        int year = parseNextNumber(&q);
        if (!QDate::isValid(year, month, day))
            return QDate();
        return QDate(year, month, day);
    }
        

    return QDate();
}

const QTime TVVariant::toTime() const
{
    if(d->typ == Time)
        return *((QTime *)d->value.ptr);
    
    if(d->typ == String) {
        QString q = toString();
        int hour = parseNextNumber(&q);
        int minute = parseNextNumber(&q);
        int second = parseNextNumber(&q);
        int msecond = parseNextNumber(&q);
        if (!QTime::isValid(hour, minute, second, msecond))
            return QTime();
        return QTime(hour, minute, second, msecond);
    }

    return QTime();
}

#define TV_VARIANT_AS( f ) Q##f& TVVariant::as##f() { \
    if ( d->typ != f ) \
        *this = TVVariant( to##f() ); \
    else \
        detach(); \
    return *((Q##f*)d->value.ptr); }
 
TV_VARIANT_AS(String)
TV_VARIANT_AS(Date)
TV_VARIANT_AS(Time)

#undef TV_VARIANT_AS

int& TVVariant::asInt()
{
    detach();
    if (d->typ != Int) {
        d->value.i = toInt();
        d->typ = Int;
    }
    return d->value.i;
}

/*!
    valid cast is
    anything to String
    same to same
*/
bool TVVariant::canCast(KeyType t) const
{
    if(d->typ == t)
        return TRUE;

    if(t == String)
        return TRUE;

    if(t == Int) {
        if (d->typ == Date)
            return TRUE;
        if (d->typ == Time)
            return TRUE;
        if (d->typ == String)
            return TRUE;
    }

    return FALSE;
}

bool TVVariant::operator==( const TVVariant &v ) const
{
    switch(d->typ) {
        case String:
            return v.toString() == toString();
        case Date:
            return v.toDate() == toDate();
        case Time:
            return v.toTime() == toTime();
        case Int:
            return v.toInt() == toInt();
        case Invalid:
            break;
    }

    return FALSE;
}

bool TVVariant::operator!=( const TVVariant &v ) const
{
    return !( v == *this);
}

bool TVVariant::operator<( const TVVariant &v ) const
{
    switch(d->typ) {
        case String:
            return toString().lower() < v.toString().lower();
        case Date:
            return toDate() < v.toDate();
        case Time:
            return toTime() < v.toTime();
        case Int:
            return toInt() < v.toInt();
        case Invalid:
        default:
            break;
    }
    return FALSE;
}

bool TVVariant::operator>( const TVVariant &v ) const
{
    switch(d->typ) {
        case String:
            return toString().lower() > v.toString().lower();
        case Date:
            return toDate() > v.toDate();
        case Time:
            return toTime() > v.toTime();
        case Int:
            return toInt() > v.toInt();
        case Invalid:
        default:
            break;
    }
    return FALSE;
}

/*! True if n is closer to this than o */
bool TVVariant::closer(TVVariant n, TVVariant o)
{
    /* Nothing is close to an invalid, so nothing can be closer */
    if(d->typ == Invalid)
        return FALSE; 

    /* can't be closer if of different type */
    if(n.type() != type())
        return FALSE;

    /* if new shares type, and old doesn't, then new is closer */
    if(o.type() != type())
        return TRUE;

    switch(type()){
        case String: {
            /* case for strings is close is a substring.. closer is 
             * earlier alphabetically */
            QString qs1 = n.toString().lower();
            QString qs2 = o.toString().lower();
            QString qsv = toString().lower();

            if (!qs1.startsWith(qsv))
                return FALSE;

            /* contains sub-str, if later than is not closer */
            if(QString::compare(qs1, qs2) > 0) 
                return FALSE;
            return TRUE;
        }
        case Int: {
            /* case for int is smallest absolute difference */
            int i1 = n.toInt();
            int i2 = o.toInt();
            int iv = toInt();

            int diff1 = (i1 - iv);
            if (diff1 < 0) 
                diff1 = -diff1;
            int diff2 = (i2 - iv);
            if (diff2 < 0) 
                diff2 = -diff2;

            if (diff1 < diff2) 
                return TRUE;
            return FALSE;
        }
        case Date: {
            QDate i1 = n.toDate();
            QDate i2 = o.toDate();
            QDate iv = toDate();

            /* definition of closer is the least difference in days */
            int diff1 = i1.daysTo(iv);
            if (diff1 < 0) 
                diff1 = -diff1;
            int diff2 = i2.daysTo(iv);
            if (diff2 < 0) 
                diff2 = -diff2;

            if (diff1 < diff2)
                return TRUE;
            return FALSE;
        }
        case Time: {
            QTime i1 = n.toTime();
            QTime i2 = o.toTime();
            QTime iv = toTime();

            /* definition of closer is the least difference in days */
            int diff1 = i1.msecsTo(iv);
            if (diff1 < 0) 
                diff1 = -diff1;
            int diff2 = i2.msecsTo(iv);
            if (diff2 < 0) 
                diff2 = -diff2;
            if (diff1 < diff2)
                return TRUE;
            return FALSE;
        }
        default:
            /* don't know how to do 'closer' on this type, hence never closer
             * or even close */
            break;
    }
    return FALSE;
}

/*! True if n is close to this */
bool TVVariant::close(TVVariant n)
{
    /* Nothing is close to an invalid, so nothing can be closer */
    if(type() == Invalid)
        return FALSE; 

    /* can't be close if of different type */
    if(n.type() != type())
        return FALSE;

    switch(type()){
        case String: {
            /* case for strings is close is a substring.. closer is 
             * earlier alphabetically */
            QString qs1 = n.toString().lower();
            QString qsv = toString().lower();

            if (!qs1.startsWith(qsv))
                return FALSE;
            return TRUE;
        }
        case Int: 
        case Date:
        case Time:
            return TRUE;
        default:
            /* don't know how to do 'closer' on this type, hence never closer
             * or even close */
            break;
    }
    return FALSE;
}

/*!
    \class Key
    \brief document me!
    
    document me!
*/

Key::Key() : kname(), kexample(), kflags(0) { }

Key::Key(QString name, TVVariant example, int flags) :
    kname(name), kexample(example), kflags(flags) { } 

Key::Key(const Key &other)
{
  kname = other.kname;
  kexample = other.kexample;
  kflags = other.kflags;
}

Key& Key::operator=(const Key& key) 
{
    kname = key.kname;
    kexample = key.kexample;
    kflags = key.kflags;
    return *this;
}

QString Key::name() const
{
    return QString(kname);
}

TVVariant Key::example() const
{
    return TVVariant(kexample);
}

TVVariant::KeyType Key::type() const
{
    return kexample.type();
}

void Key::setName(const QString &name)
{
    kname = QString(name);
}

void Key::setExample(const TVVariant &e)
{
    kexample = TVVariant(e);
}

int Key::flags() const
{
    return kflags;
}

void Key::setFlags(int fl)
{
    kflags = fl;
}

bool Key::delFlag() const
{
    if(kflags & del_flag)
        return TRUE;
    return FALSE;
}

bool Key::newFlag() const
{
    if(kflags & new_flag)
        return TRUE;
    return FALSE;
}

void Key::setDelFlag(bool v)
{
    if(delFlag() != v)
        kflags = kflags ^ del_flag;
}

void Key::setNewFlag(bool v)
{
    if(newFlag() != v)
        kflags = kflags ^ new_flag;
}

/*!
    \class KeyList
    \brief A represntation of keys used for a table.

    The KeyList class is used to store the representation of keys used in table
    headings by DBStore.  It stores the names and types of the keys
*/

/*!
    Constructs a KeyList
*/
KeyList::KeyList() : QIntDict<Key>(20) 
{
    setAutoDelete(TRUE);
}

/* Should be deep copy, but isn't */
KeyList::KeyList(const KeyList &k) : QIntDict<Key>(k) 
{
    KeyListIterator it(k);
  while(it.current()) {
    replace(it.currentKey(), new Key(*it.current()));
    ++it;
  }

    setAutoDelete(TRUE);
}

/*!
    Destroys a KeyList
*/
KeyList::~KeyList() {
}

/* Do a comparision base on Keys */
bool KeyList::operator!=(const KeyList &other)
{
    KeyListIterator it(*this);

    if (other.getNumFields() != getNumFields())
  return TRUE;

    while(it.current()) {
  //it.currentKey(), it.current();
  if (other.getKeyName(it.currentKey()) != getKeyName(it.currentKey()))
      return TRUE;
  if (other.getKeyType(it.currentKey()) != getKeyType(it.currentKey()))
      return TRUE;
  ++it;
    }
    return FALSE;
}

/*!
    Returns the number of keys stored in the KeyList
*/
int KeyList::getNumFields() const 
{
    return count();
}

/*!
    Adds a new key to the KeyList

    \param name the name of the new key
    \param type the type of the new key
*/
int KeyList::addKey(QString name, TVVariant example)
{
    int i = count();
    while(find(i) && (i > -1))
        i--;
    replace(i, new Key(name, example, 0));
    return i;
}

int KeyList::addKey(QString name, TVVariant::KeyType type)
{
    /* generate a valid type for the example? */
    TVVariant e = TVVariant("0");
    switch(type) {
        case TVVariant::String: 
            return addKey(name, TVVariant("<undefined>").asString());
            break;
        case TVVariant::Date: 
            return addKey(name, TVVariant(QDate::currentDate()).asDate());
            break;
        case TVVariant::Time: 
            return addKey(name, TVVariant(QTime(0,0,0)).toTime());
            break;
        case TVVariant::Int: 
            return addKey(name, TVVariant(0).toInt());
            break;
        default:
            qWarning(QObject::tr("KeyList::addKey() Cannot make default "
                        "value for type %1, Key not added.").arg(type));
            break;
    }
    return -1;
}

void KeyList::setKeyFlags(int i, int flag) 
{
    if(find(i))
        find(i)->setFlags(flag);
}

int KeyList::getKeyFlags(int i) const
{
    if(find(i))
        return find(i)->flags();
    return 0;
}

bool KeyList::checkNewFlag(int i) const 
{
    if (find(i))
        return find(i)->newFlag();
    return false;
}

void KeyList::setNewFlag(int i, bool f) 
{
    if(!find(i))
        return;
    find(i)->setNewFlag(f);
}

bool KeyList::checkDeleteFlag(int i) const 
{
    if (find(i))
        return find(i)->delFlag();
    return false;
}

void KeyList::setDeleteFlag(int i, bool f) 
{
    if(!find(i))
        return;
    find(i)->setDelFlag(f);
}

/*!
    Returns the name of the key at index i
*/
QString KeyList::getKeyName(int i) const
{
    if (find (i))
        return find(i)->name();
    return QString();
}

void KeyList::setKeyName(int i, const QString &n) 
{
    if(find(i))
        find(i)->setName(n);
}

/*!
    Returns the type of the key at index i
*/
TVVariant::KeyType KeyList::getKeyType(int i) const
{
    if(find(i))
        return find(i)->type();
    return TVVariant::Invalid;
}

void KeyList::setKeyType(int i, TVVariant::KeyType t) 
{
    if(!find(i))
        return;
    switch(t) {
        case TVVariant::String:
            find(i)->setExample(TVVariant(QString("default")));
            return;
        case TVVariant::Int:
            find(i)->setExample(TVVariant(int(0)));
            return;
        case TVVariant::Date:
            find(i)->setExample(TVVariant(QDate::currentDate()));
            return;
        case TVVariant::Time:
            find(i)->setExample(TVVariant(QTime(0,0,0,0)));
            return;
        default:
            break;
    }
    return;
}

TVVariant KeyList::getKeyExample(int i) const
{
    if(find(i))
        return find(i)->example();
    return TVVariant();
}

void KeyList::setKeyExample(int i, TVVariant example)
{
    if(find(i))
        find(i)->setExample(example);
}

/*!
    Returns the index of the key with name q
*/
int KeyList::getKeyIndex(QString q) const
{
    KeyListIterator it(*this);

    while(it.current()) {
        if(it.current()->name() == q)
            return it.currentKey();
        ++it;
    }
    return -1;
}

bool KeyList::validIndex(int i) const
{
    if(!find(i))
        return FALSE;
    if(find(i)->delFlag())
        return FALSE;
    return TRUE;
}

QDataStream &operator<<( QDataStream &s, const KeyList &k)
{
    s << k.getNumFields();

    KeyListIterator it(k);

    while(it.current()) {
        s << (Q_UINT16)it.currentKey();
        s << it.current()->name();
        s << it.current()->example();
        s << (Q_UINT16)it.current()->flags();
        ++it;
    }
    return s;
}

QDataStream &operator>>( QDataStream &s, KeyList &k)
{
    int i;
    int size;
    int index = 0;
    int flags = 0;
    TVVariant type = TVVariant();
    QString name;

    s >> size;

    for (i=0; i < size; i++) {
        s >> (Q_UINT16 &)index;
        s >> name;
        s >> type;
        s >> (Q_UINT16 &)flags;
        k.replace(index, new Key(name, type, flags));
    }
    return s;
}

/*!
    \class DataElem
    \brief A class representing a single row or element of a table in a DBStore

    This class holds the data of a row in a table.
*/


/*!
    Constructs a DataElem.  This function needs a container because the 
    size, types of keys and primary key are all defined by the containing 
    database 
*/
DataElem::DataElem(DBStore *c) : values(20)
{
    int size;
    contained = c;
    size = c->getNumFields();
    values.setAutoDelete(TRUE);
}

/*!
    Destroys a DataElem and frees memory used by the DataElem
*/
DataElem::~DataElem() {
}



QDataStream &operator<<( QDataStream &s, const DataElem &d)
{
    int size = d.getNumFields();

    s << size; /* redundent data but makes streaming easier */
    KeyList k = d.getKeys();

    KeyListIterator it(k);

    while(it.current()) {
        s << (Q_UINT16)it.currentKey();
        s << d.getField(it.currentKey());
        ++it;
    }
    return s;
}

QDataStream &operator>>( QDataStream &s, DataElem &d)
{
    int i;
    int size;
    TVVariant t;
    int index = 0;

    s >> size; /* redundent data but makes streaming easier */
    if (size != d.getNumFields()) {
        qWarning("DataSize mis-match");
        return s; /* sanity check failed.. don't load */
    }

    for(i = 0; i < size; i++) {
        s >> (Q_UINT16)index;
        s >> t;
        d.setField(index, t);
    }
    return s;
}

/*! Returns the number of possible (not valid) fields in the data element */
int DataElem::getNumFields() const
{
    return contained->getNumFields();
}

KeyList DataElem::getKeys() const
{
    return *(contained->getKeys());
}

/*! 
    This function determines whether field index i of the element has been
    set yet.

    \return A boolean value that is TRUE if the specfied field of this 
    element has been set and FALSE if the field has not yet been set 
*/
bool DataElem::hasValidValue(int i) const
{
    if(!values.find(i))
        return FALSE;
    if(!contained->getKeys()->validIndex(i)) 
        return FALSE;
    return values.find(i)->isValid();
}

/*! 
    This function determines whether field name qs of the element has been
    set yet.

    \return A boolean value that is TRUE if the specfied field of this 
    element has been set and FALSE if the field has not yet been set 
*/
bool DataElem::hasValidValue(QString qs) const 
{
    int i = contained->getKeyIndex(qs);
    return hasValidValue(i);
}

/*!  returns the type of the field specified by index i */
TVVariant::KeyType DataElem::getFieldType(int i) const 
{
    return contained->getKeyType(i);
}

/*!  returns the type of the field specified by name qs */
TVVariant::KeyType DataElem::getFieldType(QString qs) const
{
    int i = contained->getKeyIndex(qs);
    return contained->getKeyType(i);
}

/*!  
    returns a pointer to the data stored in field index i for this 
    data element, (value may not be valid) 
*/
TVVariant DataElem::getField(int i) const
{
    if(hasValidValue(i))
        return TVVariant(*values.find(i));
    return TVVariant();
}

/*!  
    returns a pointer to the data stored in field name qs for this 
    data element, (value may not be valid) 
*/
TVVariant DataElem::getField(QString qs) const
{
    int i = contained->getKeyIndex(qs);
    return getField(i);
}

/*!
    Sets the value of the elements field index i to the value represented in
    the QString q.

    \param i index of the field to set
    \param q a string that can be parsed to get the value to be set
*/
void DataElem::setField(int i, QString q) 
{
    /* from the type of the field, parse q and store */
    TVVariant::KeyType kt = contained->getKeyType(i);

    TVVariant t = TVVariant(q);

    switch(kt) {
        case TVVariant::Int: {
            t.asInt();
            setField(i, t);
      return;
        }
        case TVVariant::String: {
            t.asString();
            setField(i, t);
            return;
        }
        case TVVariant::Date: {
            t.asDate();
            setField(i, t);
            return;
        }
        case TVVariant::Time: {
            t.asTime();
            setField(i, t);
            return;
        }
        default:
            qWarning(
                QObject::tr("DataElem::setField(%1, %2) No valid type found").arg(i).arg(q)
                );
    }
}

/*!
    Sets the value of the elements field index i to the value at the pointer
    value.

    \param i index of the field to set
    \param value a pointer to the (already allocated) value to set 
*/
void DataElem::setField(int i, TVVariant value) 
{
    if (value.isValid()) {
        values.remove(i);
        values.replace(i, new TVVariant(value));
    }
}

/*!
    Sets the value of the elements field name qs to the value represented in
    the QString q.

    \param qs name of the field to set
    \param q a string that can be parsed to get the value to be set
*/
void DataElem::setField(QString qs, QString q) 
{
    /* from the type of the field, parse q and store */
    int i = contained->getKeyIndex(qs);
    setField(i, qs);
}

/*!
    Sets the value of the elements field name qs to the value at the pointer
    value.

    \param qs name of the field to set
    \param value a pointer to the (already allocated) value to set 
*/
void DataElem::setField(QString qs, TVVariant value)
{
    int i = contained->getKeyIndex(qs);
    setField(i, value);
}

void DataElem::unsetField(int i) {
    values.remove(i);
}

void DataElem::unsetField(QString qs)
{
    int i = contained->getKeyIndex(qs);
    unsetField(i);
}

/*!
    Converts the data element to a Rich Text QString 
*/
QString DataElem::toQString() const
{
    /* lets make an attempt at this function */
    int i;
    QString scratch = "";

    QIntDictIterator<TVVariant> it(values);

    while (it.current()) {
      i = it.currentKey();
        if(hasValidValue(i)) {
            scratch += "<B>" + contained->getKeyName(i) + ":</B> ";
            scratch += getField(i).toString();
            scratch += "<br>";
        }
        ++it;
    }
    return scratch;
}

/*! formats individual fields to strings so can be displayed */
QString DataElem::toQString(int i) const 
{
    if(hasValidValue(i)) {
        return getField(i).toString();
    }
    return "";
}
/*! formats individual fields to strings so can be sorted by QListView */
QString DataElem::toSortableQString(int i) const 
{
    QString scratch = "";
    if(hasValidValue(i)) {
        switch (contained->getKeyType(i)) {
            case TVVariant::String: {
                scratch += getField(i).toString();
                break;
            }
            case TVVariant::Int: {
                scratch.sprintf("%08d", getField(i).toInt());
                break;
            }
            case TVVariant::Date: {
                static QDate epochD(1800, 1, 1);
                scratch.sprintf("%08d", 
                                epochD.daysTo(getField(i).toDate()));
                break;
            }
            case TVVariant::Time: {
                static QTime epochT(0, 0, 0);
                scratch.sprintf("%08d", 
                                epochT.msecsTo(getField(i).toTime()));
                break;
            }
            default:
                scratch += "Unknown type";
                break;
        }
    }
    return scratch;
}

/* compare functions */

bool DataElem::lessThan(int i, TVVariant v) const
{
    if (!hasValidValue(i)) return FALSE;

    if (getField(i).type() != v.type())
        return FALSE;

    return (getField(i) < v);
}

bool DataElem::moreThan(int i, TVVariant v) const
{
    if (!hasValidValue(i)) return FALSE;

    if (getField(i).type() != v.type())
        return FALSE;

    return (getField(i) > v);
}

bool DataElem::equalTo(int i, TVVariant v) const
{
    if (!hasValidValue(i)) return FALSE;

    if (getField(i).type() != v.type())
        return FALSE;

    return (getField(i) == v);
}

bool DataElem::contains(int i, TVVariant v) const
{
    if (!hasValidValue(i)) return FALSE;

    if (getField(i).type() != v.type())
        return FALSE;

    switch(getField(i).type()) {
        case TVVariant::String: {
            QString qs1 = getField(i).toString().lower();
            QString qs2 = v.toString().lower();
            if (qs1.contains(qs2) > 0) return TRUE;
            break;
        }
            /* meaningless for ints */
            /* meaningless for time */
            /* meaningless for dates */
        case TVVariant::Int: 
        case TVVariant::Time:
        case TVVariant::Date:
            break;
        default:
            qWarning("Tried to compare unknown data type");
    }
    return FALSE;
}

bool DataElem::startsWith(int i, TVVariant v) const
{
    if (!hasValidValue(i)) return FALSE;

    if (getField(i).type() != v.type())
        return FALSE;

    switch(getField(i).type()) {
        case TVVariant::String: {
            QString qs1 = getField(i).toString().lower();
            QString qs2 = v.toString().lower();
            return qs1.startsWith(qs2);
        }
            /* meaningless for ints */
            /* meaningless for time */
            /* meaningless for dates */
        case TVVariant::Int: 
        case TVVariant::Time:
        case TVVariant::Date:
            return FALSE;
        default:
            qWarning("Tried to compare unknown data type");
    }
    return FALSE;
}

bool DataElem::endsWith(int i, TVVariant v) const
{
    if (!hasValidValue(i)) return FALSE;

    if (getField(i).type() != v.type())
        return FALSE;

    switch(getField(i).type()) {
        case TVVariant::String: {
            QString qs1 = getField(i).toString().lower();
            QString qs2 = v.toString().lower();
            return qs1.startsWith(qs2);
        }
            /* meaningless for ints */
            /* meaningless for time */
            /* meaningless for dates */
        case TVVariant::Int: 
        case TVVariant::Time:
        case TVVariant::Date:
            return FALSE;
        default:
            qWarning("Tried to compare unknown data type");
    }
    return FALSE;
}

/*!
    Determins which of the first to parameters are closer to the third, target
    parameter.

    \return 
    <UL>
    <LI>TRUE if the first element is a closer match to the target than the 
    second element</LI>
    <LI>FALSE if the first element is not a closer match to the target than
    the second element</LI>
    </UL>
*/
bool DataElem::closer(DataElem*d1, DataElem *d2, TVVariant target, int column)
{
    int type; 

    if(!d1) return FALSE;

    if (!d1->hasValidValue(column)) return FALSE;

    if(!target.isValid()) return FALSE;

    type = d1->getField(column).type();

    if(d2) {
        if (type != d2->getField(column).type()) {
                /* can't do compare */
                qWarning("Tried to compare two incompatable types");
                return FALSE;
        }
        return target.closer(d1->getField(column), d2->getField(column));
    } 
    return target.close(d1->getField(column));
}
