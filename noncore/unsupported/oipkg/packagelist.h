/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PACKAGELIST_H
#define PACKAGELIST_H

#include <qlistview.h>
#include <qdict.h>
#include "package.h"
#include "pksettings.h"

class PackageListView;

class PackageList : public QCheckListItem
{
public:
  PackageList( PackageListView *parent=0, const char *name=0, PackageManagerSettings *s=0 );
 	virtual ~PackageList();
  void insertPackage( Package* );
  Package* find( QString );
  Package* next();
  Package* first();

  QStringList getSections();
  QStringList getSubSections();
  void setSettings( PackageManagerSettings* );
  void filterPackages( QString );
  Package* getByName( QString );
  void clear();
  void allPackages();
  virtual void expand();

  QDict<Package> *versions;

  void setSection(QString);
  void setSubSection(QString);
  virtual void update(){};

protected:
  int currentPackage;
  int packageCount;

  PackageManagerSettings     *settings;
  QDict<Package>         packageList;
  QDict<Package>         origPackageList;
  QDictIterator<Package> packageIter;

  QString            aktSection;
  QString            aktSubSection;
  QStringList        sections;
  QDict<QStringList> subSections;
  QDict<bool> sectionsDict;


  void updateSections( Package* );
  void readFileEntries( QString file, QString dest="" );
private:
	QCheckListItem *dummy;
};


#endif
