
#include "packagelistdoclnk.h"

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qlist.h>

#include "package.h"
#include "pksettings.h"

PackageListDocLnk::PackageListDocLnk(PackageManagerSettings* s, QObject *parent=0, const char *name=0)
	: PackageList(s)
{
	PackageListDocLnk(parent, name);
}

PackageListDocLnk::PackageListDocLnk(QObject *parent=0, const char *name=0)
	: PackageList(parent, name)
{
	Config cfg( "oipkg", Config::User );
	cfg.setGroup( "Common" );
	docLnkDir = cfg.readEntry( "docLnkDir", "/root/" );
 	pvDebug(2,"opening DocLnkSet "+docLnkDir);
	doclnkset = new DocLnkSet(docLnkDir,"application/ipkg");
}

PackageListDocLnk::~PackageListDocLnk()
{
	Config cfg( "oipkg", Config::User );
	cfg.setGroup( "Common" );
	cfg.writeEntry( "docLnkDir", docLnkDir );
	delete doclnkset;
}



void PackageListDocLnk::update()
{
	pvDebug(2,"PackageListDocLnk::update ");	
	QList<DocLnk> packlist = doclnkset->children();
  for (DocLnk *pack =packlist.first(); pack != 0; pack=packlist.next() )
  {
    insertPackage( new Package(pack->file(), settings ) );
  }
}
