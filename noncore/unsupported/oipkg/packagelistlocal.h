#ifndef PACKAGELISTLOCAL_H
#define PACKAGELISTLOCAL_H

#include "packagelist.h"


#define HACK
#ifdef HACK
  static QString listsDir="/usr/lib/ipkg/";
  static QString statusDir="/usr/lib/ipkg/";
#endif

class PackageListLocal : public PackageList  {
public:
	PackageListLocal();
	PackageListLocal( PackageManagerSettings* );
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
