/***************************************************************************
                          packagelistview.cpp  -  description
                             -------------------
    begin                : Sat Apr 27 2002
    copyright            : (C) 2002 by tille
    email                : tille@handhelds.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "packagelistview.h"

#include <qpopupmenu.h>
#include <qaction.h>

#include "packagelistitem.h"
#include "pksettings.h"

PackageListView::PackageListView(QWidget *p, const char* n, PackageManagerSettings *s)
	: QListView(p,n)
{
	settings = s;
	popupMenu = new QPopupMenu( this );	
	destsMenu = new QPopupMenu( popupMenu );
  popupTimer = new QTimer( this );
  setSelectionMode(QListView::NoSelection);
	addColumn( tr("Package") );
 	setRootIsDecorated( true );

  connect( popupTimer, SIGNAL(timeout()),
  		this, SLOT(showPopup()) );
  connect( this, SIGNAL( pressed( QListViewItem* ) ),
	    this, SLOT( setCurrent( QListViewItem* ) ) );
	connect( this, SIGNAL( clicked( QListViewItem* ) ),
	   this, SLOT( stopTimer( QListViewItem* ) ) );	

}

//PackageListView::~PackageListView()
//{
//}

void PackageListView::setCurrent( QListViewItem* p )
{
  if ( !p ) return;
  activePackageListItem = (PackageListItem*)p;
  activePackage = activePackageListItem->getPackage();
  if (!activePackage) return;
  popupTimer->start( 750, true );
}


void PackageListView::showPopup()
{
  popupMenu->clear();
  destsMenu->clear();
 		
 	QAction *popupAction;
  if ( !activePackage->installed() )
  {
	  popupMenu->insertItem( QObject::tr("Install to"), destsMenu );
  	QStringList dests = settings->getDestinationNames();
   	QString ad = settings->getDestinationName();
	  for (uint i = 0; i < dests.count(); i++ )
  	{		
	  	popupAction = new QAction( dests[i], QString::null, 0, this, 0 );
		  popupAction->addTo( destsMenu );
    	if ( dests[i] == ad && activePackage->toInstall() )
     	{
        popupAction->setToggleAction( true );
			  popupAction->setOn(true);
  		};
  	}
	  connect( destsMenu, SIGNAL( activated( int ) ),
		   this, SLOT( changePackageDest( int ) ) );
   }else{
//   	popupActcat setOn( activePackage->toProcess() );
   }
  popupMenu->popup( QCursor::pos() );
}

void PackageListView::stopTimer( QListViewItem* )
{
	popupTimer->stop();
}


void PackageListView::changePackageDest( int i )
{
 	activePackage->setDest( destsMenu->text(i) );
  activePackage->setOn();
  activePackage->setLink( settings->createLinks() );
  activePackageListItem->displayDetails();
}

void PackageListView::toggleProcess()
{
  activePackage->toggleProcess() ;
  activePackageListItem->displayDetails();
}
