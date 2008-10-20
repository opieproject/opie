/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
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

#ifndef DATEBOOKDAYHEADER_H
#define DATEBOOKDAYHEADER_H
#include "datebookdayheader.h"

#include <qdatetime.h>

class DateBookDayHeader : public DateBookDayHeaderBase
{
    Q_OBJECT

public:
    DateBookDayHeader( bool bUseMonday, QWidget* parent = 0,
		       const char* name = 0 );
    ~DateBookDayHeader();
    void setStartOfWeek( bool onMonday );

public slots:
    void goBack();
    void goForward();
    void goBackWeek();
    void goForwardWeek();
    void setDate( int, int, int );
    void setDay( int );
    void gotHide();

signals:
    void dateChanged( int y, int m, int d );

private slots:
    void pickDate();


private:
    QDate currDate;
    bool bUseMonday;
    void setupNames();

};

#endif // DATEBOOKDAYHEADER_H
