/*

               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Benjamin Meyer <meyerb@sharpsec.com>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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
/****************************************************************************
** Form interface generated from reading ui file 'app.ui'
**
** Created: Fri May 3 14:38:09 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef APPEDIT_H
#define APPEDIT_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;

class AppEdit : public QDialog
{
    Q_OBJECT

public:
    AppEdit( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AppEdit();

    QLabel* TextLabel3;
    QLabel* TextLabel4;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QLineEdit* iconLineEdit;
    QLineEdit* nameLineEdit;
    QLineEdit* commentLineEdit;
    QLineEdit* execLineEdit;

protected:
    QGridLayout* AppEditLayout;
};

#endif // APPEDIT_H
