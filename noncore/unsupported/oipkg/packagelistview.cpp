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
