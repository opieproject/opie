/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 OPIE team <opie@handhelds.org?>
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

#include "settingstabwidget.h"

#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qspinbox.h>

SettingsTabWidget::SettingsTabWidget( QWidget *parent )
	: QWidget( parent, 0x0, 0 )
{
	QVBoxLayout *tmpvb = new QVBoxLayout( this );
	QScrollView *sv = new QScrollView( this );
	tmpvb->addWidget( sv, 0, 0 );
	sv->setResizePolicy( QScrollView::AutoOneFit );
	sv->setFrameStyle( QFrame::NoFrame );
	QWidget *container = new QWidget( sv->viewport() );
	sv->addChild( container );
    
	QGridLayout *layout = new QGridLayout( container );
	layout->setMargin( 2 );
	layout->setSpacing( 4 );

	// Time server selector
	layout->addWidget( new QLabel( tr( "Time server" ), container ), 0, 0 );
	cbTimeServer = new QComboBox( TRUE, container );
	layout->addMultiCellWidget( cbTimeServer, 1, 1, 0, 1 );

	// Lookup delay selector
	layout->addWidget( new QLabel( tr( "minutes between time updates" ), container ), 2, 1 );
	sbNtpDelay = new QSpinBox( 1, 9999999, 1, container );
	sbNtpDelay->setWrapping( TRUE );
	sbNtpDelay->setMaximumWidth( 50 );
	connect( sbNtpDelay, SIGNAL(valueChanged(int)), this, SIGNAL(ntpDelayChanged(int)) );
	layout->addWidget( sbNtpDelay, 2, 0 );

	// Prediction delay selector
	layout->addWidget( new QLabel( tr( "minutes between prediction updates" ), container ), 3, 1 );
	sbPredictDelay = new QSpinBox( 42, 9999999, 1, container );
	sbPredictDelay->setWrapping( TRUE );
	sbPredictDelay->setMaximumWidth( 50 );
	layout->addWidget( sbPredictDelay, 3, 0 );

	// Space filler
	layout->addItem( new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ), 4, 0 );

	// Display time server information selector
	chNtpTab = new QCheckBox( tr( "Display time server information" ), container );
	connect( chNtpTab, SIGNAL( toggled(bool) ), this, SIGNAL( displayNTPTab(bool) ) );
	layout->addMultiCellWidget( chNtpTab, 5, 5, 0, 1 );

	// Display time prediction information selector
	chPredictTab = new QCheckBox( tr( "Display time prediction information" ), container );
	connect( chPredictTab, SIGNAL( toggled(bool) ), this, SIGNAL( displayPredictTab(bool) ) );
	layout->addMultiCellWidget( chPredictTab, 6, 6, 0, 1 );

	// Space filler
	layout->addItem( new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ), 7, 0 );

	// Initialize values
	QString ntpSrvsFile = QPEApplication::qpeDir();
	ntpSrvsFile.append( "etc/ntpservers" );
	Config ntpSrvs( ntpSrvsFile, Config::File );
	ntpSrvs.setGroup( "servers" );
	int srvCount = ntpSrvs.readNumEntry( "count", 0 );
	for ( int i = 0; i < srvCount; i++ )
	{
		ntpSrvs.setGroup( QString::number( i ) );
		cbTimeServer->insertItem( ntpSrvs.readEntry( "name" ) );
	}
	if ( srvCount==0 )
		cbTimeServer->insertItem( "time.fu-berlin.de" );

	Config config( "ntp" );
	config.setGroup( "settings" );
	sbPredictDelay->setValue( config.readNumEntry( "minLookupDiff", 720 ) );
	sbNtpDelay->setValue( config.readNumEntry( "ntpRefreshFreq", 1440 ) );
	cbTimeServer->setCurrentItem( config.readNumEntry( "ntpServer", 0 ) );
	chNtpTab->setChecked( config.readBoolEntry( "displayNtpTab", FALSE ) );
	chPredictTab->setChecked( config.readBoolEntry( "displayPredictTab", FALSE ) );
}

SettingsTabWidget::~SettingsTabWidget()
{
}

void SettingsTabWidget::saveSettings()
{
	int srvCount = cbTimeServer->count();
	bool serversChanged = TRUE;
	int curSrv = cbTimeServer->currentItem();
	QString edit = cbTimeServer->currentText();
	for ( int i = 0; i < srvCount; i++ )
	{
		if ( edit == cbTimeServer->text( i ) )
			serversChanged = FALSE;
	}
	if ( serversChanged )
	{
		QString ntpSrvsFile = QPEApplication::qpeDir();
		ntpSrvsFile.append( "etc/ntpservers" );
		Config ntpSrvs( ntpSrvsFile, Config::File );
		ntpSrvs.setGroup( "servers" );
		ntpSrvs.writeEntry( "count", ++srvCount );
		ntpSrvs.setGroup( "0" );
		ntpSrvs.writeEntry( "name", edit );
		curSrv = 0;
		for ( int i = 1; i < srvCount; i++ )
		{
//			qDebug( "ntpSrvs[%i/%i]=%s", i, srvCount, cbTimeServer->text( i ).latin1() );
			ntpSrvs.setGroup( QString::number( i ) );
			ntpSrvs.writeEntry( "name", cbTimeServer->text( i-1 ) );
		}
	}
	Config config( "ntp", Config::User );
	config.setGroup( "settings" );
	config.writeEntry( "ntpServer", curSrv );
	config.writeEntry( "minLookupDiff", sbPredictDelay->value() );
	config.writeEntry( "ntpRefreshFreq", sbNtpDelay->value() );
	config.writeEntry( "displayNtpTab", chNtpTab->isChecked() );
	config.writeEntry( "displayPredictTab", chPredictTab->isChecked() );
}

QString SettingsTabWidget::ntpServer()
{
	return cbTimeServer->currentText();
}
