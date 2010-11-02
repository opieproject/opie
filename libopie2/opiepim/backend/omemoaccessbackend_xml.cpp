/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

/* OPIE */
#include <opie2/omemoaccessbackend_xml.h>
#include <opie2/odebug.h>

#include <qtopia/global.h>
#include <qtopia/stringutil.h>

/* QT */
#include <qasciidict.h>
#include <qfile.h>
#include <qintdict.h>


using namespace Opie;

namespace {

    static void save( const OPimMemo& memo, const QIntDict<QString> &revdict, QString& buf ) {

        QMap<int, QString> map = memo.toMap();

        for ( QMap<int, QString>::ConstIterator it = map.begin(); it != map.end(); ++it ) {
            const QString &value = it.data();
            int key = it.key();
            if ( !value.isEmpty() ) {
                QString *name = revdict[ key ];
                if( name ) {
                    buf += " " + *name;
                    buf += "=\"" + Qtopia::escapeString( value ) + "\"";
                }
            }
        }

        // skip custom writing
    }

    static bool saveEachItem( const QMap<int, OPimMemo>& list, QIntDict<QString> &revdict, OAbstractWriter &wr ) {
        QMap<int, OPimMemo>::ConstIterator it;
        QString buf;
        QCString str;
        for ( it = list.begin(); it != list.end(); ++it ) {
            buf = "<memo";
            save( it.data(), revdict, buf );
            buf += " />\n";
            str = buf.utf8();

            if ( !wr.writeString( str ) )
                return false;
        }
        return true;
    }
}

namespace Opie {
OPimMemoAccessBackend_XML::OPimMemoAccessBackend_XML()
    : OPimMemoAccessBackend()
{
}

OPimMemoAccessBackend_XML::~OPimMemoAccessBackend_XML()
{
}

void OPimMemoAccessBackend_XML::initDict( QAsciiDict<int> &dict ) const
{
    dict.setAutoDelete( true );
    dict.insert( "categories", new int(OPimMemo::Category) );
    dict.insert( "text", new int(OPimMemo::Text) );
    dict.insert( "uid", new int(OPimMemo::Uid) );
    dict.insert( "change_action", new int(FIELDID_ACTION) );
}

bool OPimMemoAccessBackend_XML::load()
{
    return true;
}

bool OPimMemoAccessBackend_XML::reload()
{
    clear();
    return load();
}

bool OPimMemoAccessBackend_XML::save()
{
    owarn << "OPimMemoAccessBackend_XML: unimplemented save() function called" << oendl;
    return false;
}

bool OPimMemoAccessBackend_XML::write( OAbstractWriter &wr )
{
    QString buf( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    buf += "<!DOCTYPE notes><notes>\n";
    buf += "<memos>\n";
    QCString str = buf.utf8();
    if ( !wr.writeString( str ) )
        return false;

    QAsciiDict<int> dict(OPimMemo::Text+1);
    initDict( dict );
    QIntDict<QString> revdict( dict.size() );
    revdict.setAutoDelete( true );
    // Now we need to reverse the dictionary (!)
    for( QAsciiDictIterator<int> it( dict ); it.current(); ++it ) {
        revdict.insert( (*it), new QString( it.currentKey() ) );
    }

    if (!saveEachItem( m_items, revdict, wr ) )
        return false;

    buf = "</memos>\n</notes>\n";
    str = buf.utf8();
    if ( !wr.writeString( str ) )
        return false;

    return true;
}

QArray<int> OPimMemoAccessBackend_XML::allRecords() const
{
    QArray<int> ints( m_items.count() );
    uint i = 0;
    QMap<int, OPimMemo>::ConstIterator it;

    for ( it = m_items.begin(); it != m_items.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}

QArray<int> OPimMemoAccessBackend_XML::queryByExample(const OPimMemo&, int,  const QDateTime& ) const
{
    owarn << "OPimMemoAccessBackend_XML: unimplemented queryByExample() function called" << oendl;
    return QArray<int>();
}

QArray<int> OPimMemoAccessBackend_XML::matchRegexp(  const QRegExp & ) const
{
    owarn << "OPimMemoAccessBackend_XML: unimplemented matchRegexp() function called" << oendl;
    return QArray<int>();
}

void OPimMemoAccessBackend_XML::clear()
{
    m_items.clear();
}

OPimMemo OPimMemoAccessBackend_XML::find( int uid ) const
{
    return m_items[uid];
}

bool OPimMemoAccessBackend_XML::add( const OPimMemo& memo )
{
    m_items.insert( memo.uid(), memo );
    return true;
}

bool OPimMemoAccessBackend_XML::remove( int uid )
{
    m_items.remove( uid );
    return true;
}

bool OPimMemoAccessBackend_XML::replace( const OPimMemo& memo )
{
    remove( memo.uid() );
    return add( memo );
}

bool OPimMemoAccessBackend_XML::wasChangedExternally()
{
    // This backend is for syncing only so we don't need to implement this
    return false;
}

bool OPimMemoAccessBackend_XML::readInto( OPimXmlReader &rd, OPimMemoAccess *target )
{
    QAsciiDict<int> dict(OPimMemo::Text+1);
    initDict( dict );

    OPimWriteBackXmlHandler<OPimMemo> handler( "memo", dict, target );
    OPimXmlStreamParser parser( handler );
    rd.read( parser );
    return true;
}


}
