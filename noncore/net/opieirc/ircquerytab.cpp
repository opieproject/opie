#include <qhbox.h>
#include "ircquerytab.h"
#include "ircservertab.h"

IRCQueryTab::IRCQueryTab(IRCPerson *person, IRCServerTab *parentTab, MainWindow *mainWindow, QWidget *parent, const char *name, WFlags f) : IRCTab(parent, name, f) {
    m_mainWindow = mainWindow;
    m_parentTab = parentTab;
    m_lines = 0;
    m_person = new IRCPerson(*person); /* We need this (the person might sign off and the original IRCPerson gets deleted) */
    m_description->setText(tr("Talking to ") + " <b>" + person->nick() + "</b>");
    QHBox *hbox = new QHBox(this);
    m_textview = new QTextView(hbox);
    m_textview->setHScrollBarMode(QScrollView::AlwaysOff);
    m_textview->setVScrollBarMode(QScrollView::AlwaysOn);
    m_textview->setTextFormat(RichText);
    m_field = new IRCHistoryLineEdit(this);
    m_layout->add(hbox);
    hbox->show();
    m_layout->add(m_field);
    m_field->setFocus();
    connect(m_field, SIGNAL(returnPressed()), this, SLOT(processCommand()));
    settingsChanged();
}

void IRCQueryTab::appendText(QString text) {
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

IRCQueryTab::~IRCQueryTab() {
    m_parentTab->removeQueryTab(this);
    delete m_person;
}

void IRCQueryTab::processCommand() {
    QString text = m_field->text();
    if (text.length()>0) {
        if (session()->isSessionActive()) {
            if (text.startsWith("/") && !text.startsWith("//")) {
                /* Command mode */
                m_parentTab->executeCommand(this, text);;
            } else {
                if (text.startsWith("//"))
                    text = text.right(text.length()-1);
                session()->sendMessage(m_person, m_field->text());
                appendText("<font color=\"" + m_textColor + "\">&lt;</font><font color=\"" + m_selfColor + "\">"+m_parentTab->server()->nick()+"</font><font color=\"" + m_textColor + "\">&gt; "+IRCOutput::toHTML(m_field->text())+"</font><br>");
            }
        } else {
            appendText("<font color=\"" + m_errorColor + "\">"+tr("Disconnected")+"</font><br>");
        }
    }
    m_field->clear();
}

void IRCQueryTab::display(IRCOutput output) {
    if (output.type() == OUTPUT_QUERYPRIVMSG) {
        appendText("<font color=\"" + m_textColor + "\">&lt;</font><font color=\"" + m_otherColor + "\">"+m_person->nick()+"</font><font color=\"" + m_textColor + "\">&gt; " + output.htmlMessage() + "</font><br>");
    } else if (output.type() == OUTPUT_QUERYACTION) {
        appendText("<font color=\"" + m_otherColor + "\">" + output.htmlMessage() + "<br>");
    }
}

void IRCQueryTab::settingsChanged() {
    m_textview->setText("<qt bgcolor=\"" + m_backgroundColor + "\"/>");
    m_lines = 0;
}

QString IRCQueryTab::title() {
    return m_person->nick();
}

IRCSession *IRCQueryTab::session() {
    return m_parentTab->session();
}

void IRCQueryTab::remove() {
    m_mainWindow->killTab(this);
}

IRCPerson *IRCQueryTab::person() {
    return m_person;
}

