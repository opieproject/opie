/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

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
	pvDebug(4,"set query "+s);
	searchString = s;	
}

void PackageListRemote::update()
{
	pvDebug(2,"PackageListRemote::update");	
 	if (searchString.isEmpty()) return;
 	int r=0;
  QString cmd = "wget";
  QString redirect = "/tmp/oipkg.query";

  // use file for output
  cmd += " --output-document="+redirect;
//http://ipkgfind.handhelds.org/packages.phtml?format=pda&query=ipkg&searchtype=package&section=
	QString server="http://ipkgfind.handhelds.org/"; 	
  cmd += " \""+server+"/packages.phtml";
  cmd += "?format=pda&searchtype=package&section=";
  cmd += "&query="+searchString;           	
  cmd += "\"";

  pvDebug(4,"search :"+cmd);
  r = system(cmd.latin1());
  readFileEntries( redirect, "remote" );
}
