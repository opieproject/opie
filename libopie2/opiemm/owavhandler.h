/*
                             This file is part of the Opie Project
              =.             (C) 2009 Team Opie <opie-users@lists.sourceforge.net>
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

#ifndef OWAVHANDLER_H
#define OWAVHANDLER_H

#include <qobject.h>

#include <opie2/odebug.h>

#include "owavfile.h"
#include "osounddevice.h"

namespace Opie {
namespace MM {


/**
 * @brief Base class for sound recorder/player
 *
 */
class OWavHandler : public QObject
{
  Q_OBJECT

  public:
    OWavHandler();
    virtual ~OWavHandler();
    bool setupSoundDevice( bool record );

  protected:
    OWavFileParameters m_fileparams;
    int m_bufsize;
    OWavFile *m_wavfile;
    OSoundDevice *m_device;

  protected:
    void finalize();

  private:
    class Private;
    Private *d;
};



}
}

#endif