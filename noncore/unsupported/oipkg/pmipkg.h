#ifndef PMIPKG_H
#define PMIPKG_H


#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include "pksettings.h"
#include "runwindow.h"
#include "packagelist.h"
#include "debug.h"


#include <qpe/qcopenvelope_qws.h>

class PmIpkg : public RunWindow
//class PmIpkg : public QObject
{
  Q_OBJECT
public:
  PmIpkg( PackageManagerSettings* , QWidget* p=0,  const char * name=0, WFlags f=0 );
  //	PmIpkg( QObject*, PackageManagerSettings* );
  ~PmIpkg();

  PackageList* getPackageList();

private:
  PackageManagerSettings* settings;
  QCopChannel *linkDest;
  void processLinkDir( QString, QString );


public:
  void makeLinks(QString);
  int runIpkg(const QString& args);
  void commit( PackageList );

public slots:
  void linkDestination( QString, QString );
};

#endif
