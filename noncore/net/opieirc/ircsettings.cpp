#include <qlayout.h>
#include "ircsettings.h"
#include "irctab.h"
#include "ircmisc.h"
#include <stdio.h>

IRCSettings::IRCSettings(QWidget* parent, const char* name, bool modal, WFlags f) : QDialog(parent, name, modal, f) {
    m_config = new Config("OpieIRC");
    m_config->setGroup("OpieIRC");
    
    setCaption("Settings");
    QGridLayout *layout = new QGridLayout(this, 7, 2, 5, 0);
    QLabel *label = new QLabel(tr("Background color :"), this);
    layout->addWidget(label, 0, 0);
    m_background = new IRCFramedColorLabel(QColor(m_config->readEntry("BackgroundColor", "#FFFFFF")), this);
    layout->addWidget(m_background, 0, 1);
    label = new QLabel(tr("Normal text color :"), this);
    layout->addWidget(label, 1, 0);
    m_text = new IRCFramedColorLabel(m_config->readEntry("TextColor", "#000000"), this);
    layout->addWidget(m_text, 1, 1);
    label = new QLabel(tr("Error color :"), this);
    layout->addWidget(label, 2, 0);
    m_error = new IRCFramedColorLabel(m_config->readEntry("ErrorColor", "#FF0000"), this);
    layout->addWidget(m_error, 2, 1);
    label = new QLabel(tr("Text written by yourself :"), this);
    layout->addWidget(label, 3, 0);
    m_self = new IRCFramedColorLabel(m_config->readEntry("SelfColor", "#CC0000"), this);
    layout->addWidget(m_self, 3, 1);
    label = new QLabel(tr("Text written by others :"), this);
    layout->addWidget(label, 4, 0);
    m_other = new IRCFramedColorLabel(m_config->readEntry("OtherColor", "#0000BB"), this);
    layout->addWidget(m_other, 4, 1);
    label = new QLabel(tr("Text written by the server :"), this);
    layout->addWidget(label, 5, 0);
    m_server = new IRCFramedColorLabel(m_config->readEntry("ServerColor", "#0000FF"), this);
    layout->addWidget(m_server, 5, 1);
    label = new QLabel(tr("Notifications :"), this);
    layout->addWidget(label, 6, 0);
    m_notification = new IRCFramedColorLabel(m_config->readEntry("NotificationColor", "#AAE300"), this);
    layout->addWidget(m_notification, 6, 1);
    showMaximized();
}

QString IRCSettings::getColorString(QWidget *widget) {
    QColor color = ((IRCFramedColorLabel *)widget)->color();
    QString temp;
    temp.sprintf("#%02x%02x%02x", color.red(), color.green(), color.blue());
    return temp;
}

void IRCSettings::accept() {
    IRCTab::m_backgroundColor = getColorString(m_background);
    IRCTab::m_textColor = getColorString(m_text);
    IRCTab::m_errorColor = getColorString(m_error);
    IRCTab::m_selfColor = getColorString(m_self);
    IRCTab::m_otherColor = getColorString(m_other);
    IRCTab::m_serverColor = getColorString(m_server);
    IRCTab::m_notificationColor = getColorString(m_notification);
    m_config->writeEntry("BackgroundColor", getColorString(m_background));
    m_config->writeEntry("TextColor", getColorString(m_text));
    m_config->writeEntry("ErrorColor", getColorString(m_error));
    m_config->writeEntry("SelfColor", getColorString(m_self));
    m_config->writeEntry("OtherColor", getColorString(m_other));
    m_config->writeEntry("ServerColor", getColorString(m_server));
    m_config->writeEntry("NotificationColor", getColorString(m_notification));
    QDialog::accept();
}

IRCSettings::~IRCSettings() {
    delete m_config;
}
