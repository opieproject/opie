#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qcursor.h>
#include <stdio.h>
#include <qhbox.h>
#include "ircchanneltab.h"
#include "ircservertab.h"

IRCChannelTab::IRCChannelTab(IRCChannel *channel, IRCServerTab *parentTab, MainWindow *mainWindow, QWidget *parent, const char *name, WFlags f) : IRCTab(parent, name, f) {
    m_mainWindow = mainWindow;
    m_parentTab = parentTab;
    m_channel = channel;
    m_description->setText(tr("Talking on channel") + " <b>" + channel->channelname() + "</b>");
    QHBox *hbox = new QHBox(this);
    m_textview = new QTextView(hbox);
    m_textview->setHScrollBarMode(QScrollView::AlwaysOff);
    m_textview->setVScrollBarMode(QScrollView::AlwaysOn);
    m_listVisible = TRUE;
    m_listButton = new QPushButton(">", m_textview);
    m_textview->setCornerWidget(m_listButton);
    m_textview->setTextFormat(RichText);
    connect(m_listButton, SIGNAL(clicked()), this, SLOT(toggleList()));
    m_list = new IRCChannelList(m_channel, hbox);
    m_list->update();
    m_list->setMaximumWidth(LISTWIDTH);
    m_field = new IRCHistoryLineEdit(this);
    m_popup = new QPopupMenu(m_list);
    m_lines = 0;
    /* Required so that embedded-style "right" clicks work */
    QPEApplication::setStylusOperation(m_list->viewport(), QPEApplication::RightOnHold);
    connect(m_list, SIGNAL(mouseButtonPressed(int, QListBoxItem *, const QPoint&)), this, SLOT(mouseButtonPressed(int, QListBoxItem *, const QPoint &)));

    /* Construct the popup menu */
    QPopupMenu *ctcpMenu = new QPopupMenu(m_list);
    m_popup->insertItem(Resource::loadPixmap("opieirc/ctcp"), tr("CTCP"), ctcpMenu);
    m_popup->insertItem(Resource::loadPixmap("opieirc/query"), tr("Query"), this, SLOT(popupQuery()));
    ctcpMenu->insertItem(Resource::loadPixmap("opieirc/ping"), tr("Ping"), this, SLOT(popupPing()));
    ctcpMenu->insertItem(Resource::loadPixmap("opieirc/version"), tr("Version"), this, SLOT(popupVersion()));
    ctcpMenu->insertItem(Resource::loadPixmap("opieirc/whois"), tr("Whois"), this, SLOT(popupWhois()));
    
    m_layout->add(hbox);
    hbox->show();
    m_layout->add(m_field);
    m_field->setFocus();
    connect(m_field, SIGNAL(returnPressed()), this, SLOT(processCommand()));
    settingsChanged();
}

void IRCChannelTab::appendText(QString text) {
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

IRCChannelTab::~IRCChannelTab() {
    m_parentTab->removeChannelTab(this);
}

void IRCChannelTab::processCommand() {
    QString text = m_field->text();
    if (text.length()>0) {
        if (session()->isSessionActive()) {
            if (text.startsWith("/") && !text.startsWith("//")) {
                /* Command mode */
                m_parentTab->executeCommand(this, text);;
            } else {
                if (text.startsWith("//"))
                    text = text.right(text.length()-1);
                session()->sendMessage(m_channel, m_field->text());
                appendText("<font color=\"" + m_textColor + "\">&lt;</font><font color=\"" + m_selfColor + "\">"+m_parentTab->server()->nick()+"</font><font color=\"" + m_textColor + "\">&gt; "+IRCOutput::toHTML(m_field->text())+"</font><br>");
            }
        } else {
            appendText("<font color=\"" + m_errorColor + "\">"+tr("Disconnected")+"</font><br>");
        }
    }
    m_field->clear();
}

void IRCChannelTab::settingsChanged() {
    m_textview->setText("<qt bgcolor=\"" + m_backgroundColor + "\"/>");
    m_lines = 0;
}

void IRCChannelTab::toggleList() {
    if (m_listVisible) {
        m_list->setMaximumWidth(0);
        m_listButton->setText("<");
    } else {
        m_list->setMaximumWidth(LISTWIDTH);
        m_listButton->setText(">");
    }
    m_listVisible = !m_listVisible;
}

void IRCChannelTab::mouseButtonPressed(int mouse, QListBoxItem *, const QPoint &point) {
    switch (mouse) {
        case 1:
            break;
        case 2:
            m_popup->popup(point);
            break;
    };
}

void IRCChannelTab::popupQuery() {
    if (m_list->currentItem() != -1) {
        IRCPerson *person = session()->getPerson(m_list->item(m_list->currentItem())->text());
        if (person) {
            IRCQueryTab *tab = m_parentTab->getTabForQuery(person);
            if (!tab) {
                tab = new IRCQueryTab(person, m_parentTab, m_mainWindow, (QWidget *)parent()); 
                m_parentTab->addQueryTab(tab);
                m_mainWindow->addTab(tab);
            }
        }
    }
}

void IRCChannelTab::popupPing() {
}

void IRCChannelTab::popupVersion() {
}

void IRCChannelTab::popupWhois() {
}

QString IRCChannelTab::title() {
    return m_channel->channelname();
}

IRCSession *IRCChannelTab::session() {
    return m_parentTab->session();
}

void IRCChannelTab::remove() {
    if (session()->isSessionActive()) {
        session()->part(m_channel);
    } else {
        m_mainWindow->killTab(this);
    }
}

IRCChannel *IRCChannelTab::channel() {
    return m_channel;
}

IRCChannelList *IRCChannelTab::list() {
    return m_list;
}
