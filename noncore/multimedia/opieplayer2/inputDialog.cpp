/*
                            This file is part of the Opie Project

                             Copyright (c)  2002,2003 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
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
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include "inputDialog.h"

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include <qfileinfo.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

InputDialog::InputDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
    if ( !name ) {
        setName( "InputDialog" );
    }
    resize( 234, 115);
    setMaximumSize( QSize( 240, 40));
    setCaption( tr( name ) );

    QPushButton *browserButton;
    //browserButton = new QPushButton( Resource::loadIconSet("fileopen"),"",this,"BrowseButton");
    //browserButton->setGeometry( QRect( 205, 10, 22, 22));
    //connect( browserButton, SIGNAL(released()),this,SLOT(browse()));
    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 4, 10, 190, 22 ) );
    LineEdit1->setFocus();
}
/*
 * return the current text(input)
 */
QString InputDialog::text() const {
   return LineEdit1->text();
}
/*
 *  Destroys the object and frees any allocated resources
 */
InputDialog::~InputDialog() {
}

