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

#include "tabssettings.h"

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/applnk.h>
#include <qpe/mimetype.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

#include <qlistview.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <stdlib.h>

TabsSettings::TabsSettings ( QWidget *parent, const char *name )
	: QWidget ( parent, name )
{
	QBoxLayout *lay = new QVBoxLayout ( this, 4, 4 );

	QLabel *l = new QLabel ( tr( "Launcher Tabs:" ), this );
	lay-> addWidget ( l );

	m_list = new QListView ( this );
	m_list-> addColumn ( "foobar" );
	m_list-> header ( )-> hide ( );

	lay-> addWidget ( m_list );

	QWhatsThis::add ( m_list, tr( "foobar" ));

	init ( );
}

void TabsSettings::init ( )
{
	QListViewItem *lvi;

	AppLnkSet rootFolder( MimeType::appsFolderName ( ));
	QStringList types = rootFolder. types ( );
	for ( QStringList::Iterator it = types. begin ( ); it != types. end ( ); ++it ) {
		lvi = new QListViewItem ( m_list, rootFolder. typeName ( *it ));
		lvi-> setPixmap ( 0, rootFolder. typePixmap ( *it ));
		m_ids << *it;
	}
	QImage img ( Resource::loadImage ( "DocsIcon" ));
	QPixmap pix;
	pix = img. smoothScale ( AppLnk::smallIconSize ( ), AppLnk::smallIconSize ( ));
	lvi = new QListViewItem ( m_list, tr( "Documents" ));
	lvi-> setPixmap ( 0, pix );
	m_ids += "Documents"; // No tr

	readTabSettings ( );
}

void TabsSettings::readTabSettings ( )
{
	Config cfg ( "Launcher" );
	QString grp ( "Tab %1" ); // No tr
	m_tabs. clear ( );

	for ( QStringList::Iterator it = m_ids. begin ( ); it != m_ids. end ( ); ++it ) {
		TabSettings ts;
		ts. m_view    = Icon;
		ts. m_bg_type = Ruled;
		ts. m_changed = false;

		cfg. setGroup ( grp. arg ( *it ));

		QString view = cfg. readEntry ( "View", "Icon" );
		if ( view == "List" ) // No tr
			ts. m_view = List;

		QString bgType = cfg. readEntry ( "BackgroundType", "Ruled" );
		if ( bgType == "SolidColor" )
			ts. m_bg_type = SolidColor;
		else if ( bgType == "Image" ) // No tr
			ts. m_bg_type = Image;

		ts. m_bg_image = cfg. readEntry ( "BackgroundImage", "wallpaper/opie" );
		ts. m_bg_color = cfg. readEntry ( "BackgroundColor" );
		ts. m_text_color = cfg. readEntry ( "TextColor" );
		QStringList f = cfg. readListEntry ( "Font", ',' );
		if ( f. count ( ) == 4 ) {
			ts. m_font_family = f [0];
			ts. m_font_size = f [1]. toInt ( );
		} else {
			ts. m_font_family = font ( ). family ( );
			ts. m_font_size = font ( ). pointSize ( );
		}
		m_tabs [*it] = ts;
	}
}


void TabsSettings::accept ( )
{
	Config cfg ( "Launcher" );

	// Launcher Tab
	QString grp ( "Tab %1" ); // No tr

	for ( QStringList::Iterator it = m_ids. begin ( ); it != m_ids. end ( ); ++it ) {
		TabSettings &tab = m_tabs [*it];

		cfg. setGroup ( grp. arg ( *it ));
		if ( !tab. m_changed )
			continue;
		switch ( tab. m_view ) {
		case Icon:
			cfg.writeEntry ( "View", "Icon" );
			break;
		case List:
			cfg.writeEntry ( "View", "List" );
			break;
		}

		QCopEnvelope e ( "QPE/Launcher", "setTabView(QString,int)" );
		e << *it << tab. m_view;

		cfg. writeEntry ( "BackgroundImage", tab. m_bg_image );
		cfg. writeEntry ( "BackgroundColor", tab. m_bg_color );
		cfg. writeEntry ( "TextColor", tab. m_text_color );

		QString f = tab. m_font_family + "," + QString::number ( tab. m_font_size ) + ",50,0";
		cfg. writeEntry ( "Font", f );
		QCopEnvelope be ( "QPE/Launcher", "setTabBackground(QString,int,QString)" );

		switch ( tab. m_bg_type ) {
		case Ruled:
			cfg.writeEntry( "BackgroundType", "Ruled" );
			be << *it << tab. m_bg_type << QString("");
			break;
		case SolidColor:
			cfg.writeEntry( "BackgroundType", "SolidColor" );
			be << *it << tab. m_bg_type << tab. m_bg_color;
			break;
		case Image:
			cfg.writeEntry( "BackgroundType", "Image" );
			be << *it << tab. m_bg_type << tab. m_bg_image;
			break;
		}

		QCopEnvelope te( "QPE/Launcher", "setTextColor(QString,QString)" );
		te << *it << tab. m_text_color;

		QCopEnvelope fe ( "QPE/Launcher", "setFont(QString,QString,int,int,int)" );
		fe << *it << tab. m_font_family;
		fe << tab. m_font_size;
		fe << 50 << 0;

		tab. m_changed = false;
	}
}

