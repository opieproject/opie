/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PACKAGELISTFEEDS_H
#define PACKAGELISTFEEDS_H

#include "ochecklistitem.h"
#include "pksettings.h"
#include "packagelistview.h"



class PackageListFeeds : public OCheckListItem{
public:
	PackageListFeeds( PackageListView* parent=0, const char* name=0, PackageManagerSettings *s=0 );
	virtual ~PackageListFeeds();
private:
  PackageManagerSettings *settings;   
  QString listsDir;
  QString statusDir;
};

#endif
