/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "swordoptionlist.h"

#include <opie2/otabwidget.h>

#include <qpe/config.h>

#include <qmainwindow.h>
#include <qtimer.h>

#include <swmgr.h>

class QAction;
class QMenuBar;
class QPixmap;
class QToolBar;

class NavBar;
class SearchBar;

class MainWindow : public  QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0x0, const char *name = 0x0, WFlags fl = 0 );
    ~MainWindow();

    static QString appName() { return QString::fromLatin1( "dagger" ); };

protected:
    bool eventFilter( QObject *obj, QEvent *event );

private:
    sword::SWMgr *m_swordMgr;      // Sword library module manager
    Config        m_config;        // Application configuration

    // Configuration items
    QString       m_modulePath;         // Directory where sword modules are located
    bool          m_alwaysOpenNew;      // Whether or not open modules as new window/tab (or re-use existing)
    int           m_numVerses;          // Number of verses to display at a time for Bible modules
    bool          m_disableScreenBlank; // Whether or not to disable automatic screen blanking
    int           m_copyFormat;         // Format used when copying
    QFont         m_textFont;           // Font used for module text

    // UI components
    QToolBar   *m_barDock;         // Main toolbar which contains menu and all other toolbars
    QMenuBar   *m_menuBar;         // Application menu bar
    QPopupMenu *m_bookmarkMenu;    // Pointer to bookmark menu
    NavBar     *m_navToolbar;      // Text navigation toolbar
    SearchBar  *m_searchToolbar;   // Text search toolbar
    Opie::Ui::OTabWidget m_tabs;   // Main widget in which all texts, notes, etc. will be displayed

    // Other visual items
    QPixmap *m_bibleIcon;          // Icon used for bible modules
    QPixmap *m_commentaryIcon;     // Icon used for commentary modules
    QPixmap *m_lexiconIcon;        // Icon used for lexicon modules

    QTimer   m_autoScrollTimer;    // Timer for auto-scrolling of bible texts

    // Menubar/toolbar actions
    SwordOptionList m_actionSwordOpts;         // List of actions for sword options
                                               // (e.g. footnotes, Strong's numbers, etc.)
    QAction        *m_actionTextClose;         // Action for closing currently opened module
    QAction        *m_actionEditCopy;          // Action for copying text from current module
    QAction        *m_actionBookmarkAdd;       // Action for adding a bookmark
    QAction        *m_actionBookmarkRemove;    // Action for removing a bookmark
    QAction        *m_actionViewNavToolbar;    // Action for displaying/hiding the navigation toolbar
    QAction        *m_actionViewSearchToolbar; // Action for displaying/hiding the search toolbar

    void initUI();
    void openModule( const QString &modulename, const QString &key = 0x0 );
    int  findBookmark( const QString &bookmark );
    void enableScreenBlanking( bool enable );

private slots:
    void initConfig();

    void slotTextDisplayed( QWidget *textWidget );

    // Menubar/toolbar action slots
    void slotTextOpen();
    void slotTextClose();
    void slotTextInstall();
    void slotEditCopy();
    void slotEditConfigure();
    void slotBookmarkAdd();
    void slotBookmarkRemove();
    void slotBookmarkSelected();
    void slotViewSwordOption( bool enabled );
    void slotViewNavToolbar( bool enabled );
    void slotViewSearchToolbar( bool enabled );

    // Navigation toolbar slots
    void slotNavPrevPage();
    void slotNavPrevVerse();
    void slotNavKeyChanged( const QString &newKey );
    void slotNavNextVerse();
    void slotNavNextPage();
    void slotNavAutoScroll( bool enabled );
    void slotNavScrollRateChanged( int newRate );

    // Search toolbar slots
    void slotSearchResultClicked( const QString &key );

    // Text widget slots
    void slotTextRefClicked( const QString &ref );

signals:
    void sigNumVersesChanged( int numVerses );
    void sigFontChanged( const QFont *newFont );
    void sigOptionChanged();
};

#endif
