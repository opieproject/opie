/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PACKAGELISTREMOTE_H
#define PACKAGELISTREMOTE_H

#include "packagelist.h"
#include "package.h"
#include "pksettings.h"

class PackageListRemote : public PackageList  {
public:
//	PackageListRemote( PackageListView*, const char *name);
	PackageListRemote( PackageListView*, const char *name, PackageManagerSettings* s=0);
	virtual ~PackageListRemote();
  void query(QString);
  virtual void expand() {};
//public slots:
  void update();
private:
	QString searchString;
};

#endif
