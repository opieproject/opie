/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include <qpe/config.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include "packagelistlocal.h"

PackageListLocal::PackageListLocal()
	: PackageList()
{
	Config cfg( "oipkg", Config::User );
	cfg.setGroup( "Common" );
	statusDir = cfg.readEntry( "statusDir", "" );
	listsDir  = cfg.readEntry( "listsDir", "" );
  if ( statusDir=="" || ! QFileInfo(statusDir+"/status").isFile() )
  {
		statusDir="/usr/lib/ipkg/";
		listsDir="/usr/lib/ipkg/lists/";
		cfg.writeEntry( "statusDir", statusDir );
		cfg.writeEntry( "listsDir", listsDir );
  }
}

PackageListLocal::PackageListLocal(PackageManagerSettings* s)
	: PackageList(s)
{
	PackageListLocal();
}

PackageListLocal::~PackageListLocal()
{
}

void PackageListLocal::parseStatus()
{
  QStringList dests = settings->getDestinationUrls();
  QStringList destnames = settings->getDestinationNames();
  QStringList::Iterator name = destnames.begin();
  for ( QStringList::Iterator dir = dests.begin(); dir != dests.end(); ++dir )
    {
      pvDebug( 2,"Status: "+*dir+statusDir+"/status");
      readFileEntries( *dir+statusDir+"/status", *name );
      ++name;
    };
}

void PackageListLocal::parseList()
{
  QStringList srvs = settings->getActiveServers();
	
  for ( QStringList::Iterator it = srvs.begin(); it != srvs.end(); ++it )
    {
      pvDebug( 2, "List: "+listsDir+"/"+*it);
      readFileEntries( listsDir+"/"+*it );  	
    }
}


void PackageListLocal::update()
{
  pvDebug( 2, "parseStatus");
  parseStatus();
  pvDebug( 2, "parseList "+listsDir);
  parseList();
  pvDebug( 2, "finished parsing");
}
