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
