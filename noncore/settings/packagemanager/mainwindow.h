/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2003 Dan Williams <drw@handhelds.org>
             .=l.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "opackagemanager.h"

#include <qpe/config.h>

#include <qlabel.h>
#include <qlistview.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qprogressbar.h>
#include <qtoolbar.h>
#include <qwidgetstack.h>

class QAction;
class QLineEdit;

class MainWindow :public  QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0x0, const char *name = 0x0, WFlags fl = 0 );
    static QString appName() { return QString::fromLatin1( "packagemanager" ); };

protected:
    void closeEvent( QCloseEvent *event );

private:
    Config          m_config;     // Configuration file

    OPackageManager m_packman;    // Package manager

    // Toolbars
    QToolBar     m_menuBar;       // Main toolbar containing menu
    QToolBar     m_toolBar;       // Main toolbar
    QToolBar     m_findBar;       // Find toolbar

    QWidgetStack m_widgetStack;   // Main widget stack which contains m_packageList & m_statusWidget
    QListView    m_packageList;   // Main list view of all packages

    QLineEdit   *m_findEdit;      // Line edit box used for find toolbar

    // Status widget controls
    QWidget      m_statusWidget;  // Widget to display status during long operations
    QLabel       m_statusText;    // Text status message
    QProgressBar m_statusBar;     // Progress bar showing % completed

    // Icon pixmaps
    QPixmap m_iconUpdated;   // Cached icon which shows when package can be updated
    QPixmap m_iconInstalled; // Cached icon which shows when package is installed
    QPixmap m_iconNull;      // Cached icon which shows when package is not installed

    // Menu/tool bar actions
    QAction *m_actionShowNotInstalled; // Action to show pakages not currently installed
    QAction *m_actionShowInstalled;    // Action to show pakages currently installed
    QAction *m_actionShowUpdated;      // Action to show pakages currently installed with update available
    QAction *m_actionFilter;           // Action to filter packages
    QAction *m_actionFindNext;         // Action to find next match

    // Cached filter settings
    QString                 m_filterName;     // Cached name filter value
    QString                 m_filterServer;   // Cached server name filter value
    QString                 m_filterDest;     // Cached destination name filter value
    OPackageManager::Status m_filterStatus;   // Cached status filter value
    QString                 m_filterCategory; // Cached category filter value

    void initPackageList();
    void initStatusWidget();
    void initUI();

    void loadPackageList( OPackageList *packages = 0x0, bool clearList = true );
    void searchForPackage( const QString &text );

private slots:
    void initPackageInfo();
    void slotWidgetStackShow( QWidget *widget );

    // Status widget slots
    void slotInitStatusBar( int numSteps );
    void slotStatusText( const QString &status );
    void slotStatusBar( int currStep );

    // Actions menu action slots
    void slotUpdate();
    void slotUpgrade();
    void slotDownload();
    void slotApply();
    void slotCloseDlg();
    void slotConfigure();

    // View menu action slots
    void slotShowNotInstalled();
    void slotShowInstalled();
    void slotShowUpdated();
    void slotFilterChange();
    void slotFilter( bool isOn );

    // Find action slots
    void slotFindShowToolbar();
    void slotFindHideToolbar();
    void slotFindChanged( const QString &findText );
    void slotFindNext();

    // Other slots
    void slotDisplayPackageInfo( QListViewItem * );
};

#endif
