/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.
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

#include "eventvisualiser.h"

using namespace Opie;

QColor EventVisualiser::eventColour( const OPimOccurrence &occ )
{
    const OPimEvent ev = occ.toEvent();
    bool external = !ev.isValidUid();

    if( external )
        return QColor( 150, 255, 150 ); // light green
    else if( ev.isAllDay() )
        return Qt::yellow;

    QDate currentDate = QDate::currentDate();
    QDate occDate = occ.date();
    if( occDate == currentDate ) {
        QTime curTime = QTime::currentTime();
        if( occ.endTime() < curTime ) {
            return QColor( 220, 220, 220 ); // light grey, inactive
        }
        else {
            //change color in dependence of the time till the event starts
            int duration = curTime.secsTo(occ.startTime());
            if (duration < 0) duration = 0;
            int colChange = duration*160/86400; //86400: secs per day, 160: max color shift
            return  QColor( 200-colChange, 200-colChange, 255 ); //blue
        }
    }
    else if( occDate > currentDate )
        return QColor( 150, 150, 255 ); // blue
    else
        return QColor( 220, 220, 220 ); // light grey, inactive
}
