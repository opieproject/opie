/*
                             This file is part of the Opie Project

                             (C) 2003 Patrick S. Vogt
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OPIECONFIG_H
#define OPIECONFIG_H


#ifdef QWS
#include <qpe/config.h>
#else 
#include <qsettings.h>
#endif


class OpieConfig 
#ifdef QWS
 : public Config
#else
: public QSettings
#endif
{
  
 public:	
#ifndef QWS
    enum Domain { File, User };
#endif

    OpieConfig( const QString&, Domain );

#ifndef QWS
    void setGroup( const QString& key);
	bool hasKey ( const QString & key ) const;
#endif

    /**
     * @returns the name of the current group.
     * The current group is used for searching keys and accessing entries.
     */
    const QString& group() 
#ifdef QWS
    { return git.key(); };
#else
    { return group(); };
#endif

#ifndef QWS
 private:
    bool hasGroup;
#endif

};

#endif // OPIECONFIG_H
