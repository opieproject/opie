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

class QWidgetStack;
class QPEToolBar;
class QLineEdit;
class QAction;
class ProgressWidget;
class NetworkPackageManager;
class DataManager;

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
    
    NetworkPackageManager *networkPkgWindow;
    ProgressWidget *progressWindow;

    int mnuShowUninstalledPkgsId;
    int mnuShowInstalledPkgsId;
    int mnuShowUpgradedPkgsId;
    int mnuFilterByCategory;
    int mnuSetFilterCategory;

public slots:
    void setDocument( const QString &doc );
    void displayHelp();
    void displayFindBar();
    void repeatFind();
    void findPackage( const QString & );
    void hideFindBar();
    void displayAbout();
    void displaySettings();
    void filterUninstalledPackages();
    void filterInstalledPackages();
    void filterUpgradedPackages();
    void filterCategory();
    void setFilterCategory();
    void raiseMainWidget();
    void raiseProgressWidget();
    void enableUpgrade( bool );
    void enableDownload( bool );

private slots:
    void init();
};
#endif
