#include <stdio.h>
#include <qtextstream.h>
#include "ircservertab.h"

IRCServerTab::IRCServerTab(IRCServer server, MainWindow *mainWindow, QWidget *parent, const char *name, WFlags f) : IRCTab(parent, name, f) {
    m_server = server;
    m_session = new IRCSession(&m_server);
    m_mainWindow = mainWindow;
    m_close = FALSE;
    m_description->setText(tr("Connection to")+" <b>" + server.hostname() + ":" + QString::number(server.port()) + "</b>");
    m_textview = new QTextView(this);
    m_textview->setHScrollBarMode(QScrollView::AlwaysOff);
    m_textview->setVScrollBarMode(QScrollView::AlwaysOn);
    m_layout->add(m_textview);
    m_field = new QLineEdit(this);
    m_layout->add(m_field);
    connect(m_field, SIGNAL(returnPressed()), this, SLOT(processCommand()));
    m_field->setFocus();
    connect(m_session, SIGNAL(outputReady(IRCOutput)), this, SLOT(display(IRCOutput)));
}

void IRCServerTab::appendText(QString text) {
    /* not using append because it creates layout problems */
    m_textview->setText(m_textview->text() + text);
    m_textview->ensureVisible(0, m_textview->contentsHeight());
}

IRCServerTab::~IRCServerTab() {
    delete m_session;
}

void IRCServerTab::removeChannelTab(IRCChannelTab *tab) {
    m_channelTabs.remove(tab);
}

QString IRCServerTab::title() {
    return "Server";
}

IRCSession *IRCServerTab::session() {
    return m_session;
}

IRCServer *IRCServerTab::server() {
    return &m_server;
}

void IRCServerTab::executeCommand(IRCTab *tab, QString line) {
    QTextIStream stream(&line);
    QString command;
    stream >> command;
    command = command.upper().right(command.length()-1);
    
    if (command == "JOIN") {
        QString channel;
        stream >> channel;
        if (channel.length() > 0 && channel.startsWith("#")) {
            m_session->join(channel);
        } else {
            tab->appendText("<font color=\"#ff0000\">Unknown channel format!</font><br>"); 
        }
    } else {
        tab->appendText("<font color=\"#ff0000\">Unknown command</font><br>"); 
    }
}

void IRCServerTab::processCommand() {
    QString text = m_field->text();
    if (text.startsWith("/") && !text.startsWith("//")) {
        /* Command mode */
        executeCommand(this, text);
    }
    m_field->clear();
}

void IRCServerTab::doConnect() {
    m_session->beginSession();
}

void IRCServerTab::remove() {
    /* Close requested */
    if (m_session->isSessionActive()) {
        /* While there is a running session */
        m_close = TRUE;
        m_session->endSession();
    } else {
        /* Session has previously been closed */
        m_channelTabs.first();
        while (m_channelTabs.current() != 0) {
            m_mainWindow->killTab(m_channelTabs.current());
        }
        m_mainWindow->killTab(this);
    }
}

IRCChannelTab *IRCServerTab::getTabForChannel(IRCChannel *channel) {
    QListIterator<IRCChannelTab> it(m_channelTabs);
    
    for (; it.current(); ++it) {
        if (it.current()->channel() == channel)
            return it.current();
    }
    return 0;
}

void IRCServerTab::display(IRCOutput output) {

    /* All messages to be displayed inside the GUI get here */
    switch (output.type()) {
        case OUTPUT_CONNCLOSE:
            if (m_close) {
                m_channelTabs.first();
                while (m_channelTabs.current() != 0) {
                    m_mainWindow->killTab(m_channelTabs.current());
                }
                m_mainWindow->killTab(this);
            } else {
                appendText("<font color=\"#0000dd\">" + output.htmlMessage() +"</font><br>");
                QListIterator<IRCChannelTab> it(m_channelTabs);
                for (; it.current(); ++it) {
                    it.current()->appendText("<font color=\"#0000dd\">" + output.htmlMessage() +"</font><br>");
                }
            }
            break;
        case OUTPUT_SELFJOIN: {
                IRCChannelTab *channeltab = new IRCChannelTab((IRCChannel *)output.getParam(0), this, m_mainWindow, (QWidget *)parent());
                m_channelTabs.append(channeltab);
                m_mainWindow->addTab(channeltab);
            }
            break;
        case OUTPUT_CHANPRIVMSG: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                channelTab->appendText("&lt;<font color=\"#0000dd\">"+((IRCChannelPerson *)output.getParam(1))->person->nick()+"</font>&gt; "+output.htmlMessage()+"<br>");
            }
            break;
        case OUTPUT_SELFPART: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                if (channelTab)
                    m_mainWindow->killTab(channelTab);
            }
            break;
        case OUTPUT_SELFKICK: {
                appendText("<font color=\"#ff0000\">" + output.htmlMessage() + "</font><br>");
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                if (channelTab)
                    m_mainWindow->killTab(channelTab);
            }
            break;
        case OUTPUT_CHANACTION: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                channelTab->appendText("<font color=\"#cc0000\">"+output.htmlMessage()+"</font><br>");
            }
            break;
        case OUTPUT_QUIT: {
                QString nick = ((IRCPerson *)output.getParam(0))->nick();
                QListIterator<IRCChannelTab> it(m_channelTabs);
                for (; it.current(); ++it) {
                    if (it.current()->list()->hasPerson(nick)) {
                        it.current()->appendText("<font color=\"#aa3e00\">"+output.htmlMessage()+"</font><br>");
                        it.current()->list()->update();
                    }
                }
            }
            break;
        case OUTPUT_OTHERJOIN:
        case OUTPUT_OTHERKICK:
        case OUTPUT_CHANPERSONMODE:
        case OUTPUT_OTHERPART: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                channelTab->appendText("<font color=\"#aa3e00\">"+output.htmlMessage()+"</font><br>");
                channelTab->list()->update();
            }
            break;
        case OUTPUT_CTCP:
            appendText("<font color=\"#00bb00\">" + output.htmlMessage() + "</font><br>");
            break;
        case OUTPUT_ERROR:
            appendText("<font color=\"#ff0000\">" + output.htmlMessage() + "</font><br>");
            break;
        default:
            appendText("<font color=\"#0000dd\">" + output.htmlMessage() + "</font><br>");
            break;
    }
}
