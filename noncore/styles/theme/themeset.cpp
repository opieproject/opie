/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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



#include "themeset.h"

#include <qpe/qpeapplication.h>
#include <qpe/global.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qdir.h>

#include <qpe/config.h>


class MyConfig : public Config
{
public:
	MyConfig ( const QString &f, Domain d ) : Config ( f, d )
	{ }

	bool hasGroup ( const QString &gname ) const
	{
		QMap< QString, ConfigGroup>::ConstIterator it = groups. find ( gname );
		return ( it != groups.end() );
	}
};

class MyItem : public QListViewItem 
{
public:
	MyItem ( QListView *lv, const QString &name, const QString &comm, const QString &theme ) : QListViewItem ( lv, name, comm )
	{
		m_theme = theme;	
	}
	

	QString m_theme;
};


ThemeSettings::ThemeSettings ( QWidget* parent, const char *name, WFlags fl )
		: QWidget ( parent, name, fl )
{
	setCaption ( tr( "Theme Style" ) );
	
	Config config ( "qpe" );
    config. setGroup ( "Appearance" );

	QString active = config. readEntry ( "Theme", "default" );

	QVBoxLayout *vbox = new QVBoxLayout ( this );
	vbox-> setSpacing ( 3 );
	vbox-> setMargin ( 6 );

	vbox-> addWidget ( new QLabel ( tr( "Select the theme to be used" ), this ));

	m_list = new QListView ( this );
	m_list-> addColumn ( tr( "Name" ));
	m_list-> addColumn ( tr( "Description" ));
	m_list-> setSelectionMode ( QListView::Single );
	m_list-> setAllColumnsShowFocus ( true );
	vbox-> addWidget ( m_list, 10 );
		
	QListViewItem *item = new MyItem ( m_list, tr( "[No theme]" ), "", "" );
	m_list-> setSelected ( item, true );
	
	QString path = QPEApplication::qpeDir() + "/plugins/styles/themes";
	QStringList list = QDir ( path, "*.themerc" ). entryList ( );
	
	for ( QStringList::Iterator it = list. begin(); it != list. end ( ); ++it ) {
		MyConfig cfg ( path + "/" + *it, Config::File );
		
		if ( cfg. hasGroup ( "Misc" )) {
			cfg. setGroup ( "Misc" );
			
			QString name = cfg. readEntry ( "Name" );
			QString comm = cfg. readEntry ( "Comment" );
	
			if ( !name. isEmpty ( )) {	
				QString fname = (*it). left ((*it). length ( ) - 8 );
			
				item = new MyItem ( m_list, name, comm, fname );
				if ( active == fname ) {
					m_list-> setSelected ( item, true );
				}
			}
		}
	}
}

bool ThemeSettings::writeConfig ( )
{
	Config config ( "qpe" );
    config. setGroup ( "Appearance" );

	MyItem *it = (MyItem *) m_list-> selectedItem ( );
	config. writeEntry ( "Theme", it ? it-> m_theme : QString ( "" ));
	config. write ( );

	return true;
}

