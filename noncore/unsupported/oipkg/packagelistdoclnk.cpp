
#include "packagelistdoclnk.h"

#include <qpe/applnk.h>
#include <qlist.h>

#include "package.h"
#include "pksettings.h"

PackageListDocLnk::PackageListDocLnk(PackageManagerSettings* s)
	: PackageList(s)
{
	PackageListDocLnk();
}

PackageListDocLnk::PackageListDocLnk()
	: PackageList()
{
	doclnkset = new DocLnkSet("/mnt/nfs/ipk","application/ipkg");
}

PackageListDocLnk::~PackageListDocLnk()
{
}



void PackageListDocLnk::update()
{
	pvDebug(2,"PackageListDocLnk::update ");	
 	QList<DocLnk> packlist = doclnkset->children();
  for (DocLnk *pack =packlist.first(); pack != 0; pack=packlist.next() )
  {
    insertPackage( new Package(pack->file(), settings) );
  }
}
