/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Team <opie@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>
#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/odebug.h>

#include <qpe/qpeapplication.h>

/* QT */
#include <qdir.h>

using namespace Opie::DB;
int main( int argc,  char* argv[] ) {

QPEApplication app( argc, argv );
OSQLManager man;
man.registerPath( QDir::currentDirPath() );
OSQLBackEnd::ValueList list = man.queryBackEnd();

OSQLDriver *driver = man.standard();
 owarn << "testmain" + driver->id() << oendl;
 driver->setUrl("/home/ich/test2vhgytry");
 if ( driver->open() ) {
     owarn << "could open" << oendl;
 }else
     owarn << "wasn't able to open" << oendl;
 OSQLRawQuery raw("select * from t2" );
 OSQLResult res = driver->query( &raw );

 OSQLRawQuery raw2( "insert into t2 VALUES(ROWID,'Meine Mutter') ");
 res = driver->query(&raw2);

};
