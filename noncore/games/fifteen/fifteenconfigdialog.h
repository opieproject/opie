/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2004 Holger Freyther <zecke@handhelds.org>
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


#ifndef FIFTEEN_CONFIG_DIALOG_H
#define FIFTEEN_CONFIG_DIALOG_H

#include "fifteenconfigdialogbase.h"

class FifteenConfigDialog : public FifteenConfigDialogBase {
    Q_OBJECT
public:
    FifteenConfigDialog( QWidget *parent = 0, const char* name = 0, bool modal =TRUE );
    ~FifteenConfigDialog();

    void setImageSrc( const QString& );
    QString imageSrc()const;

    void setGameboard( int rows, int columns );
    int columns()const;
    int rows()const;

protected slots:
    void slotLoadImage();

private:
    QPixmap preview( const QString& );

};


#endif
