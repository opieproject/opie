#ifndef PACKAGELIST_H
#define PACKAGELIST_H

#include <qdict.h>
#include "package.h"
#include "pksettings.h"
#include "debug.h"

#define HACK
#ifdef HACK
  static QString listsDir="/usr/lib/ipkg/";
  static QString statusDir="/usr/lib/ipkg/";
#endif

class PackageList {
public:
//	static QString all = QObject::tr("All");

  PackageList();
  PackageList( PackageManagerSettings* );
  ~PackageList();
  void insertPackage( Package* );
  Package* find( QString );
  Package* next();
  Package* first();

  QStringList getSections();
  QStringList getSubSections();
  void setSettings( PackageManagerSettings* );
  void filterPackages( QString );
  Package* getByName( QString );
  /** No descriptions */
  void clear();

public slots:
  void setSection(QString);
  void setSubSection(QString);
  void update();

private:
  int currentPackage;
  int packageCount;

  PackageManagerSettings     *settings;
  QDict<Package>         packageList;
  QDict<Package>         origPackageList;
  QDictIterator<Package> packageIter;

  bool empty;
#ifndef HACK
  QString listsDir;
  QString statusDir;
#endif
  QString            aktSection;
  QString            aktSubSection;
  QStringList        sections;
  QDict<QStringList> subSections;
  QDict<bool> sectionsDict;


  void updateSections( Package* );
  void parseStatus();
  void parseList();
  void readFileEntries( QString );
};


#endif
