/***************************************************************************
   begin                    : January 2003
   copyright                : ( C ) 2003 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qdialog.h>
#include <qpe/config.h>
#include <qlayout.h>
#include <qpe/config.h>

#include <qstring.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlineedit.h>

#include "ohighscoredlg.h"

OHighscore::OHighscore( int score )
{
	getList();
	checkIfItIsANewhighscore( score );
}

OHighscore::~OHighscore()
{
	std::list<t_playerData*>::iterator deleteIterator = playerData.begin();
	for ( ; deleteIterator != playerData.end() ; deleteIterator++ )
	{
		delete ( *deleteIterator );
	}
}

void OHighscore::getList()
{
	Config cfg ( "tetrix" );
	cfg.setGroup( QString::number( 1 ) );
	lowest = cfg.readNumEntry( "Points" );
	playerData.clear();

	int rest = 1;	//for the filling up later

	for ( int i = 1 ; i < 11 ; i++ )
	{
		if ( cfg.hasGroup( QString::number( i ) ) )
		{
			cfg.setGroup( QString::number( i ) );
			int temp = cfg.readNumEntry( "Points" );

			t_playerData *pPlayerData = new t_playerData;
			pPlayerData->sName = cfg.readEntry( "Name" );
			pPlayerData->points = temp;

			playerData.push_back( pPlayerData );

			if ( (temp < lowest) ) lowest = temp;
			rest++;
		}
	}

	//now I fill up the rest of the list
	if ( rest < 11 ) //only go in this loop if there are less than
					 //10 highscoreentries
	{
		lowest = 0;
		for ( ; rest < 11 ; rest++ )
		{
			t_playerData *pPlayerData = new t_playerData;
			pPlayerData->sName = tr( "empty");
			pPlayerData->points = 0;

			playerData.push_back( pPlayerData );
		}
	}

}

void OHighscore::checkIfItIsANewhighscore( int points)
{
	if ( points > lowest )
		isNewhighscore = true; 
	else 
		isNewhighscore = false;
}

void OHighscore::insertData( QString name , int punkte )
{
	Config cfg ( "tetrix" );
	int entryNumber = 1;
	std::list<t_playerData*>::iterator insertIterator = playerData.begin();
	while ( insertIterator != playerData.end() )
	{
		if ( punkte > ( *insertIterator )->points )
		{
			t_playerData* temp = new t_playerData;
			temp->sName = name;
			temp->points = punkte;
			playerData.insert( insertIterator , temp );
			
			//now we have to delete the last entry
			insertIterator = playerData.end();
			insertIterator--;
//X 		delete *insertIterator;              //memleak?
			playerData.erase( insertIterator );
			
		/////////////////////////////////////////
		//this block just rewrites the highscore
			insertIterator = playerData.begin();
			while ( insertIterator != playerData.end() )
			{
				cfg.setGroup( QString::number( entryNumber ) );
				cfg.writeEntry( "Name" , ( *insertIterator )->sName );
				cfg.writeEntry( "Points" , ( *insertIterator )->points );
				entryNumber++;	
				insertIterator++;
			}
		////////////////////////////////////////	

			return;
		}
		insertIterator++;
	} 
}

QString OHighscore::getName()
{
	QString name;
	QDialog *d = new QDialog ( this, 0, true );
	d->setCaption( tr( "Enter your name!" ));
	QLineEdit *ed = new QLineEdit ( d );
	( new QVBoxLayout ( d, 3, 3 ))->addWidget ( ed );
	ed->setFocus ( );

    if ( d->exec() == QDialog::Accepted ) {
        name = ed->text();
    }
    //delete d;
	return name;
}

OHighscoreDialog::OHighscoreDialog(OHighscore *highscore, QWidget *parent, const char *name, bool modal) : QDialog(parent, name,modal)
{
	hs_ = highscore;
	setCaption( tr( "Highscores" ) );
	vbox_layout = new QVBoxLayout( this, 4 , 4 );
	list = new QListView( this );
	list->setSorting( -1 );
	list->addColumn( tr( "Position" ));
	list->addColumn( tr( "Name" ));
	list->addColumn( tr( "Points" ));

	createHighscoreListView();
	
	vbox_layout->addWidget( list );
	showMaximized();
}

void OHighscoreDialog::createHighscoreListView()
{
	int pos = 10;
	int points_ = 0;

	std::list<t_playerData*>::reverse_iterator iListe = hs_->playerData.rbegin();
	
	for ( ; iListe != hs_->playerData.rend() ; ++iListe )
	{
		QListViewItem *item = new QListViewItem( list );
		item->setText(  0 , QString::number( pos ) );                   //number
		item->setText(  1 , ( *iListe )->sName );                       //name
		if ( (  *iListe )->points  == -1 )
			points_ = 0;
		else points_ =  ( *iListe )->points;
		item->setText(  2 , QString::number( points_ ) );   //points
		pos--;
	}
}

