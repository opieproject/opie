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
#include <qfile.h>
#include <stdlib.h>
#include <unistd.h>


PackageListRemote::PackageListRemote(QListView *parent, QString name)
	: PackageList(parent,name)
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
  QString cmdQuery;
  QString cmdWget = "wget";
  QString redirect = "/tmp/oipkg.query";

  // use file for output
  cmdWget += " --output-document="+redirect;
//http://ipkgfind.handhelds.org/packages.phtml?format=pda&query=ipkg&searchtype=package&section=
	QString server="http://ipkgfind.handhelds.org/"; 	
  cmdQuery = cmdWget+" \""+server+"packages.phtml?";
  cmdQuery += "format=pda&searchtype=package&section=";
  cmdQuery += "&query="+searchString;           	
  cmdQuery += "\"";

  pvDebug(4,"search :"+cmdQuery);
  r = system(cmdQuery.latin1());
  readFileEntries( redirect, "remote" );

//[15:30:38] <killefiz> http://killefiz.de/zaurus/oipkg.php?query=puzzle
  QFile::remove(redirect);
  server="http://killefiz.de/"; 	
  cmdQuery = cmdWget+" \""+server+"zaurus/oipkg.php?";
  cmdQuery += "query="+searchString;           	
  cmdQuery += "\"";

  pvDebug(4,"search :"+cmdQuery);
  r = system(cmdQuery.latin1());
  readFileEntries( redirect, "remote" );
}
