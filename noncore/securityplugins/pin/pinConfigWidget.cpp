#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>

#include "pinConfigWidget.h"

PinConfigWidget::PinConfigWidget(QWidget* parent = 0, const char* name = "PIN configuration widget")
    : MultiauthConfigWidget(parent, name)
{
    QVBoxLayout * baseLayout = new QVBoxLayout( this);
    baseLayout->setSpacing(11);
    baseLayout->setMargin(11);
    baseLayout->setAlignment( Qt::AlignTop );
    
    QGroupBox * configBox = new QGroupBox(0, Qt::Vertical, tr("Configure your PIN here"), this);
    baseLayout->addWidget(configBox);
    QVBoxLayout *boxLayout = new QVBoxLayout( configBox->layout() );

    QLabel * description = new QLabel("<p>" + tr("PIN protection provides a minimal level of protection from casual access to this device.") + "</p>", configBox);
    boxLayout->addWidget(description);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(6);
    changePIN = new QPushButton( tr("Change PIN"), configBox, "changePIN" );
    clearPIN = new QPushButton( tr("Clear PIN"), configBox, "clearPIN" );
    buttonLayout->addWidget(changePIN);
    buttonLayout->addWidget(clearPIN);
    boxLayout->addLayout(buttonLayout);
    
}

/// nothing to do
PinConfigWidget::~PinConfigWidget()
{}

/// does nothing since the config file update is already done in changePIN or clearPIN
void PinConfigWidget::writeConfig()
{}
