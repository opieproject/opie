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
  if (!activePackage)
  {
//		QDictIterator<QCheckListItem> it( rootItems );
//		while ( it.current() )
//    {
//			if ( it.current()==p )
//   			pvDebug(2,"current item");
//			++it;
//		}

		return;
  }
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
   	popupAction = new QAction( QObject::tr("Remove"),QString::null,  0, this, 0 );
		popupAction->addTo( popupMenu );
  	connect( popupAction, SIGNAL( activated() ),
	   	this , SLOT( toggleProcess() ) );
   	popupAction = new QAction( QObject::tr("Reinstall"),QString::null,  0, this, 0 );
		popupAction->addTo( popupMenu );
  	popupAction->setEnabled( false );
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

void PackageListView::display()
{
	QDictIterator<PackageList> list( PackageLists );
	PackageList *packlist;
	Package *pack;
  PackageListItem *item;
  QCheckListItem *rootItem;
  QListViewItem* it;
  QListViewItem* itdel;
	while ( list.current() ) {
  	packlist = list.current();
    rootItem = rootItems.find( list.currentKey() );
    //rootItem->clear();
    it=rootItem->firstChild();
    while ( it )
    {
      itdel = it;
      it    = it->nextSibling();
			delete itdel;
   	}
    pack = packlist->first();
  	while( pack )
  	{
	 		item = new PackageListItem( rootItem, pack, settings );				
    	pack = packlist->next();
  	}	
		++list;
  }
}

void PackageListView::addList( QString n, PackageList* pl)
{
	PackageLists.insert(n, pl);
 	QCheckListItem *item = new QCheckListItem(this,n);
 	rootItems.insert(n, item);
}
