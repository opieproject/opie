#ifndef PACKAGELISTREMOTE_H
#define PACKAGELISTREMOTE_H

#include "packagelist.h"
#include "package.h"
#include "pksettings.h"
#include "debug.h"

class PackageListRemote : public PackageList  {
public:
	PackageListRemote();
	PackageListRemote( PackageManagerSettings* s);
	virtual ~PackageListRemote();
  void query(QString);
public slots:
  void update();
private:
	QString searchString;
};

#endif
