/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
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

#include "password.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qwidget.h>

static const char* const showhideimage_data[] = {
"16 16 2 1",
". c None",
"# c #000000",
"................",
"...#...###...##.",
"..#.#..#..#.##..",
"..###..###.##...",
".#...#.#..##....",
".#...#.#.##.....",
"........##.#..#.",
"..##...##...##..",
".#..#.###...##..",
".#...##..#.#..#.",
".#..##..........",
".#.##.#..#.#..#.",
"..##...##...##..",
".##....##...##..",
".#....#..#.#..#.",
"................"};

Password::Password( QWidget *parent, const char *caption, const char *prompt )
	: QDialog( parent, 0x0, TRUE, 0x0 )
{
	setCaption( caption );

	QGridLayout *layout = new QGridLayout( this );
	layout->setSpacing( 2 );
	layout->setMargin( 4 );

	QLabel *label = new QLabel( prompt, this );
	label->setAlignment( AlignLeft | AlignTop | WordBreak );
	layout->addMultiCellWidget( label, 0, 0, 0, 1 );

	pw = new QLineEdit( this );
	pw->setEchoMode( QLineEdit::Password );
	layout->addWidget( pw, 1, 0 );

	QPixmap *pic = new QPixmap( ( const char** ) showhideimage_data );
	QPushButton *btn = new QPushButton( ( QIconSet ) *pic, QString::null, this );
	btn->setMaximumSize( pic->width() + 10, pic->height() + 10 );
	btn->setToggleButton( TRUE );
	connect( btn, SIGNAL( toggled( bool ) ), this, SLOT( slotTogglePassword( bool ) ) );
	layout->addWidget( btn, 1, 1 );
	
	password == "";
}

Password::~Password()
{
}

void Password::accept()
{
	password = pw->text();
	QDialog::accept();
}

void Password::slotTogglePassword( bool showPW )
{
	showPW ? pw->setEchoMode( QLineEdit::Normal ) : pw->setEchoMode( QLineEdit::Password );
}
