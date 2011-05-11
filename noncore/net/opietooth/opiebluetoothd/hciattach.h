/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
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
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef HCIATTACH_H
#define HCIATTACH_H

#include <qobject.h>

namespace Opie {
namespace Core {
    class OProcess;
}
}

using Opie::Core::OProcess;

class OHciAttach: public QObject {
        Q_OBJECT
    public:
        OHciAttach();
        ~OHciAttach();

        bool isConfigured();
        void start();
        void stop();

    protected:
        void readConfig();

        OProcess *m_process;
        QString m_port;
        QString m_protocol;
        uint m_speed;

    protected slots:
        virtual void slotExited( Opie::Core::OProcess* );
        //virtual void slotStdOut(Opie::Core::OProcess*, char*, int );
        //virtual void slotStdErr(Opie::Core::OProcess*, char*, int );
};

#endif