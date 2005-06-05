/*
                             This file is part of the Opie Project

                             Copyright (C)2004, 2005 Dan Williams <drw@handhelds.org>
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

#ifndef PROMPTDLG_H
#define PROMPTDLG_H

#include <qwidget.h>

class PromptDlg : public QWidget
{
    Q_OBJECT

public:
    PromptDlg( const QString &caption = QString::null, const QString &text = QString::null,
               const QString &btn1 = QString::null, const QString &btn2 = QString::null,
               QWidget *parent = 0x0 );

    int display();
    int btnPressed() { return m_btnClicked; }

    static int ask( const QString &caption = QString::null, const QString &text = QString::null,
                    const QString &btn1 = QString::null, const QString &btn2 = QString::null,
                    QWidget *parent = 0x0 );

private:
    int m_btnClicked; // Indicator for which button was pressed

private slots:
    void slotBtn1Clicked();
    void slotBtn2Clicked();
};

#endif
