/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2009 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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

#ifndef PROFILELIST_H
#define PROFILELIST_H

#include "profilelistbase.h"
#include "kbddhandler.h"

class KbddProfileList : public KbddProfileListBase
{
    Q_OBJECT

public:
    KbddProfileList( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KbddProfileList();

    static QString appName() { return QString::fromLatin1("kbddcontrol"); }

    QStringList m_profiles;

protected slots:
    void slotAddProfile();
    void slotEditProfile();
    void slotDeleteProfile();

protected:
    void accept();
  void done(int);

    KbddHandler kbdd;
};

#endif // PROFILELIST_H
