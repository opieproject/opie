/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2003 Dan Williams <drw@handhelds.org>
             .=l.
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef PROMPTDLG_H
#define PROMPTDLG_H

#include <qwidget.h>

class PromptDlg : public QWidget
{
    Q_OBJECT

public:
    PromptDlg( const QString &caption = 0x0, const QString &text = 0x0, const QString &btn1 = 0x0,
               const QString &btn2 = 0x0, QWidget *parent = 0x0 );

    int display();
    int btnPressed() { return m_btnClicked; }

    static int ask( const QString &caption = 0x0, const QString &text = 0x0, const QString &btn1 = 0x0,
                    const QString &btn2 = 0x0, QWidget *parent = 0x0 );

private:
    int m_btnClicked; // Indicator for which button was pressed

private slots:
    void slotBtn1Clicked();
    void slotBtn2Clicked();
};

#endif
