/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "newpagedialog.h"

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qspinbox.h>

NewPageDialog::NewPageDialog(uint width, uint height, const QColor& foregroundColor,
                             const QColor& backgroundColor, QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("New Page"));

    m_foregroundColor = foregroundColor;
    m_backgroundColor = backgroundColor;

    QGroupBox* sizeGroupBox = new QGroupBox(0, Qt::Vertical, tr("Page Size"), this);

    QLabel* widthLabel = new QLabel(tr("Width :"), sizeGroupBox);
    QLabel* heightLabel = new QLabel(tr("Height :"), sizeGroupBox);

    m_pWidthSpinBox = new QSpinBox(1, 1024, 1, sizeGroupBox);
    m_pHeightSpinBox = new QSpinBox(1, 1024, 1, sizeGroupBox);

    m_pWidthSpinBox->setValue(width);
    m_pHeightSpinBox->setValue(height);

    m_pContentButtonGroup = new QButtonGroup(0, Qt::Vertical, tr("Contents"), this);

    QRadioButton* whiteColorRadioButton = new QRadioButton(tr("White"), m_pContentButtonGroup);
    QRadioButton* foregroundColorRadioButton = new QRadioButton(tr("Foreground Color"), m_pContentButtonGroup);
    QRadioButton* backgroundColorRadioButton = new QRadioButton(tr("Background Color"), m_pContentButtonGroup);

    m_pContentButtonGroup->setButton(0);

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    sizeGroupBox->layout()->setSpacing(4);
    m_pContentButtonGroup->layout()->setSpacing(4);
    QGridLayout* sizeLayout = new QGridLayout(sizeGroupBox->layout(), 2, 2);
    QVBoxLayout* contentLayout = new QVBoxLayout(m_pContentButtonGroup->layout());

    sizeLayout->addWidget(widthLabel, 0, 0);
    sizeLayout->addWidget(heightLabel, 1, 0);
    sizeLayout->addWidget(m_pWidthSpinBox, 0, 1);
    sizeLayout->addWidget(m_pHeightSpinBox, 1, 1);

    sizeLayout->setColStretch(1, 1);

    contentLayout->addWidget(whiteColorRadioButton);
    contentLayout->addWidget(foregroundColorRadioButton);
    contentLayout->addWidget(backgroundColorRadioButton);

    mainLayout->addWidget(sizeGroupBox);
    mainLayout->addWidget(m_pContentButtonGroup);
}

NewPageDialog::~NewPageDialog()
{
}

uint NewPageDialog::selectedWidth()
{
    return (m_pWidthSpinBox->value());
}

uint NewPageDialog::selectedHeight()
{
    return (m_pHeightSpinBox->value());
}

const QColor& NewPageDialog::selectedColor()
{
    switch (m_pContentButtonGroup->id(m_pContentButtonGroup->selected())) {
        case 0:
            return (Qt::white);
            break;
        case 1:
            return (m_foregroundColor);
            break;
        case 2:
            return (m_backgroundColor);
            break;
        default:
            return (Qt::white);
            break;
    }
}
