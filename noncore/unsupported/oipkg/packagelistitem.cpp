/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include "packagelistitem.h"

#include <qpe/resource.h>
#include <qobject.h>
#include <qpopupmenu.h>
#include <qaction.h>

#include "debug.h"

static QPixmap *pm_uninstalled=0;
static QPixmap *pm_uninstalled_old=0;
static QPixmap *pm_installed=0;
static QPixmap *pm_installed_old=0;
static QPixmap *pm_uninstall=0;
static QPixmap *pm_install=0;
static QPixmap *pm_uninstalled_old_installed_new=0;
static QPixmap *pm_uninstalled_installed_old=0;

PackageListItem::PackageListItem(ListViewItemOipkg *parent, QString name, Type ittype)
  : ListViewItemOipkg(parent,name,ittype)
{

}

PackageListItem::PackageListItem(QListView* lv, OipkgPackage *pi, PackageManagerSettings *s)
  :	ListViewItemOipkg(lv,pi->name(),ListViewItemOipkg::Package)
{
  init(pi,s);
}

PackageListItem::PackageListItem(ListViewItemOipkg *lvi, OipkgPackage *pi, PackageManagerSettings *s)
  :	ListViewItemOipkg(lvi,pi->name(),ListViewItemOipkg::Package)
{
  init(pi,s);
}
PackageListItem::~PackageListItem()
{
  delete popupMenu;
  delete destsMenu;
}

void PackageListItem::init( OipkgPackage *pi, PackageManagerSettings *s)
{
  popupMenu = new QPopupMenu( 0 );
  destsMenu = new QPopupMenu( 0 );
  package = pi;
  settings = s;
  setExpandable( true );
  ListViewItemOipkg *item;
  nameItem = new ListViewItemOipkg( this, ListViewItemOipkg::Attribute,"name"  );
  item = new ListViewItemOipkg( this, ListViewItemOipkg::Attribute, QObject::tr("Description: ")+pi->desc()  );
  item = new ListViewItemOipkg( this, ListViewItemOipkg::Attribute, QObject::tr("Size: ")+pi->size() );
  destItem = new ListViewItemOipkg( this, ListViewItemOipkg::Attribute, "dest" );
  linkItem = new ListViewItemOipkg( this, ListViewItemOipkg::Attribute, "link" );
  statusItem = new ListViewItemOipkg( this, ListViewItemOipkg::Attribute, "status" );
  ListViewItemOipkg *otherItem = new ListViewItemOipkg( this, ListViewItemOipkg::Attribute, QObject::tr("other") );
  item = new ListViewItemOipkg( otherItem, ListViewItemOipkg::Attribute, QObject::tr("Install Name: ")+pi->installName() );
  QDict<QString> *fields = pi->getFields();
  QDictIterator<QString> it( *fields );
  while ( it.current() ) {
    item = new ListViewItemOipkg( otherItem, ListViewItemOipkg::Attribute, QString(it.currentKey()+": "+*it.current()) );
    ++it;
  }
  displayDetails();

  if (!pm_uninstalled)
    {
      pm_uninstalled = new QPixmap(Resource::loadPixmap("oipkg/uninstalled"));
      pm_uninstalled_old = new QPixmap(Resource::loadPixmap("oipkg/uninstalledOld"));
      pm_uninstalled_old_installed_new = new QPixmap(Resource::loadPixmap("oipkg/uninstalledOldinstalledNew"));
      pm_uninstalled_installed_old = new QPixmap(Resource::loadPixmap("oipkg/uninstalledInstalledOld"));
      pm_installed = new QPixmap(Resource::loadPixmap("oipkg/installed"));
      pm_installed_old = new QPixmap(Resource::loadPixmap("oipkg/installedOld"));
      pm_install = new QPixmap(Resource::loadPixmap("oipkg/install"));
      pm_uninstall = new QPixmap(Resource::loadPixmap("oipkg/uninstall"));
    }
}

void PackageListItem::paintCell( QPainter *p,  const QColorGroup & cg,
				 int column, int width, int alignment )
{
  if ( !p )
    return;

  p->fillRect( 0, 0, width, height(),
	       isSelected()? cg.highlight() : cg.base() );

  if ( column != 0 ) {
    // The rest is text
    QListViewItem::paintCell( p, cg, column, width, alignment );
    return;
  }

  QListView *lv = listView();
  if ( !lv )
    return;
  int marg = lv->itemMargin();
  int r = marg;

  QPixmap pm = statePixmap();
  p->drawPixmap(marg,(height()-pm.height())/2,pm);
  r += pm.width()+1;

  p->translate( r, 0 );
  QListViewItem::paintCell( p, cg, column, width - r, alignment );
}


void PackageListItem::paintFocus( QPainter *p, const QColorGroup & cg,
				  const QRect & r )
{
  // Skip QCheckListItem
  // (makes you wonder what we're getting from QCheckListItem)
  QListViewItem::paintFocus(p,cg,r);
}

QPixmap PackageListItem::statePixmap() const
{
  bool installed = package->installed();
  bool old = package->isOld();
  bool verinstalled = package->otherInstalled();
  if ( !package->toProcess() ) {
    if ( !installed )
      if (old)
     	{
	  if (verinstalled) return *pm_uninstalled_old_installed_new;
	  else return *pm_uninstalled_old;
	}
      else
	{
	  if (verinstalled) return *pm_uninstalled_installed_old;
	  else return *pm_uninstalled;
	}
    else
      if (old) return *pm_installed_old;
      else return *pm_installed;
  } else { //toProcess() == true
    if ( !installed )
      return *pm_install;
    else
      return *pm_uninstall;
  }
}

QString PackageListItem::key( int column, bool ascending ) const
{
  if ( column == 2 ) {
    QString t = text(2);
    double bytes=t.toDouble();
    if ( t.contains('M') ) bytes*=1024*1024;
    else if ( t.contains('K') || t.contains('k') ) bytes*=1024;
    if ( !ascending ) bytes=999999999-bytes;
    return QString().sprintf("%09d",(int)bytes);
  } else {
    return QListViewItem::key(column,ascending);
  }
}

void PackageListItem::setOn( bool b )
{
  QCheckListItem::setOn( b );
  package->toggleProcess();
  package->setLink( settings->createLinks() );
  displayDetails();
}

void PackageListItem::displayDetails()
{
  QString sod;
  sod += package->sizeUnits().isEmpty()?QString(""):QString(package->sizeUnits());
  //sod += QString(package->dest().isEmpty()?"":QObject::tr(" on ")+package->dest());
  sod += package->dest().isEmpty()?QString(""):QString(QObject::tr(" on ")+package->dest());
  sod = sod.isEmpty()?QString(""):QString(" ("+sod+")");
  setText(0, package->name()+sod );
  nameItem->setText( 0, QObject::tr("Name: ")+package->name());
  linkItem->setText( 0, QObject::tr("Link: ")+(package->link()?QObject::tr("Yes"):QObject::tr("No")));
  destItem->setText( 0, QObject::tr("Destination: ")+package->dest() );
  statusItem->setText( 0, QObject::tr("Status: ")+package->status() );
  repaint();
}

QPopupMenu* PackageListItem::getPopupMenu()
{
  popupMenu->clear();
  destsMenu->clear();

  QAction *popupAction;
  qDebug("PackageListItem::showPopup ");

  if (!package->installed()){
    popupMenu->insertItem( QObject::tr("Install to"), destsMenu );
    QStringList dests = settings->getDestinationNames();
    QString ad = settings->getDestinationName();
    for (uint i = 0; i < dests.count(); i++ )
  	{
     popupAction = new QAction( dests[i], QString::null, 0, popupMenu, 0 );
     popupAction->addTo( destsMenu );
     if ( dests[i] == ad && getPackage()->toInstall() )
	    {
	      popupAction->setToggleAction( true );
	      popupAction->setOn(true);
	    }
    }
  connect( destsMenu, SIGNAL( activated( int ) ),
	       this, SLOT( menuAction( int ) ) );
      popupMenu->popup( QCursor::pos() );
  }else{
    popupMenu->insertItem( QObject::tr("Remove"));
  connect( popupMenu, SIGNAL( activated( int ) ),
	       this, SLOT( menuAction( int ) ) );
      popupMenu->popup( QCursor::pos() );   
  }
  return popupMenu;
}

void PackageListItem::menuAction( int i )
{
  if (!package->installed()){
    qDebug("set dest[%i] -> %s",i, destsMenu->text(i).latin1());
    package->setDest( destsMenu->text(i) );
    package->setLink( settings->createLinks() );
  }
  package->setOn();
  displayDetails();
}

//void PackageListItem::toggleProcess()
//{
//  package->toggleProcess() ;
//  displayDetails();
//}