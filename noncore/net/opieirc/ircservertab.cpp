#include <qpe/config.h>
#include <qtextstream.h>
#include <qwhatsthis.h>
#include "ircservertab.h"

IRCServerTab::IRCServerTab(IRCServer server, MainWindow *mainWindow, QWidget *parent, const char *name, WFlags f) : IRCTab(parent, name, f) {
    m_server = server;
    m_session = new IRCSession(&m_server);
    m_mainWindow = mainWindow;
    m_close = FALSE;
    m_lines = 0;
    m_description->setText(tr("Connection to")+" <b>" + server.hostname() + ":" + QString::number(server.port()) + "</b>");
    m_textview = new QTextView(this);
    m_textview->setHScrollBarMode(QScrollView::AlwaysOff);
    m_textview->setVScrollBarMode(QScrollView::AlwaysOn);
    m_textview->setTextFormat(RichText);
    QWhatsThis::add(m_textview, tr("Server messages"));
    m_layout->add(m_textview);
    m_field = new IRCHistoryLineEdit(this);
    QWhatsThis::add(m_textview, tr("Type commands here. A list of available commands can be found inside the OpieIRC help"));
    m_layout->add(m_field);
    connect(m_field, SIGNAL(returnPressed()), this, SLOT(processCommand()));
    m_field->setFocus();
    connect(m_session, SIGNAL(outputReady(IRCOutput)), this, SLOT(display(IRCOutput)));
    settingsChanged();
}

void IRCServerTab::appendText(QString text) {
    /* not using append because it creates layout problems */
    QString txt = m_textview->text() + text + "\n";
    if (m_maxLines > 0 && m_lines >= m_maxLines) {
        int firstBreak = txt.find('\n');
        if (firstBreak != -1) {
            txt = "<qt bgcolor=\"" + m_backgroundColor + "\"/>" + txt.right(txt.length() - (firstBreak + 1));
        }
    } else {
        m_lines++;
    }
    m_textview->setText(txt);
    m_textview->ensureVisible(0, m_textview->contentsHeight());
    emit changed(this);
}

IRCServerTab::~IRCServerTab() {
    delete m_session;
}

void IRCServerTab::removeChannelTab(IRCChannelTab *tab) {
    m_channelTabs.remove(tab);
}

void IRCServerTab::removeQueryTab(IRCQueryTab *tab) {
    m_queryTabs.remove(tab);
}

void IRCServerTab::addQueryTab(IRCQueryTab *tab) {
    m_queryTabs.append(tab);
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

void IRCServerTab::settingsChanged() {
    m_textview->setText("<qt bgcolor=\"" + m_backgroundColor + "\"/>");
    m_lines = 0;
}

void IRCServerTab::executeCommand(IRCTab *tab, QString line) {
    QTextIStream stream(&line);
    QString command;
    stream >> command;
    command = command.upper().right(command.length()-1);
    
    if (command == "JOIN") {
        QString channel;
        stream >> channel;
        if (channel.length() > 0 && (channel.startsWith("#") || channel.startsWith("+"))) {
            m_session->join(channel);
        } else {
            tab->appendText("<font color=\"" + m_errorColor + "\">Unknown channel format!</font><br>"); 
        }
    } else if (command == "ME") {
        QString text = line.right(line.length()-4);
        if (text.length() > 0) {
            if (tab->isA("IRCChannelTab")) {
                tab->appendText("<font color=\"" + m_selfColor + "\">*" + IRCOutput::toHTML(m_server.nick()) + " " + IRCOutput::toHTML(text) + "</font><br>");
                m_session->sendAction(((IRCChannelTab *)tab)->channel(), text);
            } else if (tab->isA("IRCQueryTab")) {
                tab->appendText("<font color=\"" + m_selfColor + "\">*" + IRCOutput::toHTML(m_server.nick()) + " " + IRCOutput::toHTML(text) + "</font><br>");
                m_session->sendAction(((IRCQueryTab *)tab)->person(), text);
            } else {
                tab->appendText("<font color=\"" + m_errorColor + "\">Invalid tab for this command</font><br>"); 
            }
        }
    } else if (command == "MSG") {
        QString nickname;
        stream >> nickname;
        if (nickname.length() > 0) {
            if (line.length() > 6 + nickname.length()) {
                QString text = line.right(line.length()-nickname.length()-6);
                IRCPerson person;
                person.setNick(nickname);
                tab->appendText("<font color=\"" + m_textColor + "\">&gt;</font><font color=\"" + m_otherColor + "\">"+IRCOutput::toHTML(nickname)+"</font><font color=\"" + m_textColor + "\">&lt; "+IRCOutput::toHTML(text)+"</font><br>");
                m_session->sendMessage(&person, text);
            }
        }    
    } else {
        tab->appendText("<font color=\"" + m_errorColor + "\">Unknown command</font><br>"); 
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
        m_queryTabs.first();
        while (m_queryTabs.current() != 0) {
            m_mainWindow->killTab(m_queryTabs.current());
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

IRCQueryTab *IRCServerTab::getTabForQuery(IRCPerson *person) {
    QListIterator<IRCQueryTab> it(m_queryTabs);
    
    for (; it.current(); ++it) {
        if (it.current()->person()->nick() == person->nick())
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
                m_queryTabs.first();
                while (m_queryTabs.current() != 0) {
                    m_mainWindow->killTab(m_queryTabs.current());
                }
                m_mainWindow->killTab(this);
            } else {
                appendText("<font color=\"" + m_errorColor + "\">" + output.htmlMessage() +"</font><br>");
                QListIterator<IRCChannelTab> it(m_channelTabs);
                for (; it.current(); ++it) {
                    it.current()->appendText("<font color=\"" + m_serverColor + "\">" + output.htmlMessage() +"</font><br>");
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
                channelTab->appendText("<font color=\"" + m_textColor + "\">&lt;</font><font color=\"" + m_otherColor + "\">"+IRCOutput::toHTML(((IRCChannelPerson *)output.getParam(1))->person->nick())+"</font><font color=\"" + m_textColor + "\">&gt; " + output.htmlMessage()+"</font><br>");
            }
            break;
        case OUTPUT_QUERYACTION:
        case OUTPUT_QUERYPRIVMSG: {
                IRCQueryTab *queryTab = getTabForQuery((IRCPerson *)output.getParam(0));
                if (!queryTab) {
                    queryTab = new IRCQueryTab((IRCPerson *)output.getParam(0), this, m_mainWindow, (QWidget *)parent());
                    m_queryTabs.append(queryTab);
                    m_mainWindow->addTab(queryTab);
                }
                queryTab->display(output);
            }
            break;
        case OUTPUT_SELFPART: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                if (channelTab)
                    m_mainWindow->killTab(channelTab);
            }
            break;
        case OUTPUT_SELFKICK: {
                appendText("<font color=\"" + m_errorColor + "\">" + output.htmlMessage() + "</font><br>");
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                if (channelTab)
                    m_mainWindow->killTab(channelTab);
            }
            break;
        case OUTPUT_CHANACTION: {
                IRCChannelTab *channelTab = getTabForChannel((IRCChannel *)output.getParam(0));
                channelTab->appendText("<font color=\"" + m_otherColor + "\">"+output.htmlMessage()+"</font><br>");
            }
            break;
        case OUTPUT_TOPIC: {
                IRCChannel *channel = (IRCChannel *) output.getParam(0);
                if (channel) {
                    IRCChannelTab *channelTab = getTabForChannel(channel);
                    if (channelTab) {
                        channelTab->appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
                        return;
                    }
                }
                appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
            }
            break;
        case OUTPUT_QUIT: {
                QString nick = ((IRCPerson *)output.getParam(0))->nick();
                QListIterator<IRCChannelTab> it(m_channelTabs);
                for (; it.current(); ++it) {
                    if (it.current()->list()->hasPerson(nick)) {
                        it.current()->appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
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
                channelTab->appendText("<font color=\"" + m_notificationColor + "\">"+output.htmlMessage()+"</font><br>");
                channelTab->list()->update();
            }
            break;
        case OUTPUT_CTCP:
            appendText("<font color=\"" + m_notificationColor + "\">" + output.htmlMessage() + "</font><br>");
            break;
        case OUTPUT_ERROR:
            appendText("<font color=\"" + m_errorColor + "\">" + output.htmlMessage() + "</font><br>");
            break;
        default:
            appendText("<font color=\"" + m_serverColor + "\">" + output.htmlMessage() + "</font><br>");
            break;
    }
}
