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

#include <qdict.h>
#include "package.h"
#include "pksettings.h"
#include "debug.h"

class PackageList : public QObject
{
  Q_OBJECT
public:
//	static QString all = QObject::tr("All");

  PackageList (QObject *parent=0, const char *name=0);
  PackageList( PackageManagerSettings *s, QObject *parent=0, const char *name=0);
 	virtual ~PackageList();
  void insertPackage( OipkgPackage* );
  OipkgPackage* find( QString );
  OipkgPackage* next();
  OipkgPackage* first();

  QStringList getSections();
  QStringList getSubSections();
  void setSettings( PackageManagerSettings* );
  void filterPackages( QString );
  OipkgPackage* getByName( QString );
  void clear();
  void allPackages();

  QDict<OipkgPackage> *versions;

public slots:
  void setSection(QString);
  void setSubSection(QString);
 // virtual void update();

protected:
  int currentPackage;
  int packageCount;

  PackageManagerSettings     *settings;
  QDict<OipkgPackage>         packageList;
  QDict<OipkgPackage>         origPackageList;
  QDictIterator<OipkgPackage> packageIter;

  bool empty;
  QString            aktSection;
  QString            aktSubSection;
  QStringList        sections;
  QDict<QStringList> subSections;
  QDict<bool> sectionsDict;


  void updateSections( OipkgPackage* );
  void readFileEntries( QString file, QString dest="" );
};


#endif
