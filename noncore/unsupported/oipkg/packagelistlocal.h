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



class PackageListLocal : public PackageList  {
public:
	PackageListLocal( PackageListView* parent, const char* name, QString file, PackageManagerSettings *s );
	virtual ~PackageListLocal();
  virtual void expand();

//public slots:
  void update();
private:
  QString packageFile;
  bool readFileEntry;
//  QString listsDir;
//  QString statusDir;
//  void parseStatus();
//  void parseList();
};

#endif
