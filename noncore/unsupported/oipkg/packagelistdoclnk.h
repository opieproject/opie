/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PACKAGELISTDOCLNK_H
#define PACKAGELISTDOCLNK_H

#include "packagelist.h"

class DocLnkSet;

class PackageListDocLnk : public PackageList
{
public:
//	PackageListDocLnk(PackageListView *parent=0, const char *name=0);
	PackageListDocLnk( PackageListView *parent=0, const char *name=0, PackageManagerSettings *s=0 );
	virtual ~PackageListDocLnk();
  virtual void expand();
//public slots:
  void update();
private:
	DocLnkSet *doclnkset;
 	QString docLnkDir;
  void init();
};


#endif
