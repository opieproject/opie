#include <qmenubar.h>
#include <qpe/resource.h>

#include <opie2/odebug.h>

#include <qwhatsthis.h>

#include "mainwindow.h"
#include "ircservertab.h"
#include "dcctransfertab.h"
#include "ircserverlist.h"
#include "ircsettings.h"

#include <stdio.h>


QString MainWindow::appCaption() {
    return QObject::tr("Opie IRC");
}


MainWindow::MainWindow(QWidget *parent, const char *name, WFlags) : QMainWindow(parent, name, WStyle_ContextHelp) {
    setCaption(tr("IRC Client"));
    m_tabWidget = new IRCTabWidget(this);
    QWhatsThis::add(m_tabWidget, tr("Server connections, channels, queries and other things will be placed here"));
    connect(m_tabWidget, SIGNAL(currentChanged(QWidget*)), this, SLOT(selected(QWidget*)));
    setCentralWidget(m_tabWidget);
    setToolBarsMovable(FALSE);
    QMenuBar *menuBar = new QMenuBar(this);
    QPopupMenu *irc = new QPopupMenu(this);
    menuBar->insertItem(tr("IRC"), irc);
    QAction *a = new QAction(tr("New connection"), Resource::loadPixmap("pass"), QString::null, 0, this, 0);
    connect(a, SIGNAL(activated()), this, SLOT(newConnection()));
    a->setWhatsThis(tr("Create a new connection to an IRC server"));
    a->addTo(irc);
    a = new QAction(tr("Settings"), Resource::loadPixmap("SettingsIcon"), QString::null, 0, this, 0);
    a->setWhatsThis(tr("Configure OpieIRC's behavior and appearance"));
    connect(a, SIGNAL(activated()), this, SLOT(settings()));
    a->addTo(irc);
    m_dccTab = 0;
    loadSettings();
}

/*IRCTabWidget MainWindow::getTabWidget(){
  return m_tabWidget;
} */

void MainWindow::loadSettings() {
    Config config("OpieIRC");
    config.setGroup("OpieIRC");
    IRCTab::m_backgroundColor = config.readEntry("BackgroundColor", "#FFFFFF");
    IRCTab::m_textColor = config.readEntry("TextColor", "#000000");
    IRCTab::m_errorColor = config.readEntry("ErrorColor", "#FF0000");
    IRCTab::m_selfColor = config.readEntry("SelfColor", "#CC0000");
    IRCTab::m_otherColor = config.readEntry("OtherColor", "#0000BB");
    IRCTab::m_serverColor = config.readEntry("ServerColor", "#0000FF");
    IRCTab::m_notificationColor = config.readEntry("NotificationColor", "#AA3300");
    IRCTab::m_maxLines = config.readNumEntry("Lines", 100);
    IRCTab::setUseTimeStamps( config.readBoolEntry("DisplayTime", false ) );
}

void MainWindow::selected(QWidget *) {
    m_tabWidget->setTabColor(m_tabWidget->currentPageIndex(), black);
    emit updateScroll();
}

void MainWindow::addTab(IRCTab *tab) {
    connect(tab, SIGNAL(changed(IRCTab*)), this, SLOT(changeEvent(IRCTab*)));
    connect(tab, SIGNAL(ping (const QString&)), this, SLOT(slotPing(const QString&)));
    connect(tab, SIGNAL(nextTab()), this, SLOT(slotNextTab()));
    connect(tab, SIGNAL(prevTab()), this, SLOT(slotPrevTab()));

    m_tabWidget->addTab(tab, tab->title());
    m_tabWidget->showPage(tab);
    tab->setID(m_tabWidget->currentPageIndex());
    m_tabs.append(tab);
}

void MainWindow::changeEvent(IRCTab *tab) {
    if (tab->id() != m_tabWidget->currentPageIndex())
        m_tabWidget->setTabColor(tab->id(), blue);
}

void MainWindow::killTab(IRCTab *tab, bool imediate) {
    if (tab == m_dccTab)
        m_dccTab = 0;
    
    m_toDelete.append( tab );

    if ( imediate )
        slotKillTabsLater();
    else
        QTimer::singleShot(0, this, SLOT(slotKillTabsLater()) );
}

void MainWindow::slotKillTabsLater() {
    for ( QListIterator<IRCTab> it(m_toDelete); it.current(); ++it ) {
        m_tabWidget->removePage( it.current() );
        odebug << it.current() << oendl;
        m_tabs.remove( it.current() );
    }

    m_toDelete.setAutoDelete( true );
    m_toDelete.clear();
    m_toDelete.setAutoDelete( false );
}

void MainWindow::newConnection() {
    IRCServerList list(this, "ServerList", TRUE);
    if (list.exec() == QDialog::Accepted && list.hasServer()) {
        IRCServerTab *serverTab = new IRCServerTab(list.server(), this, m_tabWidget);
        addTab(serverTab);
        serverTab->doConnect();
    }
}

void MainWindow::settings() {
    IRCSettings settings(this, "Settings", TRUE);
    if (settings.exec() == QDialog::Accepted) {
        QListIterator<IRCTab> it(m_tabs);
        for (; it.current(); ++it) {
            /* Inform all tabs about the new settings */
            it.current()->settingsChanged();
        }
    }
}


void MainWindow::slotNextTab() {
    int i = m_tabWidget->currentPageIndex ();
    m_tabWidget->setCurrentPage ( i+1 );

    int j = m_tabWidget->currentPageIndex ();
    if ( i == j )
        m_tabWidget->setCurrentPage ( 1 );
}

void MainWindow::slotPrevTab() {
    int i = m_tabWidget->currentPageIndex ();
    if ( i > 1 )
        m_tabWidget->setCurrentPage ( i-1 );
}

void MainWindow::slotPing( const QString& /*channel*/ ) {
    raise();
}

void MainWindow::addDCC(DCCTransfer::Type type, Q_UINT32 ip4Addr, Q_UINT16 port, 
        const QString &filename, const QString &nickname, unsigned int size) {
    
    if (!m_dccTab) {
        m_dccTab = new DCCTransferTab(this);
        addTab(m_dccTab);
        m_dccTab->show();
    }
    
    m_dccTab->addTransfer(type, ip4Addr, port, filename, nickname, size);
}

