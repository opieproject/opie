/***************************************************************************
                          networkpkgmgr.h  -  description
                             -------------------
    begin                : Mon Aug 26 13:32:30 BST 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NETWORKPKGMGR_H
#define NETWORKPKGMGR_H

#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qcombobox.h>
#include <qlistview.h>

#include "datamgr.h"

class InstallData;

/** NetworkPackageManager is the base class of the project */
class NetworkPackageManager : public QWidget
{
  Q_OBJECT
public:
    /** construtor */
    NetworkPackageManager( QWidget* parent=0, const char *name=0 );
    /** destructor */
    ~NetworkPackageManager();

    void setDataManager( DataManager *dm );
    void selectLocalPackage( const QString &pkg );
    void updateData();
    void searchForPackage( bool findNext );
    void showOnlyUninstalledPackages( bool val );
    void showOnlyInstalledPackages( bool val );
    void showUpgradedPackages( bool val );
    bool filterByCategory( bool val );
    bool setFilterCategory();
    
private:
    DataManager *dataMgr;

    QComboBox *serversList;
    QListView *packagesList;
    QPushButton *update;
    QPushButton *upgrade;
    QPushButton *download;
    QPushButton *apply;

    QString currentlySelectedServer;
    QString lastSearchText;
    QString categoryFilter;

    bool categoryFilterEnabled;
    bool showJumpTo;
    bool showUninstalledPkgs;
    bool showInstalledPkgs;
    bool showUpgradedPkgs;

    void initGui();
    void setupConnections();
    void showProgressDialog( char *initialText );
    void downloadSelectedPackages();
    void downloadRemotePackage();
    void serverSelected( int index, bool showProgress );
    
    InstallData dealWithItem( QCheckListItem *item );
    QString stickyOption;

signals:
    void appRaiseMainWidget();
    void appRaiseProgressWidget();
    void progressSetSteps( int );
    void progressSetMessage( const QString & );
    void progressUpdate( int );

public slots:
    void applyChanges();
    void upgradePackages();
    void downloadPackage();
    void updateServer();
    void displayText( const QString &t );
    void letterPushed( QString t );
    void serverSelected( int index );
};

#endif
