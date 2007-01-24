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
#include <qlayout.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qstring.h>
#include <qlist.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlineedit.h>

#include "ohighscoredlg.h"

OHighscore::OHighscore( int score , int playerLevel ) : playerData()
{
	pLevel = playerLevel;
	getList();
	checkIfItIsANewhighscore( score );
	playerData.setAutoDelete( TRUE );
}

OHighscore::~OHighscore()
{
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
			pPlayerData->level = cfg.readNumEntry( "Level" );

			playerData.append( pPlayerData );

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
			pPlayerData->level = 0;

			playerData.append( pPlayerData );
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

void OHighscore::insertData( QString name , int punkte , int playerLevel )
{
	Config cfg ( "tetrix" );
	t_playerData * Run;
	int index = 0;
	int entryNumber = 1;

        for ( Run=playerData.first(); 
              Run != 0; 
              index ++, Run=playerData.next() ) {

		if ( punkte > Run->points )
		{
			t_playerData* temp = new t_playerData;
			temp->sName = name;
			temp->points = punkte;
			temp->level = playerLevel;

			playerData.insert( index, temp );
			
			//now we have to delete the last entry
			playerData.remove( playerData.count() );
			
		/////////////////////////////////////////
		//this block just rewrites the highscore
                        for ( t_playerData * Run2=playerData.first(); 
                              Run2 != 0; 
                              Run2=playerData.next() ) {
				cfg.setGroup( QString::number( entryNumber ) );
				cfg.writeEntry( "Name" , Run2->sName );
				cfg.writeEntry( "Points" , Run2->points );
				cfg.writeEntry( "Level" , Run2->level );
				entryNumber++;	
			}
		////////////////////////////////////////	

			return;
		}
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

OHighscoreDialog::OHighscoreDialog(OHighscore *highscore, QWidget *parent, const char *name, bool modal) : QDialog(parent, name, modal)
{
	hs_ = highscore;
	setCaption( tr( "Highscores" ) );
	vbox_layout = new QVBoxLayout( this, 4 , 4 );
	list = new QListView( this );
	list->setSorting( -1 );
	list->addColumn( tr( "#" ));
	list->addColumn( tr( "Name" ));
	list->addColumn( tr( "Points" ));
	list->addColumn( tr( "Level" ));

	createHighscoreListView();
	
	vbox_layout->addWidget( list );
	QPEApplication::showDialog( this );
}

void OHighscoreDialog::createHighscoreListView()
{
	int pos = 10;
	int points_ = 0;
	int level_ = 0;

        for ( t_playerData * Run = hs_->playerData.first(); 
              Run != 0; 
              Run=hs_->playerData.next() )
        {
                QListViewItem *item;

                item = new QListViewItem( list );

		item->setText(  0 , QString::number( pos ) );                   //number
		item->setText(  1 , Run->sName );                       //name
		if ( Run->points  == -1 )
			points_ = 0;
		else points_ =  Run->points;
		if ( Run->level  == -1 )
                    level_ = 0;
		else level_ =  Run->level;
		item->setText(  2 , QString::number( points_ ) );   //points
		item->setText(  3 , QString::number( level_ ) );    //level
		pos--;
	}
}

