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
#include <qpushbutton.h>
#include <qstrlist.h>
#include <qvaluelist.h>
#include <qlistview.h>
#include <qlineedit.h>

#include <qmap.h>

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

	for ( int i = 1 ; i < 11 ; i++ )
	{
		cfg.setGroup( QString::number( i ) );
		int temp = cfg.readNumEntry( "Points" );
 		
		t_playerData* pPlayerData = new t_playerData;
		pPlayerData->sName = cfg.readEntry( "Name" );
		pPlayerData->points = temp;

		playerData.push_back( pPlayerData );

//X 		qDebug( QString::number( i ) );
//X 		qDebug( pPlayerData->sName );
//X 		qDebug( QString::number( pPlayerData->points ) );
		
		if ( (temp < lowest) ) lowest = temp;
	}
	//lowest contains the lowest value in the highscore
}

void OHighscore::checkIfItIsANewhighscore( int points)
{
	qDebug( "Die niedrigste Zahl ist" );
	qDebug( QString::number( lowest ) );
	if ( points > lowest ) {
		qDebug( "isNewhighscore == true" ) ; isNewhighscore = true;
	}
	else { 
		qDebug( "isNewhighscore == true" ) ;isNewhighscore = false;
	}
}

void OHighscore::insertData( QString name , int punkte )
{
	std::list<t_playerData*>::iterator insertIterator = playerData.begin();
	while ( insertIterator != playerData.end() )
	{
		if ( punkte > ( *insertIterator )->points )
		{
			t_playerData* temp = new t_playerData;
			temp->sName = name;
			temp->points = punkte;
			playerData.insert( insertIterator , temp );
			insertIterator = playerData.end();
			insertIterator--;
//X 			delete *insertIterator;              //memleak?
			playerData.erase( insertIterator );
			return;
		}
		insertIterator++;
	} 
}

void OHighscore::writeList()
{
	qDebug( "writeList()" );
}

QString OHighscore::getName()
{
	QString name;
	QDialog *d = new QDialog ( this, 0, true );
	d->setCaption( tr( "New highscore!\nEnter your name!" ));
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
	list->addColumn( "Position" );
	list->addColumn( "Name" );
	list->addColumn( "Zahl" );

	createHighscoreListView();
	
	vbox_layout->addWidget( list );
	showMaximized();
}

void OHighscoreDialog::createHighscoreListView()
{
	int pos = 1;
	std::list<t_playerData*>::iterator iListe = hs_->playerData.begin();
	
	for ( ; iListe != hs_->playerData.end() ; iListe++ )
	{
		QListViewItem *item = new QListViewItem(  list );
		item->setText(  0 , QString::number( pos ) );           //number
		item->setText(  1 , ( *iListe )->sName );                       //name
		item->setText(  2 , QString::number( ( *iListe )->points ) );   //points
		pos++;
	}
}

