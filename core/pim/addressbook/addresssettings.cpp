/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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


#include "addresssettings.h"

#include <qpe/config.h>
#include <opie/ocontact.h>

#include <qfile.h>
#include <qlistbox.h>

#include <stdlib.h>

AddressSettings::AddressSettings( QWidget *parent, const char *name )
     : AddressSettingsBase( parent, name, TRUE )
{
    init();
}

AddressSettings::~AddressSettings()
{
}

void AddressSettings::init()
{
    QStringList slFields = OContact::trfields();
    // Make this match what is in initFields
    slFields.remove( tr("Name Title") );
    slFields.remove( tr("First Name") );
    slFields.remove( tr("Last Name") );
    slFields.remove( tr("File As") );
    slFields.remove( tr("Default Email") );
    slFields.remove( tr("Notes") );
    slFields.remove( tr("Gender") );


    for( QStringList::Iterator it = slFields.begin();
	 it != slFields.end(); ++it ) {
	fieldListBox->insertItem( *it );
    }

    Config cfg( "AddressBook" );

    cfg.setGroup( "Version" );
    int version;
    version = cfg.readNumEntry( "version" );
    if ( version >= ADDRESSVERSION ) {
	int i = 0;
	int p = 0;
	cfg.setGroup( "ImportantCategory" );
	QString zn = cfg.readEntry( "Category" + QString::number(i),
				    QString::null );
	while ( !zn.isNull() ) {
	    for ( int m = i; m < (int)fieldListBox->count(); m++ ) {
		if ( fieldListBox->text( m ) == zn ) {
		    if ( m != p ) {
			fieldListBox->removeItem( m );
			fieldListBox->insertItem( zn, p );
		    }
		    p++;
		    break;
		}
	    }
	    zn = cfg.readEntry( "Category" + QString::number(++i),
				QString::null );
	}

	fieldListBox->setCurrentItem( 0 );
    } else {
	QString str;
	str = getenv("HOME");

	str += "/Settings/AddressBook.conf";
	QFile::remove( str );
    }
}

void AddressSettings::itemUp()
{
    int i = fieldListBox->currentItem();
    if ( i > 0 ) {
	QString item = fieldListBox->currentText();
	fieldListBox->removeItem( i );
	fieldListBox->insertItem( item, i-1 );
	fieldListBox->setCurrentItem( i-1 );
    }
}

void AddressSettings::itemDown()
{
    int i = fieldListBox->currentItem();
    if ( i < (int)fieldListBox->count() - 1 ) {
	QString item = fieldListBox->currentText();
	fieldListBox->removeItem( i );
	fieldListBox->insertItem( item, i+1 );
	fieldListBox->setCurrentItem( i+1 );
    }
}

void AddressSettings::accept()
{
    save();
    QDialog::accept();
}


void AddressSettings::save()
{
    Config cfg( "AddressBook" );
    cfg.setGroup( "Version" );
    // *** To change the version change it here...
    cfg.writeEntry( "version", QString::number(ADDRESSVERSION) );
    cfg.setGroup( "ImportantCategory" );

    for ( int i = 0; i < (int)fieldListBox->count(); i++ ) {
	cfg.writeEntry( "Category"+QString::number(i), fieldListBox->text(i) );
    }
}
