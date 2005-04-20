#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>

#include <qpe/config.h>

#include "stumblersettings.h"

StumblerSettings::StumblerSettings(QWidget* parent, const char* name, bool modal, WFlags f)
    : QDialog(parent, name, modal, WStyle_ContextHelp), m_config(new Config("OpieStumbler"))
{
    setCaption(tr("Settings"));
    m_config->setGroup("General");
    QBoxLayout *l = new QHBoxLayout(this);
    QLabel *lb = new QLabel(tr("Interface"), this);
    m_interface = new QLineEdit(this);
    m_interface->setText(m_config->readEntry("interface", "wlan0"));
    connect(m_interface, SIGNAL(returnPressed()), this, SLOT(accept()));
    l->addWidget(lb);
    l->addWidget(m_interface);
}

StumblerSettings::~StumblerSettings()
{
    delete m_config;
    delete m_interface;
}

void StumblerSettings::accept()
{
    m_config->writeEntry("interface", m_interface->text());
    m_config->write();
    
    QDialog::accept();
}
