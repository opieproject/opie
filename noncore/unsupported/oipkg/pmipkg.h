#ifndef PMIPKG_H
#define PMIPKG_H


#include <qobject.h>
#include <qlist.h>
#include <qstring.h>
#include <qwidget.h>
#include "pksettings.h"
#include "runwindow.h"
#include "packagelist.h"
#include "installdialog.h"
#include "debug.h"

#define createLink 0
#define removeLink 1

class Package;
class PmIpkg : public QObject
{
  Q_OBJECT
public:
  PmIpkg( PackageManagerSettings* , QWidget* p=0,  const char * name=0, WFlags f=0 );
  ~PmIpkg();

  int linkOpp;
  void loadList( PackageList* );
  void commit();
  void update();
  void showButtons(bool b=true);
  void show( bool buttons=true );

public slots:
	void doIt();
 	void install();
  void remove();
  void installFile(const QString &fileName, const QString &dest="");
  void removeFile(const QString &fileName, const QString &dest="");
  void createLinks( const QString &dest );
  void removeLinks( const QString &dest );

private:
  PackageManagerSettings* settings;
  RunWindow *runwindow;
  InstallDialog *installDialog;
  QList<Package> to_remove;
  QList<Package> to_install;
  QString fileNameToInstall;
  QCheckBox *_force_reinstall;
  QCheckBox *_force_remove;
  QCheckBox *_force_depends;
  void startDialog();
  void makeLinks(Package*);
  void linkPackage( QString, QString );
  void processLinkDir( QString , QString );
  int  runIpkg(const QString& args, const QString& dest="" );
  void out( QString );
	QStringList* getList( QString, QString );
	void processFileList( QStringList*, QString );
 	

};

#endif
