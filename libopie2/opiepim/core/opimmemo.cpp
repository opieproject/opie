/*
                             This file is part of the Opie Project
                             
              =.             Copyright (C) 2009 The Opie Team <opie-devel@lists.sourceforge.net>
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

#include "opimmemo.h"

/* OPIE */
#include <opie2/opimresolver.h>
#include <opie2/odebug.h>

#include <qpe/palmtopuidgen.h>
#include <qpe/palmtoprecord.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>
#include <qpe/stringutil.h>

/* QT */
#include <qobject.h>
#include <qshared.h>

namespace Opie
{

struct OPimMemo::OPimMemoData : public QShared
{
    QString text;
    ChangeAction action;
};

/*!
  Creates a new, empty contact.
*/
OPimMemo::OPimMemo()
    : OPimRecord()
    , d( 0 )
    , data( new OPimMemoData() )
{
    data->action = ACTION_ADD;
}

OPimMemo::OPimMemo( const QMap<int, QString> &map )
    : OPimRecord()
    , d( 0 )
    , data( new OPimMemoData() )
{
    data->action = ACTION_ADD;
    fromMap( map );
}


OPimMemo::OPimMemo( const OPimMemo &memo )
    : OPimRecord( memo )
    , d( 0 )
    , data( memo.data )
{
    data->ref();
}


OPimMemo::~OPimMemo()
{

    if ( data->deref() )
    {
        delete data;
        data = 0l;
    }
}


OPimMemo::OPimMemo( const QString &text,
                    const QArray<int> &category,
                    int uid )
    : OPimRecord( uid )
    , d( 0 )
    , data( new OPimMemoData )
{
    setCategories( category );

    data->text = text;
    data->action = ACTION_ADD;
}


OPimMemo::OPimMemo( const QString &text,
                    const QStringList &category,
                    int uid )
    : OPimRecord( uid )
{
    setCategories( idsFromString( category.join( ";" ) ) );

    data = new OPimMemoData;

    data->text = text;
}


bool OPimMemo::match( const QRegExp &regExp ) const
{
    if ( data->text.find( regExp ) != -1 )
    {
        setLastHitField( Text );
        return true;
    }
    return false;
}


QString OPimMemo::text() const
{
    return data->text;
}

QString OPimMemo::description() const
{
    // First line of text
    int pos = data->text.find('\n');
    return data->text.left( pos );
}

void OPimMemo::setText( const QString& text )
{
    changeOrModify();
    data->text = text;
}


QString OPimMemo::toShortText() const
{
    return description();
}


/*!
  Returns a rich text string
*/
QString OPimMemo::toRichText() const
{
    QString text;
    QStringList catlist;

    text += "<p>";
    text += this->text();
    text += "</p>";

    // categories
    text += "<b>" + QObject::tr( "Category:" ) + "</b> ";
    text += categoryNames( "Notes" ).join( ", " );
    text += "<br>";

    return text;
}

bool OPimMemo::operator<( const OPimMemo &memo ) const
{
    return description() < memo.description();
}


bool OPimMemo::operator<=( const OPimMemo &memo ) const
{
    return description() <= memo.description();
}


bool OPimMemo::operator>( const OPimMemo &memo ) const
{
    return description() > memo.description();
}


bool OPimMemo::operator>=( const OPimMemo &memo ) const
{
    return description() >= memo.description();
}


bool OPimMemo::operator==( const OPimMemo &memo ) const
{
    if ( data->text != memo.data->text )
        return false;

    return OPimRecord::operator==( memo );
}


void OPimMemo::deref()
{

    if ( data->deref() )
    {
        delete data;
        data = 0;
    }
}


OPimMemo &OPimMemo::operator=( const OPimMemo &item )
{
    if ( this == &item ) return * this;

    OPimRecord::operator=( item );
    item.data->ref();
    deref();
    data = item.data;

    return *this;
}


QMap<int, QString> OPimMemo::toMap() const
{
    QMap<int, QString> map;

    map.insert( Uid, QString::number( uid() ) );
    map.insert( Category, idsToString( categories() ) );

    map.insert( Text, data->text );

    if( data->action != ACTION_ADD )
        map.insert( FIELDID_ACTION, actionToStr( data->action ) );
    
    return map;
}

void OPimMemo::fromMap( const QMap<int, QString>& map )
{
    // We just want to set the UID if it is really stored.
    if ( !map[ Uid ].isEmpty() )
        setUid( map[ Uid ].toInt() );

    setCategories( idsFromString( map[ Category ] ) );

    setText( map[ Text ] );

    setAction( strToAction( map[ FIELDID_ACTION ] ) );
}


/**
 *  change or modify looks at the ref count and either
 *  creates a new QShared Object or it can modify it
 * right in place
 */
void OPimMemo::changeOrModify()
{
    if ( data->count != 1 )
    {
        data->deref();
        OPimMemoData* d2 = new OPimMemoData();
        copy( data, d2 );
        data = d2;
    }
}


// WATCHOUT
/*
 * if you add something to the Data struct
 * be sure to copy it here
 */
void OPimMemo::copy( OPimMemoData* src, OPimMemoData* dest )
{
    dest->text = src->text;
    dest->action = src->action;
}


QString OPimMemo::type() const
{
    return QString::fromLatin1( "OPimMemo" );
}


QString OPimMemo::recordField( int  id) const
{
    QString res;
    Q_UNUSED( id )
    // FIXME Not used?
    return res;
}

OPimRecord::ChangeAction OPimMemo::action() const
{
    return data->action;
}

void OPimMemo::setAction( OPimRecord::ChangeAction action )
{
    changeOrModify();
    data->action = action;
}

int OPimMemo::rtti() const
{
    return OPimResolver::Notes;
}

/**
 * \brief Provide a SafeCast to OPimMemo from a OPimRecord
 *
 * Provide a safe cast that will return 0 if the record
 * type is not OPimMemo. In the other case it will
 * be casted to OPimMemo and returned
 *
 * @param rec The OPimRecord to be casted
 *
 * @return a pointer to OPimMemo or 0l
 */
OPimMemo* OPimMemo::safeCast( const OPimRecord* rec ) {
    return (rec && rec->rtti() == OPimResolver::Notes ) ?
        static_cast<OPimMemo*>( const_cast<OPimRecord*>(rec) ) :
        0l;
}

}
