/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "playlistwidget.h"
#include "om3u.h"

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/storage.h>
#include <qpe/mimetype.h>
#include <qpe/global.h>
#include <qpe/resource.h>

#include <qdir.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qcstring.h>

//extern PlayListWidget *playList;

Om3u::Om3u( const QString &filePath)
      : QStringList (){
//filePath is path name to m3u
//qDebug("<<<<<<<new m3u "+filePath);
 f.setName(filePath);
        if(f.exists())
            f.open( IO_ReadWrite );
        else
            f.open( IO_ReadWrite | IO_Truncate);
}

Om3u::~Om3u(){}

void Om3u::readM3u() { //it's m3u
//    qDebug("<<<<<<reading m3u "+f.name());
    QTextStream t(&f);
    QString s;
    while ( !t.atEnd() ) {
        s=t.readLine();

        if( s.find( "#", 0, TRUE) == -1 ) {
            if( s.find( " ",  0, TRUE) == -1 )  {
                if( s.left(2) == "E:" || s.left(2) == "P:" ) {
                    s = s.right( s.length() -2 );
                    QFileInfo f( s );
                    QString name = f.baseName();
                    name = name.right( name.length() - name.findRev( "\\", -1, TRUE )  -1 );
                    s=s.replace( QRegExp( "\\" ), "/" );
                    append(s);
//                    qDebug(s);
                } else { // is url
                    s.replace( QRegExp( "%20" )," " );
                    QString name;
                    if( name.left( 4 ) == "http" ) {
                        name = s.right( s.length() - 7 );
                    } else {
                        name = s;
                    }
                    append(name);
//                    qDebug(name);
                }
            }
        }
    }
}

void Om3u::readPls() { //it's a pls file
        QTextStream t( &f );
        QString s;
        while ( !t.atEnd() ) {
            s = t.readLine();
            if( s.left(4)  == "File" ) {
                s = s.right( s.length() - 6 );
                s.replace( QRegExp( "%20" )," ");
//                qDebug( "adding " + s + " to playlist" );
                // numberofentries=2
                // File1=http
                // Title
                // Length
                // Version
                // File2=http
                s = s.replace( QRegExp( "\\" ), "/" );
                QFileInfo f( s );
                QString name = f.baseName();
                if( name.left( 4 ) == "http" ) {
                    name = s.right( s.length() - 7);
                }  else {
                    name = s;
                }
                name = name.right( name.length() - name.findRev( "\\", -1, TRUE) - 1 );
                if( s.at( s.length() - 4) == '.') // if this is probably a file
                append(s);
                else { //if its a url
                    if( name.right( 1 ).find( '/' ) == -1) {
                        s += "/";
                    }
                    append(s);
                }
            }
        }
}

void Om3u::write() { //writes list to m3u file
    QString list;
    for ( QStringList::ConstIterator it = begin(); it != end(); ++it ) {
        qDebug(*it);
        list += *it+"\n";
    }
    f.writeBlock( list, list.length() );
    f.close();
}

void Om3u::add(const QString &filePath) { //adds to m3u file
    append(filePath);
}

void Om3u::remove(const QString &filePath) { //removes from m3u list

}

void Om3u::deleteFile(const QString &filePath) {//deletes m3u file

}

void Om3u::close() { //closes m3u file
    f.close();
}
