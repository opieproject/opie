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
#include "debug.h"

class PackageListRemote : public PackageList  {
Q_OBJECT
public:
	PackageListRemote(QListView *parent, QString name);
	virtual ~PackageListRemote();
  void query(QString);
public slots:
  void update();
private:
	QString searchString;
};

#endif
