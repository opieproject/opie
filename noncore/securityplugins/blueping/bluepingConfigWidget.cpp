#include "bluepingConfigWidget.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qmessagebox.h>
#include <qregexp.h>

using Opie::Security::MultiauthConfigWidget;

BluepingConfigWidget::BluepingConfigWidget(QWidget* parent = 0, const char* name = "Blueping configuration widget")
: MultiauthConfigWidget(parent, name)
{
    m_config = new Config("Security");
    m_config->setGroup("BluepingPlugin");
    QVBoxLayout * baseLayout = new QVBoxLayout( this);
    baseLayout->setSpacing(11);
    baseLayout->setMargin(11);
    baseLayout->setAlignment( Qt::AlignTop );

    QGroupBox * configBox = new QGroupBox(0, Qt::Vertical, tr("Set the MAC address to ping here"), this);
    baseLayout->addWidget(configBox);
    QVBoxLayout *boxLayout = new QVBoxLayout( configBox->layout() );

    QHBoxLayout * configLayout = new QHBoxLayout();
    configLayout->setSpacing(6);
    boxLayout->addLayout(configLayout);

    QString mac = m_config->readEntry("mac");
    if ( mac.isEmpty() )
        mac = "00:00:00:00:00:00";

    editMAC = new QLineEdit( mac, configBox, "editMAC" );
    setMAC = new QPushButton( tr("Set"), configBox, "setMAC" );
    configLayout->addWidget(editMAC);
    configLayout->addWidget(setMAC);

    QLabel * description = new QLabel( "<p>" + tr("Detecting another Bluetooth device by its MAC address provides a minimal and automatic level of protection.") + "</p><p><em>"
                                       + tr("Note: if you don't put a valid MAC here but you activate the plugin, it will always succeed!") + "</em></p>", configBox );
    boxLayout->addWidget(description);


    connect(setMAC, SIGNAL( clicked() ), this, SLOT( changeMAC() ));

}

/// checks and writes the MAC in the config file, if its format is OK
void BluepingConfigWidget::changeMAC() {
    QString mac = editMAC->text();
    QRegExp macPattern("[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]");
    if ( (mac.length() == 17) && (macPattern.match(mac) == 0) )
    {
        m_config->writeEntry("mac", mac);
        QMessageBox success( tr("MAC address saved!"), "<p>" + tr("Make sure that Bluetooth is turned on on the corresponding device when the Blueping plugin needs it.") + "</p>",
                             QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
        success.exec();
    } else {
        QMessageBox failure( tr("Please enter a valid MAC"), "<p>" + tr("Please separate the six pairs of digits of your MAC like this: 01:02:03:04:05:06") + tr("</p>"),
                             QMessageBox::Warning, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
        failure.exec();
    }
}

/// deletes the m_config pointer
BluepingConfigWidget::~BluepingConfigWidget()
{
    delete m_config;
}

// does nothing (there's a button to save the config)
void BluepingConfigWidget::writeConfig()
{
}
