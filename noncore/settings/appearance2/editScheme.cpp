/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
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

#include "editScheme.h"

#include <opie/ocolorbutton.h>

#include <qaction.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qtoolbutton.h>

EditScheme::EditScheme ( int cnt, const QString *labels, QColor *colors, QWidget* parent,  const char* name, bool modal, WFlags fl )
	: QDialog ( parent, name, modal, fl )
{
	setCaption ( tr( "Edit scheme" ) );
	QGridLayout *layout = new QGridLayout ( this, 0, 0, 4, 4 );

    m_count = cnt;
    m_buttons = new OColorButton * [cnt];
    m_colors = colors;
        
    for ( int i = 0; i < cnt; i++ )
    {
        QLabel *l = new QLabel ( labels [i], this );
        layout-> addWidget ( l, i, 0 );
        
        m_buttons [i] = new OColorButton ( this );
        m_buttons [i]-> setColor ( colors [i] );
        layout-> addWidget ( m_buttons [i], i, 1 );
    }
}

EditScheme::~EditScheme ( )
{
	delete [] m_buttons;
}

void EditScheme::accept ( )
{
	for ( int i = 0; i < m_count; i++ ) 
		m_colors [i] = m_buttons [i]-> color ( );
		
	QDialog::accept ( );
}

