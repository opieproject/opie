#ifndef PMIPKG_H
#define PMIPKG_H


#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include "pksettings.h"
#include "runwindow.h"
#include "packagelist.h"
#include "debug.h"



class PmIpkg : public QObject
{
  Q_OBJECT
public:
  PmIpkg( PackageManagerSettings* , QWidget* p=0,  const char * name=0, WFlags f=0 );
  ~PmIpkg();

  void commit( PackageList );
  void update();
  PackageList* getPackageList();
  void showButtons(bool b=true);
  void show( bool buttons=true );

private:
  PackageManagerSettings* settings;
  RunWindow *runwindow;
  QStringList to_remove;
  QStringList to_install;
  bool runwindowopen;

  void makeLinks(QString);
  void processLinkDir( QString, QString );

  int runIpkg(const QString& args);
  void out( QString );

public slots:
	void doIt();
 	void install();
  void remove();
  void linkDestination( const QString, const QByteArray );
};

#endif
