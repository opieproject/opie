#ifndef PACKAGEMANAGERSETTINGS_H
#define PACKAGEMANAGERSETTINGS_H

#include "pksettingsbase.h"
//#include "pmipkg.h"
#include <qintdict.h>
#include <qobject.h>


class PackageManagerSettings : public PackageManagerSettingsBase
{
	 Q_OBJECT
public: 
  PackageManagerSettings( QWidget* ,  const char* , WFlags  );
  ~PackageManagerSettings();

  bool showDialog( int ) ;
  QString getDestinationUrl();
  QString getDestinationName();
  QString getLinkDestinationName();
  bool createLinks();
  QStringList getServers();
  QStringList getActiveServers();
  QStringList getDestinationUrls();
  QStringList getDestinationNames();
  QString getDestinationUrlByName(QString);

public slots:
  void writeInstallationSettings();
  void readInstallationSettings();
  void writeCurrentInstallationSetting();
  void readInstallationSetting(int);
//  void installationSettingSetName(const QString &);
	void removeLinksToDest();
	void createLinksToDest();
  void newServer();
  void editServer(int);
  void removeDestination();
  void newDestination();
  void editDestination(int);
  void linkEnabled(bool);
  void removeServer();
  void serverNameChanged(const QString&);
  void serverUrlChanged(const QString&);
  void destNameChanged(const QString&);
  void destUrlChanged(const QString&);
//  void installationSettingChange(int);
//  void newInstallationSetting();
//  void removeInstallationSetting();
//  void renameInstallationSetting();
  void activeServerChanged();
  void activeDestinationChange(int);
signals:
//  void doCreateLinks( QString dest );
//  void doRemoveLinks( QString dest );

private:
  QIntDict<QString> serverurlDic;
  QIntDict<QString> destinationurlDic;
  int ipkg_old;
  int editedserver;
  int editeddestination;
  int currentSetting;
  int installationSettingsCount;
  bool changed;
  bool serverChanged;

  bool readIpkgConfig(const QString&);
  void writeIpkgConfig(const QString&);
  void writeSettings();
  void readSettings();
};

#endif
