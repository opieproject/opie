#include "packagelistitem.h"

#include <qpe/resource.h>
#include <qobject.h>

#include "debug.h"

static QPixmap *pm_uninstalled=0;
static QPixmap *pm_uninstalled_old=0;
static QPixmap *pm_installed=0;
static QPixmap *pm_installed_old=0;
static QPixmap *pm_uninstall=0;
static QPixmap *pm_install=0;
static QPixmap *pm_uninstalled_old_installed_new=0;
static QPixmap *pm_uninstalled_installed_old=0;

PackageListItem::PackageListItem(QListView* lv, Package *pi, PackageManagerSettings *s)
  :	QCheckListItem(lv,pi->name(),CheckBox)
{
 	init(pi,s);
}

PackageListItem::PackageListItem(QListViewItem *lvi, Package *pi, PackageManagerSettings *s)
  :	QCheckListItem(lvi,pi->name(),CheckBox)
{
 	init(pi,s);
}

void PackageListItem::init( Package *pi, PackageManagerSettings *s)
{
  package = pi;
  settings = s;
  setExpandable( true );
	QCheckListItem *item;
	nameItem = new QCheckListItem( this, "" );
	item = new QCheckListItem( this, QObject::tr("Description: ")+pi->desc() );
 	item = new QCheckListItem( this, QObject::tr("Size: ")+pi->size() );
	destItem = new QCheckListItem( this, "" );
	linkItem = new QCheckListItem( this, "" );
	statusItem = new QCheckListItem( this, "" );
 	QCheckListItem *otherItem = new QCheckListItem( this, QObject::tr("other") );
	item = new QCheckListItem( otherItem, QObject::tr("Install Name: ")+pi->installName() );
  QDict<QString> *fields = pi->getFields();
  QDictIterator<QString> it( *fields );
	while ( it.current() ) {
   	item = new QCheckListItem( otherItem, QString(it.currentKey()+": "+*it.current()) );
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
  bool versions = package->hasVersions();
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
