/*
               =.            This file is part of the OPIE Project
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

#include <qlayout.h>
#include <qapplication.h>

#include <opie/otabwidget.h>

#include "launchersettings.h"
#include "tabssettings.h"
#include "menusettings.h"
#include "taskbarsettings.h"


LauncherSettings::LauncherSettings ( ) : QDialog ( 0, "LauncherSettings", false )
{
	setCaption ( tr( "Launcher Settings" ));

	QVBoxLayout *lay = new QVBoxLayout ( this, 4, 4 );

	OTabWidget *tw = new OTabWidget ( this, "otab" );
	lay-> addWidget ( tw );

	m_tabs = new TabsSettings ( tw );
	m_taskbar = new TaskbarSettings ( tw );
	m_menu = new MenuSettings ( tw );

	tw-> addTab ( m_taskbar, "launchersettings/taskbartab.png", tr( "Taskbar" ));
	tw-> addTab ( m_menu, "launchersettings/menutab.png", tr( "O-Menu" ));
	tw-> addTab ( m_tabs, "launchersettings/tabstab.png", tr( "Tabs" ));
	
	tw-> setCurrentTab ( m_taskbar );
}

void LauncherSettings::accept ( )
{
	m_taskbar-> accept ( );
	m_menu-> accept ( );
	m_tabs-> accept ( );

	QDialog::accept ( );
}

void LauncherSettings::done ( int r )
{
	QDialog::done ( r );
	close ( );
}
