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

#include "configuration.h"

#include <qcheckbox.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qwhatsthis.h>

Configuration::Configuration( QWidget *parent, const QString &cs, bool sl, bool sb )
	: QDialog( parent, 0, TRUE, WStyle_ContextHelp )
{
	setCaption( tr( "Configure Checkbook" ) );

	QFontMetrics fm = fontMetrics();
	int fh = fm.height();

	QGridLayout *layout = new QGridLayout( this );
	layout->setSpacing( 4 );
	layout->setMargin( 4 );

	QLabel *label = new QLabel( tr( "Enter currency symbol:" ), this );
	QWhatsThis::add( label, tr( "Enter your local currency symbol here." ) );
	label->setMaximumHeight( fh + 3 );
	layout->addWidget( label, 0, 0 );

	symbolEdit = new QLineEdit( cs, this );
	QWhatsThis::add( symbolEdit, tr( "Enter your local currency symbol here." ) );
	symbolEdit->setMaximumHeight( fh + 5 );
	symbolEdit->setFocus();
    layout->addWidget( symbolEdit, 0, 1 );

	lockCB = new QCheckBox( tr( "Show whether checkbook is password\nprotected" ), this );
	QWhatsThis::add( lockCB, tr( "Click here to select whether or not the main window will display that the checkbook is protected with a password." ) );
	lockCB->setChecked( sl );
	layout->addMultiCellWidget( lockCB, 1, 1, 0, 1 );

	balCB = new QCheckBox( tr( "Show checkbook balances" ), this );
	QWhatsThis::add( balCB, tr( "Click here to select whether or not the main window will display the current balance for each checkbook." ) );
	balCB->setMaximumHeight( fh + 5 );
	balCB->setChecked( sb );
	layout->addMultiCellWidget( balCB, 2, 2, 0, 1 );
}

Configuration::~Configuration()
{
}
