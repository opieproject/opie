/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PACKAGELISTLOCAL_H
#define PACKAGELISTLOCAL_H

#include "packagelist.h"


#define HACK
#ifdef HACK
  static QString listsDir="/usr/lib/ipkg/";
  static QString statusDir="/usr/lib/ipkg/";
#endif

class PackageListLocal : public PackageList  {
Q_OBJECT
public:
	PackageListLocal(QListView *parent, QString name);
	virtual ~PackageListLocal();

public slots:
  void update();
private:
#ifndef HACK
  QString listsDir;
  QString statusDir;
#endif
  void parseStatus();
  void parseList();
};

#endif
