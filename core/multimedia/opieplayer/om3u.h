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

#ifndef OM3U_H
#define OM3U_H

#include "playlistwidget.h"

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
#include <qfile.h>


class Om3u : public QStringList {
//    Q_OBJECT
public:
    Om3u( const QString &filePath, int mode);
    ~Om3u();
    void readM3u();
    void readPls();
    void write();
    void add(const QString &);
    void remove(const QString &);
    void deleteFile(const QString &);
    void close();
    
public slots:

protected:

private:
  QFile f;
private slots:
        

};    

#endif// M3U_H


