#include <qpe/qpemenubar.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpopupmenu.h>
#include <qwhatsthis.h>

#include "mainwindow.h"
#include "ircservertab.h"
#include "ircserverlist.h"
#include "ircsettings.h"

MainWindow::MainWindow(QWidget *parent, const char *name, WFlags) : QMainWindow(parent, name, WStyle_ContextHelp) {
    setCaption(tr("IRC Client"));
    m_tabWidget = new IRCTabWidget(this);
    QWhatsThis::add(m_tabWidget, tr("Server connections, channels, queries and other things will be placed here"));
    connect(m_tabWidget, SIGNAL(currentChanged(QWidget *)), this, SLOT(selected(QWidget *)));
    setCentralWidget(m_tabWidget);
    setToolBarsMovable(FALSE);
    QPEMenuBar *menuBar = new QPEMenuBar(this);
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
    loadSettings();
}

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
}

void MainWindow::selected(QWidget *) {
    m_tabWidget->setTabColor(m_tabWidget->currentPageIndex(), black);
}

void MainWindow::addTab(IRCTab *tab) {
    connect(tab, SIGNAL(changed(IRCTab *)), this, SLOT(changeEvent(IRCTab *)));
    m_tabWidget->addTab(tab, tab->title());
    m_tabWidget->showPage(tab);
    tab->setID(m_tabWidget->currentPageIndex());
    m_tabs.append(tab);
}

void MainWindow::changeEvent(IRCTab *tab) {
    if (tab->id() != m_tabWidget->currentPageIndex())
        m_tabWidget->setTabColor(tab->id(), blue);
}

void MainWindow::killTab(IRCTab *tab) {
    m_tabWidget->removePage(tab);
    m_tabs.remove(tab);
    /* there might be nicer ways to do this .. */
    delete tab;
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
