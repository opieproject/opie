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
    connect(m_listButton, SIGNAL(clicked()), this, SLOT(toggleList()));
    m_list = new IRCChannelList(m_channel, hbox);
    m_list->update();
    m_list->setMaximumWidth(LISTWIDTH);
    m_field = new QLineEdit(this);
    m_layout->add(hbox);
    hbox->show();
    m_layout->add(m_field);
    m_field->setFocus();
    connect(m_field, SIGNAL(returnPressed()), this, SLOT(processCommand()));

}

void IRCChannelTab::appendText(QString text) {
    /* not using append because it creates layout problems */
    m_textview->setText(m_textview->text() + text);
    m_textview->ensureVisible(0, m_textview->contentsHeight());
}

IRCChannelTab::~IRCChannelTab() {
    m_parentTab->removeChannelTab(this);
}

void IRCChannelTab::processCommand() {
    if (m_field->text().length()>0) {
        session()->sendMessage(m_channel, m_field->text());
        appendText("&lt;<font color=\"#dd0000\">"+m_parentTab->server()->nick()+"</font>&gt; "+m_field->text()+"<br>");
        m_field->clear();
    }
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

QString IRCChannelTab::title() {
    return m_channel->channelname();
}

IRCSession *IRCChannelTab::session() {
    return m_parentTab->session();
}

void IRCChannelTab::remove() {
    session()->part(m_channel);
}

IRCChannel *IRCChannelTab::channel() {
    return m_channel;
}

IRCChannelList *IRCChannelTab::list() {
    return m_list;
}
