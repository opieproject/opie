/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include "packagelistview.h"

#include <qpopupmenu.h>
#include <qaction.h>

#include "listviewitemoipkg.h"
#include "packagelistitem.h"
#include "pksettings.h"

PackageListView::PackageListView(QWidget *p, const char* n, PackageManagerSettings *s)
  : QListView(p,n)
{
  settings = s;
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
  qDebug("PackageListView::setCurrent ");
  activeItem = (ListViewItemOipkg*)p;

  if ( activeItem != 0 ) popupTimer->start( 750, true );
  
//  if ( activeItem->getType() != ListViewItemOipkg::Package ){
//    qDebug("PackageListView::setCurrent !p ");
//    activePackage = 0;
//	  activePackageListItem = 0;
//    qDebug("PackageListView::setCurrent returning ");
//    return;
//  };
//  activePackageListItem = (PackageListItem*)p;
//  activePackage = activePackageListItem->getPackage();
//  if (activePackage == 0 )
//    {
//      qDebug("PackageListView::setCurrent  if (!activePackage)");
//      return;
//    }
  

  qDebug("PackageListView::setCurrent popupTimer->start");
}


void PackageListView::showPopup()
{
  qDebug("PackageListView::showPopup");
  QPopupMenu *popup = activeItem->getPopupMenu();
  if (popup == 0) return;
  popup->popup( QCursor::pos() );
  qDebug("PackageListView::showPopup");
}

void PackageListView::stopTimer( QListViewItem* )
{
  popupTimer->stop();
}


void PackageListView::display()
{
  QDictIterator<PackageList> list( PackageLists );
  PackageList *packlist;
  OipkgPackage *pack;
  PackageListItem *item;
  ListViewItemOipkg *rootItem;
  QListViewItem* it;
  QListViewItem* itdel;
  while ( list.current() ) {
    packlist = list.current();
    rootItem = static_cast<ListViewItemOipkg*>(packlist);
   // rootItem = rootItems.find( list.currentKey() );
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
//  rootItems.insert(n, static_cast<ListViewItemOipkg*>(pl));
//  ListViewItemOipkg *item = new ListViewItemOipkg(this,n,ListViewItemOipkg::Feed);
//  rootItems.insert(n, item);
}
