#include <qpe/qpemenubar.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpopupmenu.h>

#include "mainwindow.h"
#include "ircservertab.h"
#include "ircserverlist.h"
#include "ircsettings.h"

MainWindow::MainWindow(QWidget *parent, const char *name, WFlags f) : QMainWindow(parent, name, f) {
    setCaption(tr("IRC Client"));
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    setToolBarsMovable(FALSE);
    QPEMenuBar *menuBar = new QPEMenuBar(this);
    QPopupMenu *irc = new QPopupMenu(this);
    menuBar->insertItem(tr("IRC"), irc);
    QAction *a = new QAction(tr("New connection"), Resource::loadPixmap("pass"), QString::null, 0, this, 0);
    connect(a, SIGNAL(activated()), this, SLOT(newConnection()));
    a->addTo(irc);
    a = new QAction(tr("Settings"), Resource::loadPixmap("SettingsIcon"), QString::null, 0, this, 0);
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
}

void MainWindow::addTab(IRCTab *tab) {
    m_tabWidget->addTab(tab, tab->title());
    m_tabWidget->showPage(tab);
    m_tabs.append(tab);
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
