/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include "pksettings.h"

#include <qpe/config.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qcheckbox.h>
#include <qstring.h>
#include <qobject.h>
#include <qtextstream.h>
#include <qtabwidget.h>


PackageManagerSettings::PackageManagerSettings( QWidget* parent,  const char* name, WFlags fl )
  : PackageManagerSettingsBase( parent, name, fl )
{
  connect( newserver, SIGNAL(clicked()), this, SLOT(newServer()) );
  connect( removeserver, SIGNAL(clicked()), this, SLOT(removeServer()) );
  connect( servers, SIGNAL(highlighted(int)), this, SLOT(editServer(int)) );
  connect( newdestination, SIGNAL(clicked()), this, SLOT(newDestination()) );
  connect( removedestination, SIGNAL(clicked()), this, SLOT(removeDestination()) );
  connect( destinations, SIGNAL(highlighted(int)), this, SLOT(editDestination(int)) );
  //   connect( CheckBoxLink, SIGNAL(toggled(bool)),
  //    				 activeLinkDestination, SLOT(setEnabled(bool)) );

  connect( settingName, SIGNAL(activated(int)), this, SLOT(installationSettingChange(int)) );
  connect( settingName, SIGNAL(textChanged(const QString &)), this, SLOT(installationSettingSetName(const QString &)) );
  connect( newsetting, SIGNAL(clicked()), this, SLOT(newInstallationSetting()) );
  connect( renamesetting, SIGNAL(clicked()), this, SLOT(renameInstallationSetting()) );
  connect( removesetting, SIGNAL(clicked()), this, SLOT(removeInstallationSetting()) );
  servername->setEnabled(FALSE);
  serverurl->setEnabled(FALSE);
  serverurlDic.setAutoDelete(TRUE);
  destinationname->setEnabled(FALSE);
  destinationurl->setEnabled(FALSE);
  destinationurlDic.setAutoDelete(TRUE);
  readSettings();
}

PackageManagerSettings::~PackageManagerSettings()
{
}


void PackageManagerSettings::newServer()
{
  int i = servers->count();
  if ( servername->isEnabled() || serverurl->text().isEmpty() ) {
    serverurlDic.insert(i,new QString("http://"));
    servers->insertItem(tr("New"));
    activeServers->insertItem(tr("New"));
  } else {
    // allows one-level undo
    serverurlDic.insert(i,new QString(serverurl->text()));
    servers->insertItem(servername->text());
    activeServers->insertItem(servername->text());
  }
  servers->setSelected(i,TRUE);
  editServer(i);
}

void PackageManagerSettings::newDestination()
{
  int i = destinations->count();
  if ( destinationname->isEnabled() || destinationurl->text().isEmpty() ) {
    destinationurlDic.insert(i,new QString("/"));
    destinations->insertItem(tr("New"));
    activeDestination->insertItem(tr("New"));
    activeLinkDestination->insertItem(tr("New"));
  } else {
    // allows one-level undo
    destinationurlDic.insert(i,new QString(destinationurl->text()));
    destinations->insertItem(destinationname->text());
    activeDestination->insertItem(destinationname->text());
    activeLinkDestination->insertItem(destinationname->text());
  }
  destinations->setSelected(i,TRUE);
  editDestination(i);
}


void PackageManagerSettings::editServer(int i)
{
  if ( servername->isEnabled() ) {
    disconnect( servername, SIGNAL(textChanged(const QString&)), this, SLOT(serverNameChanged(const QString&)) );
    disconnect( serverurl, SIGNAL(textChanged(const QString&)), this, SLOT(serverUrlChanged(const QString&)) );
  } else {
    servername->setEnabled(TRUE);
    serverurl->setEnabled(TRUE);
  }

  servername->setText( servers->text(i) );
  serverurl->setText( *serverurlDic[i] );

  editedserver = i;

  connect( servername, SIGNAL(textChanged(const QString&)), this, SLOT(serverNameChanged(const QString&)) );
  connect( serverurl, SIGNAL(textChanged(const QString&)), this, SLOT(serverUrlChanged(const QString&)) );
}


void PackageManagerSettings::editDestination(int i)
{
  if ( destinationname->isEnabled() ) {
    disconnect( destinationname, SIGNAL(textChanged(const QString&)), this, SLOT(destNameChanged(const QString&)) );
    disconnect( destinationurl, SIGNAL(textChanged(const QString&)), this, SLOT(destUrlChanged(const QString&)) );
  } else {
    destinationname->setEnabled(TRUE);
    destinationurl->setEnabled(TRUE);
  }

  destinationname->setText( destinations->text(i) );
  destinationurl->setText( *destinationurlDic[i] );

  editeddestination = i;

  connect( destinationname, SIGNAL(textChanged(const QString&)), this, SLOT(destNameChanged(const QString&)) );
  connect( destinationurl, SIGNAL(textChanged(const QString&)), this, SLOT(destUrlChanged(const QString&)) );
}

void PackageManagerSettings::removeServer()
{
  disconnect( servername, SIGNAL(textChanged(const QString&)), this, SLOT(serverNameChanged(const QString&)) );
  disconnect( serverurl, SIGNAL(textChanged(const QString&)), this, SLOT(serverUrlChanged(const QString&)) );
  servername->setText(servers->text(editedserver));
  serverurl->setText(*serverurlDic[editedserver]);
  disconnect( servers, SIGNAL(highlighted(int)), this, SLOT(editServer(int)) );
  servers->removeItem(editedserver);
  activeServers->removeItem(editedserver);
  connect( servers, SIGNAL(highlighted(int)), this, SLOT(editServer(int)) );
  servername->setEnabled(FALSE);
  serverurl->setEnabled(FALSE);
}

void PackageManagerSettings::removeDestination()
{
  disconnect( destinationname, SIGNAL(textChanged(const QString&)), this, SLOT(destNameChanged(const QString&)) );
  disconnect( destinationurl, SIGNAL(textChanged(const QString&)), this, SLOT(destUrlChanged(const QString&)) );
  destinationname->setText(destinations->text(editedserver));
  destinationurl->setText(*destinationurlDic[editedserver]);
  disconnect( destinations, SIGNAL(highlighted(int)), this, SLOT(editDestination(int)) );
  destinations->removeItem(editeddestination);
  activeDestination->removeItem(editeddestination);
  activeLinkDestination->removeItem(editeddestination);
  connect( destinations, SIGNAL(highlighted(int)), this, SLOT(editDestination(int)) );
  destinationname->setEnabled(FALSE);
  destinationurl->setEnabled(FALSE);
}

void PackageManagerSettings::serverNameChanged(const QString& t)
{
  disconnect( servers, SIGNAL(highlighted(int)), this, SLOT(editServer(int)) );
  servers->changeItem( t, editedserver );
  activeServers->changeItem( t, editedserver );
  connect( servers, SIGNAL(highlighted(int)), this, SLOT(editServer(int)) );
}

void PackageManagerSettings::destNameChanged(const QString& t)
{
  disconnect( destinations, SIGNAL(highlighted(int)), this, SLOT(editDestination(int)) );
  destinations->changeItem( t, editeddestination );
  activeDestination->changeItem( t, editeddestination );
  activeLinkDestination->changeItem( t, editeddestination );
  connect( destinations, SIGNAL(highlighted(int)), this, SLOT(editDestination(int)) );
}

void PackageManagerSettings::serverUrlChanged(const QString& t)
{
  serverurlDic.replace(editedserver, new QString(t));
}

void PackageManagerSettings::destUrlChanged(const QString& t)
{
  destinationurlDic.replace(editeddestination, new QString(t));
}

void PackageManagerSettings::writeIpkgConfig(const QString& conffile)
{
  QFile conf(conffile);
  if ( ! conf.open(IO_WriteOnly) ) return;
  QTextStream s(&conf);
  s << "# "+tr("Written by Opie Package Manager")+"\n";
  for (int i=0; i<(int)activeServers->count(); i++)
    {
      QString url = serverurlDic[i] ? *serverurlDic[i] : QString("???");
      if ( !activeServers->isSelected(i) )
				s << "#";
      s << "src " << activeServers->text(i) << " " << url << "\n";
    }
  for (int i=0; i<(int)destinations->count(); i++)
    {
      QString url = destinationurlDic[i] ? *destinationurlDic[i] : QString("???");
      s << "dest " << destinations->text(i) << " " << url << "\n";
    }
  conf.close();
}


void PackageManagerSettings::readInstallationSettings()
{
  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "Settings" );
  installationSettingsCount = cfg.readNumEntry( "count", -1 );
  currentSetting  = cfg.readNumEntry( "current", 0 );
	
  for (int i = 0; i < installationSettingsCount; i++)
    {
      cfg.setGroup( "Setting_" + QString::number(i) );
      settingName->insertItem( cfg.readEntry( "name", "???" ), i );
    };
  readInstallationSetting( currentSetting );
}


void PackageManagerSettings::removeInstallationSetting()
{
  settingName->removeItem( settingName->currentItem() );
  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "Setting_" + QString::number( installationSettingsCount ) );
  cfg.clearGroup();
  installationSettingsCount--;
  settingName->setEditable( false );
}

void PackageManagerSettings::newInstallationSetting()
{
  installationSettingsCount++;
  settingName->insertItem( "New", installationSettingsCount );
  settingName->setCurrentItem( installationSettingsCount );
  settingName->setEditable( true );
}

void PackageManagerSettings::installationSettingChange(int cs)
{
  writeCurrentInstallationSetting();
  currentSetting = cs;
  readInstallationSetting( cs );
}

void PackageManagerSettings::writeInstallationSettings()
{
  {
    Config cfg( "oipkg", Config::User );
    cfg.setGroup( "Settings" );
    cfg.writeEntry( "count", installationSettingsCount );
    cfg.writeEntry( "current", currentSetting );
    writeCurrentInstallationSetting();
  }
}


void PackageManagerSettings::readInstallationSetting(int setting)
{
  if ( setting < 0 ) return;
  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "Setting_" + QString::number( setting ) );
  CheckBoxLink->setChecked( cfg.readBoolEntry( "link", false ) );
  QString dest = cfg.readEntry( "dest" );
  QString linkdest = cfg.readEntry( "linkdest" );

  for ( int i = 0; i < activeDestination->count(); i++)
    {
      QString name = cfg.readEntry( "name" );
      if ( activeDestination->text( i ) == name )
	activeDestination->setCurrentItem( i );
      if ( activeLinkDestination->text( i ) == name )
	activeLinkDestination->setCurrentItem( i );
    }
}

void PackageManagerSettings::writeCurrentInstallationSetting()
{
  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "Setting_"  + QString::number(currentSetting) );
  cfg.writeEntry( "link", CheckBoxLink->isChecked() );
  cfg.writeEntry( "dest", getDestinationName() );
  cfg.writeEntry( "linkdest" , getLinkDestinationName() );
  QStringList sers = getActiveServers();
  int srvc = 0;
  for ( QStringList::Iterator it = sers.begin(); it != sers.end(); ++it ) {
    cfg.writeEntry( "server_" + QString::number(srvc++), *it );  	
  }
  cfg.writeEntry( "server_count", srvc );
}

void PackageManagerSettings::renameInstallationSetting()
{
  settingName->setEditable( true );
}

void PackageManagerSettings::installationSettingSetName(const QString &name)
{
  settingName->changeItem( name,  settingName->currentItem() );
}


bool PackageManagerSettings::readIpkgConfig(const QString& conffile)
{
  QFile conf(conffile);
  if ( conf.open(IO_ReadOnly) ) {
    QTextStream s(&conf);
    servers->clear();
    activeServers->clear();
    activeDestination->clear();
    activeLinkDestination->clear();
    serverurlDic.clear();
    destinationurlDic.clear();
    ipkg_old=0;
    int currentserver=0;
    while ( !s.atEnd() ) {
      QString l = s.readLine();
      QStringList token = QStringList::split(' ', l);
      if ( token[0] == "src" || token[0] == "#src" ) {
	currentserver=servers->count();
	serverurlDic.insert(servers->count(),new QString(token[2]));
	int a = token[0] == "src" ? 1 : 0;
	int i = servers->count();
	servers->insertItem(token[1]);
	activeServers->insertItem( token[1] );
	activeServers->setSelected(i,a);
      } else if ( token[0] == "dest" ) {
	currentserver=destinations->count();
	destinationurlDic.insert(destinations->count(),new QString(token[2]));
	destinations->insertItem(token[1]);
	activeDestination->insertItem( token[1] );
	activeLinkDestination->insertItem( token[1] );

      } else if ( token[0] == "option" ) {
      } else {
	// Old style?
	int eq = l.find('=');
	if ( eq >= 0 ) {
	  QString v = l.mid(eq+1).stripWhiteSpace();
	  if ( v[0] == '"' || v[0] == '\'' ) {
	    int cl=v.find(v[0],1);
	    if ( cl >= 0 )
	      v = v.mid(1,cl-1);
	  }
	  if ( l.left(12) == "IPKG_SOURCE=" ) {
	    ipkg_old=1;
	    currentserver=servers->count();
	    serverurlDic.insert(servers->count(),new QString(v));
	    servers->insertItem(v);
	  } else if ( l.left(13) == "#IPKG_SOURCE=" ) {
	    serverurlDic.insert(servers->count(),new QString(v));
	    servers->insertItem(v);
	  } else if ( l.left(10) == "IPKG_ROOT=" ) {

	  }
	}
      }
    }
    if ( ipkg_old ) {
      servers->setSelectionMode(QListBox::Single);
      servers->setSelected(currentserver,TRUE);
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

void PackageManagerSettings::readSettings()
{
  readIpkgConfig("/etc/ipkg.conf");
  readInstallationSettings();
}

void PackageManagerSettings::writeSettings()
{
  writeIpkgConfig("/etc/ipkg.conf");
  writeInstallationSettings();
}

bool PackageManagerSettings::showDialog( int i )
{
  TabWidget->setCurrentPage( i );
  showMaximized();
  bool ret = exec();
  if ( ret ) writeSettings();
  else readSettings();
  return ret;
}

QString PackageManagerSettings::getDestinationName()
{
  return activeDestination->currentText();
}

QString PackageManagerSettings::getLinkDestinationName()
{
  return activeLinkDestination->currentText();
}

/** Returns the URL of the active destination */
QString PackageManagerSettings::getDestinationUrl()
{
  int dnr = activeDestination->currentItem();
  return *destinationurlDic.find(dnr);
}

/** Should oipkg create links form install destination to link destination */
bool PackageManagerSettings::createLinks()
{
  return CheckBoxLink->isChecked();
}

/** get the servers that are active */
QStringList PackageManagerSettings::getActiveServers()
{
  QStringList sl;	
  for (int i=0; i<(int)activeServers->count(); i++)
    {
      if ( activeServers->isSelected(i) )
	sl += activeServers->text(i);
    }
  return sl;
}

/** returns the destination listed in ipkg.conf */
QStringList PackageManagerSettings::getDestinationUrls()
{
  QStringList sl;	
  for (int i=0; i<(int)destinations->count(); i++)
    {   	
      sl += *destinationurlDic[i];
    }
  return sl;
}

void PackageManagerSettings::linkEnabled( bool b )
{
  activeLinkDestination->setEnabled( b );
}

void PackageManagerSettings::activeDestinationChange(int i)
{
  activeLinkDestination->setCurrentItem( i );
}