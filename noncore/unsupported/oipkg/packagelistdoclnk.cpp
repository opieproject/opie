/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include "packagelistdoclnk.h"

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qlist.h>

#include "package.h"
#include "pksettings.h"

PackageListDocLnk::PackageListDocLnk(PackageListView *parent, const char *name,PackageManagerSettings* s)
	: PackageList(parent,name,s)
{
	init();
}

//PackageListDocLnk::PackageListDocLnk( PackageListView *parent, const char *name)
//	: PackageList(parent, name)
//{
//	init();
//}

void PackageListDocLnk::init()
{
	Config cfg( "oipkg", Config::User );
	cfg.setGroup( "Common" );
	docLnkDir = cfg.readEntry( "docLnkDir", "/root/" );
 	qDebug("opening DocLnkSet "+docLnkDir);
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
	qDebug("PackageListDocLnk::update ");	
	QList<DocLnk> packlist = doclnkset->children();
  for (DocLnk *pack =packlist.first(); pack != 0; pack=packlist.next() )
  {
    insertPackage( new Package(pack->file(), settings ) );
  }
}

void PackageListDocLnk::expand()
{
  update();
}

  