/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

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

	QLabel *l = new QLabel ( tr( "Load applets:" ), this );
	lay-> addWidget ( l );

	m_list = new QListView ( this );
	m_list-> addColumn ( "foobar" );
	m_list-> header ( )-> hide ( );

	lay-> addWidget ( m_list );

	m_omenu = new QCheckBox ( tr( "Show O-Menu" ), this );
	lay-> addWidget ( m_omenu );

	m_omenu_tabs = new QCheckBox ( tr( "Show Launcher tabs in O-Menu" ), this );
	lay-> addWidget ( m_omenu_tabs );

	QWhatsThis::add ( m_list, tr( "Check the applets that you want displayed in the Taskbar." ));
	QWhatsThis::add(  m_omenu_tabs, tr( "Adds the contents of the Launcher as menus in the O-Menu." ));
	QWhatsThis::add(  m_omenu, tr( "Check if you want the O-Menu in the taskbar." ));

	connect ( m_list, SIGNAL( clicked ( QListViewItem * )), this, SLOT( appletChanged ( )));
	connect ( m_omenu, SIGNAL( toggled ( bool )), m_omenu_tabs, SLOT( setEnabled ( bool )));

	// This option does not make sense ! (sandman)
	m_omenu_tabs-> hide ( );

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
			lib-> unload ( );
		} else {
			delete lib;
			name = (*it). mid ( 3 );
			int sep = name. find( ".so" );
			if ( sep > 0 )
                            name. truncate ( sep );
			sep = name. find ( "applet" );
			if ( sep == (int) name.length ( ) - 6 )
				name. truncate ( sep );
			name[0] = name[0]. upper ( );
		}
		QCheckListItem *item;
		item = new QCheckListItem ( m_list, name, QCheckListItem::CheckBox );
		if ( !icon. isNull ( ))
			item-> setPixmap ( 0, icon );
		item-> setOn ( exclude. find ( *it ) == exclude. end ( ));
		m_applets [*it] = item;
	}

	cfg. setGroup ( "Menu" );

	m_omenu_tabs-> setChecked ( cfg. readBoolEntry ( "LauncherTabs", true ));
	m_omenu-> setChecked ( cfg. readBoolEntry ( "ShowMenu", true ));

	m_omenu_tabs-> setEnabled ( m_omenu-> isChecked ( ));
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

	cfg. setGroup ( "Menu" );

	if ( m_omenu_tabs-> isChecked ( ) != cfg. readBoolEntry ( "LauncherTabs", true )) {
		m_applets_changed = true;
		cfg. writeEntry ( "LauncherTabs", m_omenu_tabs-> isChecked ( ));
	}

	if ( m_omenu-> isChecked ( ) != cfg. readBoolEntry ( "ShowMenu", true )) {
		m_applets_changed = true;
		cfg. writeEntry ( "ShowMenu", m_omenu-> isChecked ( ));
	}
	cfg. write ( );

	if ( m_applets_changed ) {
		QCopEnvelope ( "QPE/TaskBar", "reloadApplets()" );
		m_applets_changed = false;
	}
}

