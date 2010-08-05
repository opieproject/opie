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
 * Text Backend for the OPIE-Notes Database.
 */

#ifndef _OPimMemoAccessBackend_Text_
#define _OPimMemoAccessBackend_Text_

#include <opie2/omemoaccessbackend.h>
#include <opie2/omemoaccess.h>

#include <qlist.h>
#include <qdict.h>

namespace Opie {



/* the default xml implementation */
/**
 * This class is the flat text implementation of a memo backend
 * it does implement everything available for OPimMemo.
 * @see OPimAccessBackend for more information of available methods
 */
class OPimMemoAccessBackend_Text : public OPimMemoAccessBackend
{
public:
    OPimMemoAccessBackend_Text ( const QString& appname = QString::null, const QString& path = QString::null );

    bool save();

    bool load ();

    void clear ();

    bool wasChangedExternally();

    UIDArray allRecords() const;

    OPimMemo find ( int uid ) const;

    UIDArray matchRegexp(  const QRegExp &r ) const;

    bool add ( const OPimMemo &newmemo );

    bool replace ( const OPimMemo &memo );

    bool remove ( int uid );
    bool reload();

private:
    bool checkCreateDirectory() const;
    bool loadMemo( int uid, OPimMemo &memo ) const;
    bool saveMemo( const OPimMemo &memo, bool mustExist = false ) const;

protected:
    QString m_path;
    QString m_appName;
};

}

#endif
