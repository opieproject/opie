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
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>

NewPageDialog::NewPageDialog(uint width, uint height, const QColor& penColor,
                             const QColor& brushColor, QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("New Page"));

    m_penColor = penColor;
    m_brushColor = brushColor;

    QGroupBox* generalGroupBox = new QGroupBox(0, Qt::Vertical, tr("General"), this);

    QLabel* titleLabel = new QLabel(tr("Title:"), generalGroupBox);
    m_pTitleLineEdit = new QLineEdit(generalGroupBox);

    QGroupBox* sizeGroupBox = new QGroupBox(0, Qt::Vertical, tr("Size"), this);

    QLabel* widthLabel = new QLabel(tr("Width:"), sizeGroupBox);
    QLabel* heightLabel = new QLabel(tr("Height:"), sizeGroupBox);

    m_pWidthSpinBox = new QSpinBox(1, 1024, 1, sizeGroupBox);
    m_pHeightSpinBox = new QSpinBox(1, 1024, 1, sizeGroupBox);

    m_pWidthSpinBox->setValue(width);
    m_pHeightSpinBox->setValue(height);

    m_pContentButtonGroup = new QButtonGroup(0, Qt::Vertical, tr("Background"), this);

    QRadioButton* whiteColorRadioButton = new QRadioButton(tr("White"), m_pContentButtonGroup);
    QRadioButton* penColorRadioButton = new QRadioButton(tr("Pen Color"), m_pContentButtonGroup);
    QRadioButton* brushColorRadioButton = new QRadioButton(tr("Fill Color"), m_pContentButtonGroup);

    m_pContentButtonGroup->setButton(0);

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    generalGroupBox->layout()->setSpacing(4);
    sizeGroupBox->layout()->setSpacing(4);
    m_pContentButtonGroup->layout()->setSpacing(4);
    QGridLayout* generalLayout = new QGridLayout(generalGroupBox->layout(), 1, 1);
    QGridLayout* sizeLayout = new QGridLayout(sizeGroupBox->layout(), 2, 2);
    QVBoxLayout* contentLayout = new QVBoxLayout(m_pContentButtonGroup->layout());

    generalLayout->addWidget(titleLabel, 0, 0);
    generalLayout->addWidget(m_pTitleLineEdit, 0, 1);

    sizeLayout->addWidget(widthLabel, 0, 0);
    sizeLayout->addWidget(heightLabel, 1, 0);
    sizeLayout->addWidget(m_pWidthSpinBox, 0, 1);
    sizeLayout->addWidget(m_pHeightSpinBox, 1, 1);

    sizeLayout->setColStretch(1, 1);

    contentLayout->addWidget(whiteColorRadioButton);
    contentLayout->addWidget(penColorRadioButton);
    contentLayout->addWidget(brushColorRadioButton);

    mainLayout->addWidget(generalGroupBox);
    mainLayout->addWidget(sizeGroupBox);
    mainLayout->addWidget(m_pContentButtonGroup);
}

NewPageDialog::~NewPageDialog()
{
}

QString NewPageDialog::selectedTitle()
{
    return (m_pTitleLineEdit->text());
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
            return (m_penColor);
            break;
        case 2:
            return (m_brushColor);
            break;
        default:
            return (Qt::white);
            break;
    }
}
