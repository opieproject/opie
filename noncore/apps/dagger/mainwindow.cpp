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

#include "mainwindow.h"
#include "navbar.h"
#include "searchbar.h"
#include "opentextdlg.h"
#include "configuredlg.h"
#include "textwidget.h"

#include <opie2/odebug.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qclipboard.h>
#include <qmenubar.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qtoolbar.h>

#include <markupfiltmgr.h>

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags /*fl*/ )
    : QMainWindow( parent, name, WStyle_ContextHelp )
    , m_config( "dagger" )
    , m_tabs( this )
    , m_autoScrollTimer( this )
{
    // Initialize sword library manager
    m_config.setGroup( "Sword" );
    m_modulePath = m_config.readEntry( "ModPath", "/usr/local/share/sword" );
    m_swordMgr = new sword::SWMgr( m_modulePath.latin1(), true,
                                   new sword::MarkupFilterMgr( sword::FMT_HTMLHREF ) );

    // Retrieve list of available Sword module options (e.g. footnotes, Strong's numbers, etc.)
    sword::OptionsList swordOpts = m_swordMgr->getGlobalOptions();
    for ( sword::OptionsList::iterator it = swordOpts.begin(); it != swordOpts.end(); it++ )
        m_actionSwordOpts.append( new QAction( (*it).c_str(), QString::null, 0, this, 0 ) );
    m_actionSwordOpts.sort();

    // Initialize user interface
    setCaption( tr( "Dagger" ) );
    initUI();

    connect( &m_tabs, SIGNAL(currentChanged(QWidget *)), this, SLOT( slotTextDisplayed(QWidget *)) );
    connect( &m_autoScrollTimer, SIGNAL(timeout()), this, SLOT(slotNavNextVerse()) );

    m_bibleIcon = new QPixmap( Resource::loadPixmap( "dagger/bibletext" ) );
    m_commentaryIcon = new QPixmap( Resource::loadPixmap( "dagger/commentary" ) );
    m_lexiconIcon = new QPixmap( Resource::loadPixmap( "dagger/lexicon" ) );

    // Load initial configuration
    QTimer::singleShot( 100, this, SLOT( initConfig() ) );
}

MainWindow::~MainWindow()
{
    // Save Sword options
    m_config.setGroup( "Sword" );
    m_config.writeEntry( "ModPath", m_modulePath );

    for ( QAction *a = m_actionSwordOpts.first(); a; a = m_actionSwordOpts.next() )
        m_config.writeEntry( a->text(), a->isOn() );

    // Save configuration options
    m_config.setGroup( "Config" );
    m_config.writeEntry( "AlwaysOpenNew", m_alwaysOpenNew );
    m_config.writeEntry( "AutoScroll", m_navToolbar->autoScrollRate() );
    m_config.writeEntry( "DisableScreenBlanking", m_disableScreenBlank );
    m_config.writeEntry( "CopyFormat", m_copyFormat );
    m_config.writeEntry( "NavBar", m_actionViewNavToolbar->isOn() );
    m_config.writeEntry( "NumVerses", m_numVerses );
    m_config.writeEntry( "SearchBar", m_actionViewSearchToolbar->isOn() );

    // Save text font
    m_config.setGroup( "Font");
    m_config.writeEntry( "Family", m_textFont.family() );
    m_config.writeEntry( "Italic", m_textFont.italic() );
    m_config.writeEntry( "Size", m_textFont.pointSize() );
    m_config.writeEntry( "Weight", m_textFont.weight() );

    // Save bookmarks
    m_config.setGroup( "Bookmarks");
    m_config.clearGroup();
    int index = 3;
    int id = m_bookmarkMenu->idAt( index );
    while ( id != -1 )
    {
        QString bookmark = m_bookmarkMenu->text( id );
        int pos = bookmark.find( " (" );
        QString key = bookmark.left( pos );
        pos += 2;
        QString module = bookmark.mid( pos, bookmark.find( ")", pos ) - pos );
        QString modkey;
        modkey.sprintf( "%s/%s", module.latin1(), key.latin1() );
        m_config.writeEntry( QString::number( index - 2 ), modkey );

        ++index;
        id = m_bookmarkMenu->idAt( index );
    }

    // Save opened modules
    m_config.setGroup( "Session");
    m_config.clearGroup();
    QObjectList *childlist = queryList( "TextWidget" );
    QObjectListIt it( *childlist );
    TextWidget *module;
    int count = 1;
    while ( ( module = reinterpret_cast<TextWidget *>(it.current()) ) != 0 )
    {
        QString modkey;
        modkey.sprintf( "%s/%s", module->getModuleName().latin1(), module->getAbbrevKey().latin1() );
        m_config.writeEntry( QString::number( count ), modkey );
        ++count;
        ++it;
    }
}

bool MainWindow::eventFilter( QObject *obj, QEvent *event )
{
    if ( event->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyev = reinterpret_cast<QKeyEvent *>(event);
        if ( keyev->key() == Key_Up )
        {
            slotNavPrevVerse();
            return true;
        }
        else if ( keyev->key() == Key_Down )
        {
            slotNavNextVerse();
            return true;
        }
    }

    return QWidget::eventFilter( obj, event );
}

void MainWindow::initUI()
{
    setCentralWidget( &m_tabs );
    m_tabs.installEventFilter( this );

    setToolBarsMovable( false );
    m_barDock = new QToolBar( this );
    m_barDock->setHorizontalStretchable( true );

    m_menuBar = new QMenuBar( m_barDock );
    m_menuBar->setMargin( 0 );

    // Allocate toolbars
    m_navToolbar = new NavBar( this );
    m_navToolbar->navBtnsEnable( false );
    connect( m_navToolbar, SIGNAL(prevChapter()), this, SLOT(slotNavPrevChapter()) );
    connect( m_navToolbar, SIGNAL(prevVerse()), this, SLOT(slotNavPrevVerse()) );
    connect( m_navToolbar, SIGNAL(keyChanged(const QString &)), this, SLOT(slotNavKeyChanged(const QString &)) );
    connect( m_navToolbar, SIGNAL(nextVerse()), this, SLOT(slotNavNextVerse()) );
    connect( m_navToolbar, SIGNAL(nextChapter()), this, SLOT(slotNavNextChapter()) );
    connect( m_navToolbar, SIGNAL(autoScroll(bool)), this, SLOT(slotNavAutoScroll(bool)) );
    connect( m_navToolbar, SIGNAL(scrollRateChanged(int)), this, SLOT(slotNavScrollRateChanged(int)) );

    m_searchToolbar = new SearchBar( this );
    connect( m_searchToolbar, SIGNAL(sigResultClicked(const QString &)), this, SLOT(slotSearchResultClicked(const QString &)) );

    // Text menu
    QPopupMenu *popup = new QPopupMenu( this );

    QAction *a = new QAction( tr( "Open..." ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotTextOpen()) );
    a->addTo( popup );

    m_actionTextClose = new QAction( tr( "Close" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    connect( m_actionTextClose, SIGNAL(activated()), this, SLOT(slotTextClose()) );
    m_actionTextClose->addTo( popup );

    popup->insertSeparator();

    // TODO - need to implent
    a = new QAction( tr( "Install..." ), Resource::loadPixmap( "install" ), QString::null, 0, this, 0 );
    a->setEnabled( false );
    connect( a, SIGNAL(activated()), this, SLOT(slotTextInstall()) );
    a->addTo( popup );

    m_menuBar->insertItem( tr( "Text" ), popup );

    // Edit menu
    popup = new QPopupMenu( this );

    m_actionEditCopy = new QAction( tr( "Copy" ), Resource::loadPixmap( "copy" ), QString::null, 0, this, 0 );
    connect( m_actionEditCopy, SIGNAL(activated()), this, SLOT(slotEditCopy()) );
    m_actionEditCopy->addTo( popup );

    popup->insertSeparator();

    a = new QAction( tr( "Configure..." ), Resource::loadPixmap( "SettingsIcon" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotEditConfigure()) );
    a->addTo( popup );

    m_menuBar->insertItem( tr( "Edit" ), popup );

    // Bookmark menu
    m_bookmarkMenu = new QPopupMenu( this );

    m_actionBookmarkAdd = new QAction( tr( "Add" ), Resource::loadPixmap( "dagger/bookmarkadd" ), QString::null, 0, this, 0 );
    connect( m_actionBookmarkAdd, SIGNAL(activated()), this, SLOT(slotBookmarkAdd()) );
    m_actionBookmarkAdd->addTo( m_bookmarkMenu );

    m_actionBookmarkRemove = new QAction( tr( "Remove" ), Resource::loadPixmap( "dagger/bookmarkremove" ), QString::null, 0, this, 0 );
    connect( m_actionBookmarkRemove, SIGNAL(activated()), this, SLOT(slotBookmarkRemove()) );
    m_actionBookmarkRemove->addTo( m_bookmarkMenu );

    m_bookmarkMenu->insertSeparator();

    m_menuBar->insertItem( tr( "Bookmark" ), m_bookmarkMenu );

    // View menu
    popup = new QPopupMenu( this );

    // Retrieve list of available Sword module options (e.g. footnotes, Strong's numbers, etc.)
    for ( a = m_actionSwordOpts.first(); a; a = m_actionSwordOpts.next() )
    {
        a->setToggleAction( true );
        connect( a, SIGNAL(toggled(bool)), this, SLOT(slotViewSwordOption(bool)) );
        a->addTo( popup );
    }

    popup->insertSeparator();

    m_actionViewNavToolbar = new QAction( tr( "Navigation toolbar" ), QString::null, 0, this, 0 );
    m_actionViewNavToolbar->setToggleAction( true );
    connect( m_actionViewNavToolbar, SIGNAL(toggled(bool)), this, SLOT(slotViewNavToolbar(bool)) );
    m_actionViewNavToolbar->addTo( popup );

    m_actionViewSearchToolbar = new QAction( tr( "Search toolbar" ), QString::null, 0, this, 0 );
    m_actionViewSearchToolbar->setToggleAction( true );
    connect( m_actionViewSearchToolbar, SIGNAL(toggled(bool)), this, SLOT(slotViewSearchToolbar(bool)) );
    m_actionViewSearchToolbar->addTo( popup );

    m_menuBar->insertItem( tr( "View" ), popup );
}

void MainWindow::openModule( const QString &modulename, const QString &key )
{
    sword::SWModule *module = m_swordMgr->Modules[ modulename.latin1() ];
    if ( module )
    {
        TextWidget *tw = 0x0;

        if ( !m_alwaysOpenNew )
        {
            // Try to find if the module is already opened, if so will use that TextWidget
            QObjectList *childlist = queryList( "TextWidget" );
            QObjectListIt it( *childlist );
            while ( ( tw = reinterpret_cast<TextWidget *>(it.current()) ) != 0 &&
                    tw->getModuleName() != modulename )
                ++it;
            if ( tw && tw->getModuleName() == modulename )
            {
                // Set key if one is present
                if ( !key.isNull() )
                    tw->setKey( key );

                // Raise tab
                m_tabs.setCurrentTab( tw );
            }
        }

        if ( m_alwaysOpenNew || !tw )
        {
            // Open module in new tab
            QString icon;
            QString type = module->Type();

            if ( type == "Biblical Texts" )
                icon = "dagger/bibletext";
            else if ( type == "Commentaries" )
                icon = "dagger/commentary";
            else if ( type == "Lexicons / Dictionaries" )
                icon = "dagger/lexicon";

            tw = new TextWidget( this, module, m_numVerses, &m_textFont );
            connect( tw, SIGNAL(sigRefClicked(const QString &)),
                     this, SLOT(slotTextRefClicked(const QString &)) );
            connect( this, SIGNAL(sigNumVersesChanged(int)), tw, SLOT(slotNumVersesChanged(int)) );
            connect( this, SIGNAL(sigFontChanged(const QFont *)), tw, SLOT(slotFontChanged(const QFont *)) );
            connect( this, SIGNAL(sigOptionChanged()), tw, SLOT(slotOptionChanged()) );

            m_tabs.addTab( tw, icon, modulename );

            m_actionTextClose->setEnabled( true );
            m_actionEditCopy->setEnabled( true );
            m_actionBookmarkAdd->setEnabled( true );

            // Set key if one is present
            if ( !key.isNull() )
                tw->setKey( key );
        }
    }
}

int MainWindow::findBookmark( const QString &bookmark )
{
    int index = 3;
    int id = m_bookmarkMenu->idAt( index );
    while ( ( id != -1 ) && ( m_bookmarkMenu->text( id ) != bookmark ) )
    {
        ++index;
        id = m_bookmarkMenu->idAt( index );
    }

    return id;
}

void MainWindow::enableScreenBlanking( bool enable )
{
    enable ? QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable
           : QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Disable;
}

void MainWindow::initConfig()
{
    bool show;

    m_config.setGroup( "Sword" );
    for ( QAction *a = m_actionSwordOpts.first(); a; a = m_actionSwordOpts.next() )
    {
        show = m_config.readBoolEntry( a->text(), false );
        a->setOn( show );
        m_swordMgr->setGlobalOption ( a->text(), show ? "On" : "Off" );
    }

    // Display/hide toolbars based on last run
    m_config.setGroup( "Config" );

    m_alwaysOpenNew = m_config.readBoolEntry( "AlwaysOpenNew", false );
    m_navToolbar->setAutoScrollRate( m_config.readNumEntry( "AutoScroll", 50 ) );
    m_disableScreenBlank = m_config.readBoolEntry( "DisableScreenBlanking", false );
    enableScreenBlanking( !m_disableScreenBlank );
    m_copyFormat = m_config.readNumEntry( "CopyFormat", 0 );

    show = m_config.readBoolEntry( "NavBar", false );
    m_actionViewNavToolbar->setOn( show );
    slotViewNavToolbar( show );

    m_numVerses = m_config.readNumEntry( "NumVerses", 5 );

    show = m_config.readBoolEntry( "SearchBar", false );
    m_actionViewSearchToolbar->setOn( show );
    slotViewSearchToolbar( show );

    // Set text font
    m_config.setGroup( "Font" );
    QString fontFamily = m_config.readEntry( "Family", QString::null );
    !fontFamily.isNull() ? m_textFont = QFont( fontFamily,
                                               m_config.readNumEntry( "Size", -1 ),
                                               m_config.readNumEntry( "Weight", QFont::Normal ),
                                               m_config.readBoolEntry( "Italic", false ) )
                         : m_textFont = font(); // If font is not configured, set to default widget font

    // Load bookmarks
    m_config.setGroup( "Bookmarks");
    int count = 1;
    QString key = m_config.readEntry( QString::number( count ), QString::null );
    while ( !key.isNull() )
    {
        int pos = key.find( "/" );
        if ( pos > -1 )
        {
            QString bookmark;
            bookmark.sprintf( "%s (%s)", key.right( key.length() - ( pos + 1 ) ).latin1(),
                                         key.left( pos ).latin1() );
            QAction *a = new QAction( bookmark, QString::null, 0, this, 0 );
            a->addTo( m_bookmarkMenu );
            connect( a, SIGNAL(activated()), this, SLOT(slotBookmarkSelected()) );
        }

        ++count;
        key = m_config.readEntry( QString::number( count ), QString::null );
    }
    m_actionBookmarkRemove->setEnabled( count > 1 );

    // Load opened modules
    m_config.setGroup( "Session");
    QString first;
    count = 1;
    key = m_config.readEntry( QString::number( count ), QString::null );
    while ( !key.isNull() )
    {
        int pos = key.find( "/" );
        if ( pos > -1 )
        {
            if ( count == 1 )
                first = key.left( pos );
            openModule( key.left( pos ), key.right( key.length() - ( pos + 1 ) ) );
        }

        ++count;
        key = m_config.readEntry( QString::number( count ), QString::null );
    }
    m_tabs.setCurrentTab( first );
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );
        m_navToolbar->setKey( text->getAbbrevKey() );
    }
    m_actionTextClose->setEnabled( count > 1 );
    m_actionEditCopy->setEnabled( count > 1 );
}

void MainWindow::slotTextDisplayed( QWidget *textWidget )
{
    TextWidget *text = reinterpret_cast<TextWidget *>(textWidget);
    setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );

    m_navToolbar->setKey( text->getAbbrevKey() );
    m_navToolbar->navBtnsEnable( text->isBibleText() );

    m_searchToolbar->setCurrModule( text );
}

void MainWindow::slotTextOpen()
{
    OpenTextDlg dlg( this, m_swordMgr, m_bibleIcon, m_commentaryIcon, m_lexiconIcon );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
    {
        openModule( dlg.selectedText() );
    }
}

void MainWindow::slotTextClose()
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        m_tabs.removePage( text );
        delete text;

        // If no other modules are open, disable appropriate UI items
        if ( !m_tabs.currentWidget() )
        {
            m_navToolbar->navBtnsEnable( false );
            m_navToolbar->setKey( QString::null );
            m_searchToolbar->setCurrModule( 0x0 );
            m_actionTextClose->setEnabled( false );
            m_actionEditCopy->setEnabled( false );
            m_actionBookmarkAdd->setEnabled( false );
            m_actionBookmarkRemove->setEnabled( false );
        }
    }
}

void MainWindow::slotTextInstall()
{
}

void MainWindow::slotEditCopy()
{
    TextWidget *currModule = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( currModule )
    {
        QString text;

        switch( m_copyFormat )
        {
            case 0: text.sprintf( "%s (%s, %s)", currModule->getCurrVerse().latin1(),
                                                 currModule->getAbbrevKey().latin1(),
                                                 currModule->getModuleName().latin1() );
                break;
            case 1: text.sprintf( "%s (%s)", currModule->getCurrVerse().latin1(),
                                             currModule->getAbbrevKey().latin1() );
                break;
            case 2: text = currModule->getCurrVerse();
                break;
            case 3: text = currModule->getAbbrevKey();
                break;
            default: text = QString::null;
        };

        if ( !text.isNull() )
            QPEApplication::clipboard()->setText( text );
    }
}

void MainWindow::slotEditConfigure()
{
    ConfigureDlg dlg( this, m_modulePath, m_alwaysOpenNew, m_numVerses, m_disableScreenBlank, m_copyFormat,
                      &m_textFont );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
    {
        m_modulePath = dlg.swordPath();
        m_alwaysOpenNew = dlg.alwaysOpenNew();
        if ( dlg.numVerses() != m_numVerses )
        {
            m_numVerses = dlg.numVerses();
            emit sigNumVersesChanged( m_numVerses );
        }
        m_disableScreenBlank = dlg.screenBlank();
        enableScreenBlanking( !m_disableScreenBlank );
        m_copyFormat = dlg.copyFormat();
        m_textFont = dlg.selectedFont();
        emit sigFontChanged( &m_textFont );
    }
}

void MainWindow::slotBookmarkAdd()
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        // See if bookmark doesn't already exists
        QString bookmark = text->getFullKey();
        int menuId = findBookmark( bookmark );
        if ( menuId == -1 )
        {
            // Bookmark not found, add
            QAction *a = new QAction( bookmark, QString::null, 0, this, 0 );
            a->addTo( m_bookmarkMenu );
            connect( a, SIGNAL(activated()), this, SLOT(slotBookmarkSelected()) );

            // Make sure remove option is enabled
            m_actionBookmarkRemove->setEnabled( true );

        }
    }
}

void MainWindow::slotBookmarkRemove()
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        // See if bookmark exists for current module key
        int menuId = findBookmark( text->getFullKey() );
        if ( menuId != -1 )
        {
            // Bookmark found, remove
            m_bookmarkMenu->removeItem( menuId );

            //If this was the last bookmark, disable the remove option
            if ( m_bookmarkMenu->idAt( 3 ) == -1 )
                m_actionBookmarkRemove->setEnabled( false );
        }
    }
}

void MainWindow::slotBookmarkSelected()
{
    const QAction *action = reinterpret_cast<const QAction *>(sender());
    if ( action )
    {
        QString bookmark = action->text();
        int pos = bookmark.find( " (" );
        QString key = bookmark.left( pos );
        pos += 2;
        QString module = bookmark.mid( pos, bookmark.find( ")", pos ) - pos );

        openModule( module, key );
    }
}

void MainWindow::slotViewSwordOption( bool enabled )
{
    const QAction *action = reinterpret_cast<const QAction*>(sender());
    m_swordMgr->setGlobalOption ( action->text(), enabled ? "On" : "Off" );

    emit sigOptionChanged();
}

void MainWindow::slotViewNavToolbar( bool enabled )
{
    enabled ? m_navToolbar->show()
            : m_navToolbar->hide();
}

void MainWindow::slotViewSearchToolbar( bool enabled )
{
    enabled ? m_searchToolbar->show()
            : m_searchToolbar->hide();
}

void MainWindow::slotNavPrevChapter()
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        text->prevChapter();
        setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );
        m_navToolbar->setKey( text->getAbbrevKey() );
    }
}

void MainWindow::slotNavPrevVerse()
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        text->prevVerse();
        setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );
        m_navToolbar->setKey( text->getAbbrevKey() );
    }
}

void MainWindow::slotNavKeyChanged( const QString &newKey )
{
    QString key = newKey;
    key.replace( QRegExp( "[-=.]" ), ":" );

    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        text->setKey( key );
        setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );
    }
}

void MainWindow::slotNavNextVerse()
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        text->nextVerse();
        setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );
        m_navToolbar->setKey( text->getAbbrevKey() );
    }
}

void MainWindow::slotNavNextChapter()
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        text->nextChapter();
        setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );
        m_navToolbar->setKey( text->getAbbrevKey() );
    }
}

void MainWindow::slotNavAutoScroll( bool enabled )
{
    m_autoScrollTimer.stop();

    if ( enabled )
        m_autoScrollTimer.start( m_navToolbar->autoScrollRate() * 100 );
}

void MainWindow::slotNavScrollRateChanged( int newRate )
{
    if ( m_autoScrollTimer.isActive() )
    {
        m_autoScrollTimer.stop();
        m_autoScrollTimer.start( newRate * 100 );
    }
}

void MainWindow::slotSearchResultClicked( const QString &key )
{
    TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
    if ( text )
    {
        text->setKey( key );
        setCaption( QString( "%1 - Dagger" ).arg( text->getFullKey() ) );
        m_navToolbar->setKey( text->getAbbrevKey() );
    }
}

void MainWindow::slotTextRefClicked( const QString &ref )
{
    //owarn << "Reference: " << ref << oendl;
    if ( !ref.isNull() )
    {
        TextWidget *text = reinterpret_cast<TextWidget *>(m_tabs.currentWidget());
        if ( text )
        {
            QString module;
            QString key( ref );
            key.remove( 0, 2 );

            QChar book = ref.at( 1 );
            // TODO- this is ugly, need better way to determine type of reference
            //       take a look at SWModule::getRawEntry()
            int keyValue = key.toInt();
            if ( book == 'H' && keyValue <= 8674 )
                module = "StrongsHebrew";
            else if ( book == 'G' && keyValue <= 5624 )
                module = "StrongsGreek";

            if ( !module.isEmpty() )
                openModule( module, key );
        }
    }
}
