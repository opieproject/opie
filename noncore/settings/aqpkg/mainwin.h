/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

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
class QListViewItem;
class QToolBar;
class QProgressBar;
class QWidgetStack;

class MainWindow :public  QMainWindow
{
	Q_OBJECT
public:

	MainWindow();
	~MainWindow();

protected:
    void closeEvent( QCloseEvent* e );

private:
    DataManager *mgr;

    QWidgetStack *stack;
    
    QToolBar *findBar;
    QToolBar *jumpBar;
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
	bool reloadDocuments;

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
    void setDocument( const QString &doc );
    void displayFindBar();
    void displayJumpBar();
    void repeatFind();
    void findPackage( const QString & );
    void hideFindBar();
    void hideJumpBar();
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
    void slotDisplayPackage( QListViewItem * );
};
#endif
