/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PACKAGEMANAGERSETTINGS_H
#define PACKAGEMANAGERSETTINGS_H

#include "pksettingsbase.h"
#include <qintdict.h>


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
  QStringList getActiveServers();
  QStringList getDestinationUrls();

public slots:                                                                                                                                                                                             /** No descriptions */
  void createLinks();
  void removeLinks();
	void writeInstallationSettings();
  void readInstallationSettings();
  void writeCurrentInstallationSetting();
  void readInstallationSetting(int);
  void installationSettingSetName(const QString &);
  void activeDestinationChange(int)
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
  void installationSettingChange(int);
  void newInstallationSetting();
  void removeInstallationSetting();
  void renameInstallationSetting();

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

