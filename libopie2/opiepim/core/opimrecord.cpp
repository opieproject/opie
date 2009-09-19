/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "opimrecord.h"

/* OPIE */
#include <qpe/categories.h>
#include <qpe/categoryselect.h>
#include <opie2/odebug.h>

/* QT */

namespace Opie
{
Qtopia::UidGen OPimRecord::m_uidGen( Qtopia::UidGen::Qtopia );

QString OPimRecord::actionToStr( ChangeAction action )
{
    if( action == ACTION_REMOVE )
        return QString("D");
    else if( action == ACTION_REPLACE )
        return QString("U");
    else
        return QString::null;
}

OPimRecord::ChangeAction OPimRecord::strToAction( const QString &str )
{
    if( str == "D" )
        return ACTION_REMOVE;
    else if( str == "U" )
        return ACTION_REPLACE;
    else
        return ACTION_ADD;
}


OPimRecord::OPimRecord( int uid )
        : Qtopia::Record()
{

    m_lastHit = -1;
    setUid( uid );
}


OPimRecord::~OPimRecord()
{}


OPimRecord::OPimRecord( const OPimRecord& rec )
        : Qtopia::Record( rec )
{
    ( *this ) = rec;
}


OPimRecord &OPimRecord::operator=( const OPimRecord& rec )
{
    if ( this == &rec ) return * this;

    Qtopia::Record::operator=( rec );
    m_xrefman = rec.m_xrefman;
    m_lastHit = rec.m_lastHit;

    return *this;
}


/*
 * category names
 */
QStringList OPimRecord::categoryNames( const QString& appname ) const
{
    QStringList list;
    QArray<int> cats = categories();
    Categories catDB;
    catDB.load( categoryFileName() );

    for ( uint i = 0; i < cats.count(); i++ )
    {
        list << catDB.label( appname, cats[ i ] );
    }

    return list;
}


void OPimRecord::setCategoryNames( const QStringList& catnames, const QString& appname )
{
    Categories catDB;
    catDB.load( categoryFileName() );

    QArray<int> cats;
    cats.resize( catnames.count() );

    bool addedCat = false;
    uint i=0;
    for( QStringList::ConstIterator it = catnames.begin(); it != catnames.end(); ++it )
    {
        QString category = (*it).stripWhiteSpace();
        int id = catDB.id( appname, category );
        if( id == 0 ) {
            id = catDB.addCategory( appname, category );
            if( id != 0 )
                addedCat = true;
        }

        if( id != 0 )
            cats[i++] = id;
    }

    // Just in case any couldn't be added
    if( i < catnames.count() )
        cats.resize( i );

    if( addedCat )
        catDB.save( categoryFileName() );

    setCategories( cats );
}


void OPimRecord::addCategoryName( const QString& category, const QString& appname )
{
    Categories catDB;
    catDB.load( categoryFileName() );

    QString catname = category.stripWhiteSpace();
    int id = catDB.id( appname, catname );
    if( id == 0 ) {
        id = catDB.addCategory( appname, catname );
        if( id != 0 )
            catDB.save( categoryFileName() );
    }

    if( id != 0 ) {
        QArray<int> cats = categories();
        if( !cats.find( id ) ) {
            cats.resize( cats.size() + 1 );
            cats[cats.size()-1] = id;
            setCategories( cats );
        }
    }
}


bool OPimRecord::isEmpty() const
{
    return ( uid() == 0 );
}


/* if uid = 1 assign a new one */
void OPimRecord::setUid( int uid )
{
    if ( uid == 1 )
        uid = uidGen().generate();

    Qtopia::Record::setUid( uid );
};


Qtopia::UidGen &OPimRecord::uidGen()
{
    return m_uidGen;
}


OPimXRefManager &OPimRecord::xrefmanager()
{
    return m_xrefman;
}


int OPimRecord::rtti() const
{
    return 0;
}

/**
 * now let's put our data into the stream
 */
/*
 * First read UID
 *            Categories
 *            XRef
 */
bool OPimRecord::loadFromStream( QDataStream& stream )
{
    int Int;
    uint UInt;
    stream >> Int;
    setUid( Int );

    /** Categories */
    stream >> UInt;
    QArray<int> array( UInt );
    for ( uint i = 0; i < UInt; i++ )
    {
        stream >> array[ i ];
    }
    setCategories( array );

    /*
     * now we do the X-Ref stuff
     */
    OPimXRef xref;
    stream >> UInt;
    for ( uint i = 0; i < UInt; i++ )
    {
        xref.setPartner( OPimXRef::One, partner( stream ) );
        xref.setPartner( OPimXRef::Two, partner( stream ) );
        m_xrefman.add( xref );
    }

    return true;
}


bool OPimRecord::saveToStream( QDataStream& stream ) const
{
    /** UIDs */

    stream << uid();

    /** Categories  */
    stream << categories().count();
    for ( uint i = 0; i < categories().count(); i++ )
    {
        stream << categories() [ i ];
    }

    /*
     * first the XRef count
     * then the xrefs
     */
    stream << m_xrefman.list().count();
    for ( OPimXRef::ValueList::ConstIterator it = m_xrefman.list().begin();
            it != m_xrefman.list().end(); ++it )
    {
        flush( ( *it ).partner( OPimXRef::One ), stream );
        flush( ( *it ).partner( OPimXRef::Two ), stream );
    }
    return true;
}


void OPimRecord::flush( const OPimXRefPartner& par, QDataStream& str ) const
{
    str << par.service();
    str << par.uid();
    str << par.field();
}


OPimXRefPartner OPimRecord::partner( QDataStream& stream )
{
    OPimXRefPartner par;
    QString str;
    int i;

    stream >> str;
    par.setService( str );

    stream >> i;
    par.setUid( i );

    stream >> i ;
    par.setField( i );

    return par;
}


void OPimRecord::setLastHitField( int lastHit ) const
{
    m_lastHit = lastHit;
}


int OPimRecord::lastHitField() const
{
    return m_lastHit;
}


QMap<QString, QString> OPimRecord::toExtraMap() const
{
    return customMap;
}


void OPimRecord::setExtraMap( const QMap<QString, QString>& map )
{
    customMap = map;
}

#if 0
QString OPimRecord::recordAttributeTranslated( int field )const
{
    return recordAttributeTranslated()[field];
}
#endif

}
