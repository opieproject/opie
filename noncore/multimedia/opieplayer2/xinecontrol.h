/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
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

#ifndef XINECONTROL_H
#define XINECONTROL_H

#include "lib.h"
#include "mediadetect.h"
#include <qobject.h>

class XineControl : public QObject  {
    Q_OBJECT
public:
    XineControl( QObject *parent = 0, const char *name =0 );
    ~XineControl();

public slots:
    void play( const QString& fileName );
    void stop( bool );
    void pause( bool );
    void setFullscreen( bool );
    int currentTime();
    void seekTo( long );
    // get length of media file and set it
    void length();

    int position();

private:
    XINE::Lib *libXine;
    MediaDetect mdetect;
    int m_length;
    int m_currentTime;
    int m_position;

signals:
    void positionChanged( int position  );

};


#endif
