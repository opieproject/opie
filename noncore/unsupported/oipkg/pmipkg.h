#ifndef PMIPKG_H
#define PMIPKG_H


#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include "pksettings.h"
#include "runwindow.h"
#include "packagelist.h"
#include "debug.h"

//#define DIA

#include <qpe/qcopenvelope_qws.h>


//class PmIpkg : public RunWindow
class PmIpkg : public QObject
{
  Q_OBJECT
public:
  PmIpkg( PackageManagerSettings* , QWidget* p=0,  const char * name=0, WFlags f=0 );
  //	PmIpkg( QObject*, PackageManagerSettings* );
  ~PmIpkg();

  void commit( PackageList );
  void update();
  PackageList* getPackageList();

private:
  PackageManagerSettings* settings;
  RunWindow *runwindow;
  QCopChannel *linkDest;

  void makeLinks(QString);
  void processLinkDir( QString, QString );

  int runIpkg(const QString& args);
  void out( QString );

public slots:
  void linkDestination( const QString, const QByteArray );
};

#endif
