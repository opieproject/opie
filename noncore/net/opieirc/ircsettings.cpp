#include <opie/ocolorbutton.h>
#include <opie/otabwidget.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qscrollview.h>
#include <qwhatsthis.h>
#include "ircsettings.h"
#include "irctab.h"
#include "ircmisc.h"

IRCSettings::IRCSettings(QWidget* parent, const char* name, bool modal, WFlags) : QDialog(parent, name, modal, WStyle_ContextHelp) {
    setCaption("Settings");
    m_config = new Config("OpieIRC");
    m_config->setGroup("OpieIRC");
    QHBoxLayout *l = new QHBoxLayout(this, 2, 2);
    OTabWidget *tw = new OTabWidget(this);
    l->addWidget(tw);
    /* General Configuration */
    QWidget *genwidget = new QWidget(tw);
    QGridLayout *layout = new QGridLayout(genwidget, 1, 2, 5, 0);
    QLabel *label = new QLabel(tr("Lines displayed :"), genwidget);
    layout->addWidget(label, 0, 0);
    m_lines = new QLineEdit(m_config->readEntry("Lines", "100"), genwidget);
    QWhatsThis::add(m_lines, tr("Amount of lines to be displayed in chats before old lines get deleted - this is necessary to restrain memory consumption. Set to 0 if you don't need this"));
    QIntValidator *validator = new QIntValidator(this);
    validator->setTop(10000);
    validator->setBottom(0);
    m_lines->setValidator(validator);
    layout->addWidget(m_lines, 0, 1);
    tw->addTab(genwidget, "opieirc/settings", tr("General"));

    /* Color configuration */
    QScrollView *view = new QScrollView(this);
    view->setResizePolicy(QScrollView::AutoOneFit);
    view->setFrameStyle( QFrame::NoFrame );
    QWidget *widget = new QWidget(view->viewport());
    view->addChild(widget);
    layout = new QGridLayout(widget, 7, 2, 5, 0);
    label = new QLabel(tr("Background color :"), widget);
    layout->addWidget(label, 0, 0);
    m_background = new OColorButton(widget, m_config->readEntry("BackgroundColor", "#FFFFFF"));
    QWhatsThis::add(m_background, tr("Background color to be used in chats"));
    layout->addWidget(m_background, 0, 1);
    label = new QLabel(tr("Normal text color :"), widget);
    layout->addWidget(label, 1, 0);
    m_text = new OColorButton(widget, m_config->readEntry("TextColor", "#000000"));
    QWhatsThis::add(m_text, tr("Text color to be used in chats"));
    layout->addWidget(m_text, 1, 1);
    label = new QLabel(tr("Error color :"), widget);
    layout->addWidget(label, 2, 0);
    m_error = new OColorButton(widget, m_config->readEntry("ErrorColor", "#FF0000"));
    QWhatsThis::add(m_error, tr("Text color to be used to display errors"));
    layout->addWidget(m_error, 2, 1);
    label = new QLabel(tr("Text written by yourself :"), widget);
    layout->addWidget(label, 3, 0);
    m_self = new OColorButton(widget, m_config->readEntry("SelfColor", "#CC0000"));
    QWhatsThis::add(m_self, tr("Text color to be used to identify text written by yourself"));
    layout->addWidget(m_self, 3, 1);
    label = new QLabel(tr("Text written by others :"), widget);
    layout->addWidget(label, 4, 0);
    m_other = new OColorButton(widget, m_config->readEntry("OtherColor", "#0000BB"));
    QWhatsThis::add(m_other, tr("Text color to be used to identify text written by others"));
    layout->addWidget(m_other, 4, 1);
    label = new QLabel(tr("Text written by the server :"), widget);
    layout->addWidget(label, 5, 0);
    m_server = new OColorButton(widget, m_config->readEntry("ServerColor", "#0000FF"));
    QWhatsThis::add(m_server, tr("Text color to be used to identify text written by the server"));
    layout->addWidget(m_server, 5, 1);
    label = new QLabel(tr("Notifications :"), widget);
    layout->addWidget(label, 6, 0);
    m_notification = new OColorButton(widget, m_config->readEntry("NotificationColor", "#AAE300"));
    QWhatsThis::add(m_notification, tr("Text color to be used to display notifications"));
    layout->addWidget(m_notification, 6, 1);
    tw->addTab(view, "opieirc/colors", tr("Colors"));
    tw->setCurrentTab( genwidget );
    showMaximized();
}

void IRCSettings::accept() {
    IRCTab::m_backgroundColor = m_background->color().name();
    IRCTab::m_textColor = m_text->color().name();
    IRCTab::m_errorColor = m_error->color().name();
    IRCTab::m_selfColor = m_self->color().name();
    IRCTab::m_otherColor = m_other->color().name();
    IRCTab::m_serverColor = m_server->color().name();
    IRCTab::m_notificationColor = m_notification->color().name();
    IRCTab::m_maxLines = m_lines->text().toInt();
    m_config->writeEntry("BackgroundColor", IRCTab::m_backgroundColor);
    m_config->writeEntry("TextColor", IRCTab::m_textColor);
    m_config->writeEntry("ErrorColor", IRCTab::m_errorColor);
    m_config->writeEntry("SelfColor", IRCTab::m_selfColor);
    m_config->writeEntry("OtherColor", IRCTab::m_otherColor);
    m_config->writeEntry("ServerColor", IRCTab::m_serverColor);
    m_config->writeEntry("NotificationColor", IRCTab::m_notificationColor);
    m_config->writeEntry("Lines", m_lines->text());
    QDialog::accept();
}

IRCSettings::~IRCSettings() {
    delete m_config;
}
