#ifndef PACKAGELISTDOCLNK_H
#define PACKAGELISTDOCLNK_H

#include "packagelist.h"
#include "debug.h"

class DocLnkSet;

class PackageListDocLnk : public PackageList
{
public:
	PackageListDocLnk();
	PackageListDocLnk( PackageManagerSettings* s);
	virtual ~PackageListDocLnk();
public slots:
  void update();
private:
	DocLnkSet *doclnkset;
};


#endif
