
#include "packagelistremote.h"

#include <qstring.h>
#include <stdlib.h>
#include <unistd.h>

PackageListRemote::PackageListRemote(PackageManagerSettings* s)
	: PackageList(s)
{
	PackageListRemote();
}

PackageListRemote::PackageListRemote()
	: PackageList()
{
}

PackageListRemote::~PackageListRemote()
{
}

void PackageListRemote::query(QString s)
{
 	int r=0;
  QString cmd = "wget";
  QString redirect = "/tmp/oipkg.query";

  // use file for output
  cmd += " --output-document="+redirect;
//http://ipkgfind.handhelds.org/packages.phtml?format=pda&query=ipkg&searchtype=package&section=
	QString server="http://ipkgfind.handhelds.org/"; 	
  cmd += " \""+server+"/packages.phtml";
  cmd += "?format=pda&searchtype=package&section=";
  cmd += "&query="+s;           	
  cmd += "\"";

  pvDebug(2,"search :"+cmd);
  r = system(cmd.latin1());
  readFileEntries( redirect );
 	
}

void PackageListRemote::update()
{
	pvDebug(2,"PackageListRemote::update\ndoing nothing ");	
}
