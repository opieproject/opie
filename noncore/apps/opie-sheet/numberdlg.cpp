/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
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

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#include "numberdlg.h"

NumberDialog::NumberDialog(QWidget *parent)
        :QDialog(parent, 0, TRUE)
{
    edit=new QSpinBox(this);
    edit->setGeometry(120, 10, 70, 25);

    label=new QLabel(this);
    label->setGeometry(10, 10, 100, 25);
    label->setBuddy(edit);

    resize(200, 45);
}

NumberDialog::~NumberDialog()
{}

int NumberDialog::exec(const QString &caption, const QString &text,
                       int value, int min, int max, int step)
{
    setCaption(caption);
    label->setText(text);
    edit->setValue(value);
    edit->setMinValue(min);
    edit->setMaxValue(max);
    edit->setLineStep(step);

    return QDialog::exec();
}

int NumberDialog::getValue()
{
    return edit->value();
}
