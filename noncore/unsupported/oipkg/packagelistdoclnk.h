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
#include "debug.h"

class DocLnkSet;

class PackageListDocLnk : public PackageList
{
public:
	PackageListDocLnk(QObject *parent=0, const char *name=0);
	PackageListDocLnk( PackageManagerSettings *s, QObject *parent=0, const char *name=0);
	virtual ~PackageListDocLnk();
public slots:
  void update();
private:
	DocLnkSet *doclnkset;
 	QString docLnkDir;
};


#endif
