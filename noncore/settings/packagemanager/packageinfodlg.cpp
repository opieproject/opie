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

#include "packageinfodlg.h"

#include <qlayout.h>
#include <qpushbutton.h>

#include <qpe/resource.h>

PackageInfoDlg::PackageInfoDlg( QWidget *parent, OPackageManager *pm, const QString &package )
    : QWidget( 0x0 )
    , m_packman( pm )
    , m_output( this )
{
    // Initialize UI
    if ( parent )
        parent->setCaption( package );

    QVBoxLayout *layout = new QVBoxLayout( this, 4, 0 );

    m_output.setReadOnly( true );
    layout->addWidget( &m_output );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "enter" ), tr( "Close" ), this );
    layout->addWidget( btn );
// TODO    connect( btn, SIGNAL(clicked()), this, SLOT(slotBtnClose()) );


}
