/*
                             This file is part of the OPIE Project
               =.            Copyright (c)  2002 Trolltech AS <info@trolltech.com>
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
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

#include "guisettings.h"

#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>

#include <qlistview.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <stdlib.h>


GuiSettings::GuiSettings ( QWidget *parent, const char *name )
	: QWidget ( parent, name )
{
	m_menu_changed = false;
	m_busy_changed = false;

	QGridLayout *lay = new QGridLayout ( this, 0, 0, 4, 4 );
	lay-> addColSpacing ( 0, 16 );

	m_omenu = new QCheckBox ( tr( "Show O-Menu" ), this );
	lay-> addMultiCellWidget ( m_omenu, 0, 0, 0, 1 );

	m_omenu_tabs = new QCheckBox ( tr( "Add Launcher tabs to O-Menu" ), this );
	lay-> addWidget ( m_omenu_tabs, 1, 1 );

	m_omenu_home = new QCheckBox ( tr( "Add 'Home' shortcut to O-Menu" ), this );
	lay-> addWidget ( m_omenu_home, 2, 1 );

	m_omenu_suspend = new QCheckBox ( tr( "Add 'Suspend' shortcut to O-Menu" ), this );
	lay-> addWidget ( m_omenu_suspend, 3, 1 );

	QWhatsThis::add(  m_omenu_tabs, tr( "Adds the contents of the Launcher as menus in the O-Menu." ));
	QWhatsThis::add(  m_omenu, tr( "Check if you want the O-Menu in the taskbar." ));

	connect ( m_omenu, SIGNAL( toggled ( bool )), m_omenu_tabs, SLOT( setEnabled ( bool )));
	connect ( m_omenu, SIGNAL( toggled ( bool )), m_omenu_home, SLOT( setEnabled ( bool )));
	connect ( m_omenu, SIGNAL( toggled ( bool )), m_omenu_suspend, SLOT( setEnabled ( bool )));

	connect ( m_omenu, SIGNAL( toggled ( bool )), this, SLOT( menuChanged ( )));
	connect ( m_omenu_tabs, SIGNAL( toggled ( bool )), this, SLOT( menuChanged ( )));
	connect ( m_omenu_home, SIGNAL( toggled ( bool )), this, SLOT( menuChanged ( )));
	connect ( m_omenu_suspend, SIGNAL( toggled ( bool )), this, SLOT( menuChanged ( )));	

	lay-> addRowSpacing ( 4, 8 );

	m_busy = new QCheckBox ( tr( "Enable blinking busy indicator" ), this );
	lay-> addMultiCellWidget ( m_busy, 5, 5, 0, 1 );

	connect ( m_busy, SIGNAL( toggled( bool )), this, SLOT( busyChanged ( )));
	
	lay-> setRowStretch ( 6, 10 );

	init ( );
}

void GuiSettings::init ( )
{
	Config cfg ( "Taskbar" );
	cfg. setGroup ( "Menu" );

	m_omenu-> setChecked ( cfg. readBoolEntry ( "ShowMenu", true ));
	m_omenu_tabs-> setChecked ( cfg. readBoolEntry ( "LauncherTabs", true ));
	m_omenu_home-> setChecked ( cfg. readBoolEntry ( "Home", true ));
	m_omenu_suspend-> setChecked ( cfg. readBoolEntry ( "Suspend", true ));

	m_omenu_tabs-> setEnabled ( m_omenu-> isChecked ( ));
	m_omenu_home-> setEnabled ( m_omenu-> isChecked ( ));
	m_omenu_suspend-> setEnabled ( m_omenu-> isChecked ( ));
	
	Config cfg2 ( "Launcher" );
	cfg2. setGroup ( "GUI" );
	
	m_busy-> setChecked ( cfg2. readEntry ( "BusyType" ). lower ( ) == "blink" );
}

void GuiSettings::menuChanged()
{
	m_menu_changed = true;
}

void GuiSettings::busyChanged()
{
	m_busy_changed = true;
}

void GuiSettings::accept ( )
{
	Config cfg ( "Taskbar" );
	cfg. setGroup ( "Menu" );

	if ( m_menu_changed ) {
		cfg. writeEntry ( "ShowMenu", m_omenu-> isChecked ( ));
		cfg. writeEntry ( "LauncherTabs", m_omenu_tabs-> isChecked ( ));
		cfg. writeEntry ( "Home", m_omenu_home-> isChecked ( ));
		cfg. writeEntry ( "Suspend", m_omenu_suspend-> isChecked ( ));		
	}	
	cfg. write ( );

	if ( m_menu_changed )
		QCopEnvelope ( "QPE/TaskBar", "reloadApplets()" );
	
	Config cfg2 ( "Launcher" );
	cfg2. setGroup ( "GUI" );

	QString busytype = QString ( m_busy-> isChecked ( ) ? "blink" : "" );	
	
	if ( m_busy_changed ) {
		
		cfg2. writeEntry ( "BusyType", busytype );	
	}
	
	cfg2. write ( );
	
	if ( m_busy_changed ) {	
		QCopEnvelope e ( "QPE/Launcher", "setBusyIndicatorType(QString)" );
		e << busytype;
	}
}

