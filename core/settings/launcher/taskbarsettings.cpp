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

#include "taskbarsettings.h"

#include <qpe/config.h>
#include <qpe/qlibrary.h>
#include <qpe/qpeapplication.h>
#include <qpe/taskbarappletinterface.h>
#include <qpe/qcopenvelope_qws.h>

#include <qdir.h>
#include <qlistview.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <stdlib.h>


TaskbarSettings::TaskbarSettings ( QWidget *parent, const char *name )
	: QWidget ( parent, name )
{
	m_applets_changed = false;

	QBoxLayout *lay = new QVBoxLayout ( this, 4, 4 );

	QLabel *l = new QLabel ( tr( "Load applets in Taskbar:" ), this );
	lay-> addWidget ( l );

	m_list = new QListView ( this );
	m_list-> addColumn ( "foobar" );
	m_list-> header ( )-> hide ( );

	lay-> addWidget ( m_list );

	QWhatsThis::add ( m_list, tr( "Check the applets that you want displayed in the Taskbar." ));

	connect ( m_list, SIGNAL( clicked ( QListViewItem * )), this, SLOT( appletChanged ( )));

	init ( );
}

void TaskbarSettings::init ( )
{
	Config cfg ( "Taskbar" );
	cfg. setGroup ( "Applets" );
	QStringList exclude = cfg. readListEntry ( "ExcludeApplets", ',' );

	QString path = QPEApplication::qpeDir ( ) + "/plugins/applets";
	QStringList list = QDir ( path, "lib*.so" ). entryList ( );

	for ( QStringList::Iterator it = list. begin ( ); it != list. end ( ); ++it ) {
		QString name;
		QPixmap icon;
		TaskbarNamedAppletInterface *iface = 0;

		QLibrary *lib = new QLibrary ( path + "/" + *it );
		lib-> queryInterface ( IID_TaskbarNamedApplet, (QUnknownInterface**) &iface );
		if ( iface ) {
			QString lang = getenv( "LANG" );
			QTranslator *trans = new QTranslator ( qApp );
			QString type = (*it). left ((*it). find ("."));
			QString tfn = QPEApplication::qpeDir ( ) + "/i18n/" + lang + "/" + type + ".qm";
			if ( trans-> load ( tfn ))
				qApp-> installTranslator ( trans );
			else
				delete trans;
			name = iface-> name ( );
			icon = iface-> icon ( );
			iface-> release ( );
		}
		if ( !iface ) {
			lib-> queryInterface ( IID_TaskbarApplet, (QUnknownInterface**) &iface );
			
			if ( iface ) {
				name = (*it). mid ( 3 );
				int sep = name. find( ".so" );
				if ( sep > 0 )
					name. truncate ( sep );
				sep = name. find ( "applet" );
				if ( sep == (int) name.length ( ) - 6 )
					name. truncate ( sep );
				name[0] = name[0]. upper ( );
				iface-> release ( );			
			}					
		}
		
		if ( iface ) {
			QCheckListItem *item;
			item = new QCheckListItem ( m_list, name, QCheckListItem::CheckBox );
			if ( !icon. isNull ( ))
				item-> setPixmap ( 0, icon );
			item-> setOn ( exclude. find ( *it ) == exclude. end ( ));
			m_applets [*it] = item;
		}
		lib-> unload ( );
		delete lib;
	}
}

void TaskbarSettings::appletChanged()
{
	m_applets_changed = true;
}

void TaskbarSettings::accept ( )
{
	Config cfg ( "Taskbar" );
	cfg. setGroup ( "Applets" );
	if ( m_applets_changed ) {
		QStringList exclude;
		QMap <QString, QCheckListItem *>::Iterator it;
		for ( it = m_applets. begin ( ); it != m_applets. end ( ); ++it ) {
			if ( !(*it)-> isOn ( ))
				exclude << it. key ( );
		}
		cfg. writeEntry ( "ExcludeApplets", exclude, ',' );
	}
	cfg. writeEntry ( "SafeMode", false );
	cfg. write ( );

	if ( m_applets_changed ) {
		QCopEnvelope ( "QPE/TaskBar", "reloadApplets()" );
		m_applets_changed = false;
	}
}

