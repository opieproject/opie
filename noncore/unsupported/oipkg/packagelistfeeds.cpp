/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
//#include <qpe/config.h>
//#include <qfile.h>
//#include <qfileinfo.h>
//#include <qtextstream.h>
//#include <qstringlist.h>
#include "packagelistfeeds.h"
#include "packagelistlocal.h"
#include "ochecklistitem.h"
#include "pksettings.h"
#include <qpe/config.h>
#include <qfileinfo.h>


PackageListFeeds::PackageListFeeds( PackageListView *parent, const char *name, PackageManagerSettings* s)
	: OCheckListItem((QListView*)parent,name)
{
  settings = s;
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
	OCheckListItem::expand(); // get rid of dummy
  QStringList srvs = settings->getActiveServers();
  PackageListLocal *servers;
  for ( QStringList::Iterator it = srvs.begin(); it != srvs.end(); ++it )
    {      
  		servers = new PackageListLocal( parent, *it, listsDir+"/"+*it, settings );
    	insertItem( servers );
    }
  servers = new PackageListLocal( parent, QObject::tr("status"), statusDir+"/status", settings  );
  servers->update();
  parent->insertItem( servers );
}

PackageListFeeds::~PackageListFeeds()
{
	Config cfg( "oipkg", Config::User );
	cfg.setGroup( "Common" );
	cfg.writeEntry( "statusDir", statusDir );
	cfg.writeEntry( "listsDir", listsDir );
}

//void PackageListFeeds::parseStatus()
//{
//  QStringList dests = settings->getDestinationUrls();
//  QStringList destnames = settings->getDestinationNames();
//  QStringList::Iterator name = destnames.begin();
//  for ( QStringList::Iterator dir = dests.begin(); dir != dests.end(); ++dir )
//    {
//      qDebug("Status: "+*dir+statusDir+"/status");
//      readFileEntries( *dir+statusDir+"/status", *name );
//      ++name;
//    };
//}
//
//void PackageListFeeds::parseList()
//{
//  QStringList srvs = settings->getActiveServers();
//
//  for ( QStringList::Iterator it = srvs.begin(); it != srvs.end(); ++it )
//    {
//      qDebug("List: "+listsDir+"/"+*it);
//      readFileEntries( listsDir+"/"+*it );
//    }
//}


//void PackageListFeeds::expand()
//{
//
//}
