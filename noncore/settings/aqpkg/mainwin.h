/***************************************************************************
                          mainwin.h  -  description
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

#ifndef MAINWIN_H
#define MAINWIN_H

#include <qmainwindow.h>
#include <qpixmap.h>

class DataManager;
class InstallData;
class InstallDlgImpl;

class QAction;
class QCheckListItem;
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QPEToolBar;
class QProgressBar;
class QWidgetStack;

class MainWindow :public  QMainWindow
{
	Q_OBJECT
public:

	MainWindow();
	~MainWindow();

private:
    DataManager *mgr;

    QWidgetStack *stack;
    
    QPEToolBar *findBar;
    QPEToolBar *jumpBar;
    QLineEdit  *findEdit;
    QAction    *actionFindNext;
    QAction    *actionFilter;
    QAction    *actionUpgrade;
    QAction    *actionDownload;
    QAction    *actionUninstalled;
    QAction    *actionInstalled;
    QAction    *actionUpdated;
    
    QPixmap iconDownload;
    QPixmap iconRemove;
    
    int mnuShowUninstalledPkgsId;
    int mnuShowInstalledPkgsId;
    int mnuShowUpgradedPkgsId;
    int mnuFilterByCategory;
    int mnuSetFilterCategory;

    // Main package list widget
    QWidget   *networkPkgWindow;
    QComboBox *serversList;
    QListView *packagesList;
    QPixmap    installedIcon;
    QPixmap    updatedIcon;
    QString    currentlySelectedServer;
    QString    categoryFilter;
    QString    stickyOption;

    bool categoryFilterEnabled;
    bool showJumpTo;
    bool showUninstalledPkgs;
    bool showInstalledPkgs;
    bool showUpgradedPkgs;
    bool downloadEnabled;

    void initMainWidget();
    void updateData();
    void serverSelected( int index, bool showProgress );
    void searchForPackage( const QString & );
    bool filterByCategory( bool val );
    void downloadSelectedPackages();
    void downloadRemotePackage();
    InstallData *dealWithItem( QCheckListItem *item );
    
    // Progress widget
    QWidget      *progressWindow;
    QLabel       *m_status;
    QProgressBar *m_progress;
    
    void initProgressWidget();
    
public slots:
//    void setDocument( const QString &doc );
    void displayHelp();
    void displayFindBar();
    void displayJumpBar();
    void repeatFind();
    void findPackage( const QString & );
    void hideFindBar();
    void hideJumpBar();
    void displayAbout();
    void displaySettings();
    void filterUninstalledPackages();
    void filterInstalledPackages();
    void filterUpgradedPackages();
    void filterCategory();
    bool setFilterCategory();
    void raiseMainWidget();
    void raiseProgressWidget();
    void enableUpgrade( bool );
    void enableDownload( bool );
    void reloadData( InstallDlgImpl * );

private slots:
    void init();
    void setProgressSteps( int );
    void setProgressMessage( const QString & );
    void updateProgress( int );
    void serverSelected( int index );
    void updateServer();
    void upgradePackages();
    void downloadPackage();
    void applyChanges();
    void letterPushed( QString t );
};
#endif
