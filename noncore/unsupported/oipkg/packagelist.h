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

  QDict<Package> *versions;

public slots:
  void setSection(QString);
  void setSubSection(QString);
 // virtual void update();

protected:
  int currentPackage;
  int packageCount;

  PackageManagerSettings     *settings;
  QDict<Package>         packageList;
  QDict<Package>         origPackageList;
  QDictIterator<Package> packageIter;

  bool empty;
  QString            aktSection;
  QString            aktSubSection;
  QStringList        sections;
  QDict<QStringList> subSections;
  QDict<bool> sectionsDict;


  void updateSections( Package* );
  void readFileEntries( QString file, QString dest="" );
};


#endif
