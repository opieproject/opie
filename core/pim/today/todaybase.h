/*
                             This file is part of the Opie Project

                             Copyright (C) Maximilian Reiss <harlekin@handhelds.org>
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

#ifndef TODAYBASE_H
#define TODAYBASE_H

#include <opie2/oclickablelabel.h>

#include <qlayout.h>

class QVBox;
class QGridLayout;
class QFrame;
class QLabel;
class QPushButton;
class QBoxLayout;




class TodayBase : public QWidget
{
    Q_OBJECT

public:
    TodayBase( QWidget* parent = 0, const char* name = 0, WFlags = 0 );
    ~TodayBase();

    QFrame* Frame4;
    QPushButton* DatesButton;
    QVBoxLayout* layout;
    Opie::Ui::OClickableLabel* OwnerField;
    QLabel* Frame;
    QLabel* DateLabel;
    QFrame* Frame15;
    QLabel* Opiezilla;
    QLabel* TodayLabel;
    Opie::Ui::OClickableLabel* ConfigButton;

};

#endif
