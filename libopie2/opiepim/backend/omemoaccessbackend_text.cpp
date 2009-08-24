/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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
/*
 * XML Backend for the OPIE-Notes Database.
 */


/* OPIE */
#include <opie2/omemoaccessbackend_text.h>
#include <opie2/xmltree.h>
#include <opie2/omemoaccessbackend.h>
#include <opie2/omemoaccess.h>
#include <opie2/odebug.h>

#include <qpe/global.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qasciidict.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qarray.h>
#include <qmap.h>
#include <qtextstream.h>

/* STD */
#include <stdlib.h>
#include <errno.h>

using namespace Opie::Core;


namespace Opie {
    
OPimMemoAccessBackend_Text::OPimMemoAccessBackend_Text ( const QString& appname, const QString& path )
{
    m_appName = appname;

    // Expecting to access the default path if nothing else is set
    if ( path.isEmpty() )
        m_path = QPEApplication::documentDir() + "/text/plain/";
    else {
        m_path = path;

        if( !m_path.isEmpty() && m_path.right(1) != '/' )
            m_path += '/';
    }
}

bool OPimMemoAccessBackend_Text::save()
{
    return true;
}

bool OPimMemoAccessBackend_Text::load ()
{
    return true;
}

void OPimMemoAccessBackend_Text::clear ()
{
    
}

bool OPimMemoAccessBackend_Text::wasChangedExternally()
{
    return false; // FIXME what to do here???
}

UIDArray OPimMemoAccessBackend_Text::allRecords() const
{
    QDir memodir( m_path );
    memodir.setFilter( QDir::Files );
    memodir.setNameFilter( "*.txt" );

    UIDArray uid_list( memodir.count() );
    uint counter = 0;
    for ( uint i=0; i<memodir.count(); i++ ) {
        QString memoname = memodir[i];
        memoname = memoname.mid( 0, memoname.length() - 4 );
        odebug << memoname << oendl;
        uid_list[counter++] = memoname.toInt();
    }

    return ( uid_list );
}

bool OPimMemoAccessBackend_Text::loadMemo( int uid, OPimMemo &memo ) const
{
    QString fileName = m_path + QString::number( uid ) + ".txt";

    QFile file( fileName );
    if ( file.exists() ) {
        if ( !file.open( IO_ReadOnly ) ) {
            odebug << "loadMemo: could not read file '" << fileName << "'" << oendl;
            return false;
        }
        else {
            QTextStream inStream( &file );
            inStream.setEncoding( QTextStream::UnicodeUTF8 );
            memo.setText( inStream.read() );
            file.close();
        }
    }
    else {
        odebug << "loadMemo: file " << fileName << " does not exist " << oendl;
        return false;
    }

    memo.setUid( uid );
    return true;
}

bool OPimMemoAccessBackend_Text::saveMemo( const OPimMemo &memo, bool mustExist ) const
{
    QString fileName = m_path + QString::number( memo.uid() ) + ".txt";

    QFile file( fileName );
    if( mustExist && !file.exists() ) {
        odebug << "saveMemo: file '" << fileName << "' was expected to exist and doesn't" << oendl;
        return false;
    }
    
    if ( !file.open( IO_WriteOnly ) ) {
        odebug << "saveMemo: could not open file '" << fileName << "' for writing" << oendl;
        return false;
    }
    else {
        QTextStream outStream( &file );
        outStream.setEncoding( QTextStream::UnicodeUTF8 );
        outStream << memo.text();
        file.close();
    }
    return true;
}



OPimMemo OPimMemoAccessBackend_Text::find ( int uid ) const
{
    OPimMemo memo; // Create empty memo

    loadMemo( uid, memo );

    return ( memo );
}


UIDArray OPimMemoAccessBackend_Text::matchRegexp(  const QRegExp &r ) const
{
    UIDArray m_currentQuery( 0 ); // FIXME
    /*
    UIDArray m_currentQuery( m_contactList.count() );
    QListIterator<OPimMemo> it( m_contactList );
    uint arraycounter = 0;

    for( ; it.current(); ++it ) {
        if ( (*it)->match( r ) ) {
            m_currentQuery[arraycounter++] = (*it)->uid();
        }

    }
    // Shrink to fit..
    m_currentQuery.resize(arraycounter);
*/
    return m_currentQuery;
}

bool OPimMemoAccessBackend_Text::add ( const OPimMemo &newcontact )
{
    return saveMemo( newcontact );
}

bool OPimMemoAccessBackend_Text::replace ( const OPimMemo &contact )
{
    return saveMemo( contact, true );
}

bool OPimMemoAccessBackend_Text::remove ( int uid )
{
    QString fileName = m_path + QString::number( uid ) + ".txt";

    QFile file( fileName );
    return file.remove();
}

bool OPimMemoAccessBackend_Text::reload() {
    /* Reload is the same as load in this implementation */
    return ( load() );
}


}
