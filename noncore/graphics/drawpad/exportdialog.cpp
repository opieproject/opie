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

#include "exportdialog.h"

#include <qpe/fileselector.h>

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include <stdlib.h>

ExportDialog::ExportDialog(uint pageAt, uint pageCount, QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("Export"));

    m_pageAt = pageAt;
    m_pageCount = pageCount;

    QButtonGroup* selectionButtonGroup = new QButtonGroup(0, Qt::Vertical, tr("Page Selection"), this);
    connect(selectionButtonGroup, SIGNAL(pressed(int)), this, SLOT(selectionChanged(int)));

    QRadioButton* selectAllRadioButton = new QRadioButton(tr("All"), selectionButtonGroup);
    QRadioButton* selectCurrentRadioButton = new QRadioButton(tr("Current"), selectionButtonGroup);
    QRadioButton* selectRangeRadioButton = new QRadioButton(tr("Range"), selectionButtonGroup);

    QLabel* toLabel = new QLabel(tr("To:"), selectionButtonGroup);

    m_pFromPageSpinBox = new QSpinBox(1, m_pageCount, 1, selectionButtonGroup);
    connect(m_pFromPageSpinBox, SIGNAL(valueChanged(int)), this, SLOT(fromPageChanged(int)));

    m_pToPageSpinBox = new QSpinBox(1, m_pageCount, 1, selectionButtonGroup);
    connect(m_pToPageSpinBox, SIGNAL(valueChanged(int)), this, SLOT(toPageChanged(int)));

    selectionButtonGroup->setButton(1);
    selectionChanged(1);

    m_pFromPageSpinBox->setValue(pageAt);
    m_pToPageSpinBox->setValue(pageAt);

    QGroupBox* exportGroupBox = new QGroupBox(0, Qt::Vertical, tr("Export As"), this);

    QLabel* nameLabel = new QLabel(tr("Name:"), exportGroupBox);
    QLabel* formatLabel = new QLabel(tr("Format:"), exportGroupBox);

    m_pNameLineEdit = new QLineEdit(exportGroupBox);

    m_pFormatComboBox = new QComboBox(exportGroupBox);
    m_pFormatComboBox->insertStrList(QImageIO::outputFormats());

    FileSelector* fileSelector = new FileSelector("image/*", this, "fileselector");
    fileSelector->setNewVisible(false);
    fileSelector->setCloseVisible(false);

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    selectionButtonGroup->layout()->setSpacing(4);
    exportGroupBox->layout()->setSpacing(4);
    QGridLayout* selectionLayout = new QGridLayout(selectionButtonGroup->layout(), 2, 2);
    QHBoxLayout* rangeLayout = new QHBoxLayout();
    QGridLayout* exportLayout = new QGridLayout(exportGroupBox->layout(), 2, 2);

    selectionLayout->addWidget(selectAllRadioButton, 0, 0);
    selectionLayout->addWidget(selectCurrentRadioButton, 1, 0);
    selectionLayout->addWidget(selectRangeRadioButton, 0, 1);
    selectionLayout->addLayout(rangeLayout, 1, 1);

    rangeLayout->addWidget(m_pFromPageSpinBox);
    rangeLayout->addWidget(toLabel);
    rangeLayout->addWidget(m_pToPageSpinBox);

    exportLayout->addWidget(nameLabel, 0, 0);
    exportLayout->addWidget(formatLabel, 1, 0);

    exportLayout->addWidget(m_pNameLineEdit, 0, 1);
    exportLayout->addWidget(m_pFormatComboBox, 1, 1);

    exportLayout->setColStretch(1, 1);

    mainLayout->addWidget(selectionButtonGroup);
    mainLayout->addWidget(exportGroupBox);
    mainLayout->addWidget(fileSelector);

    m_pNameLineEdit->setFocus();
}

ExportDialog::~ExportDialog()
{
}

uint ExportDialog::selectedFromPage()
{
    return (m_pFromPageSpinBox->value());
}

uint ExportDialog::selectedToPage()
{
    return (m_pToPageSpinBox->value());
}

QString ExportDialog::selectedName()
{
    return (m_pNameLineEdit->text());
}

QString ExportDialog::selectedFormat()
{
    return (m_pFormatComboBox->currentText());
}

void ExportDialog::accept()
{
    if (!(m_pNameLineEdit->text().isEmpty())) {
        QDialog::accept();
    }
}

void ExportDialog::selectionChanged(int id)
{
    switch (id) {
        case 0:
            m_pFromPageSpinBox->setValue(1);
            m_pToPageSpinBox->setValue(m_pageCount);

            m_pFromPageSpinBox->setEnabled(false);
            m_pToPageSpinBox->setEnabled(false);
            break;
        case 1:
            m_pFromPageSpinBox->setValue(m_pageAt);
            m_pToPageSpinBox->setValue(m_pageAt);

            m_pFromPageSpinBox->setEnabled(false);
            m_pToPageSpinBox->setEnabled(false);
            break;
        case 2:
            m_pFromPageSpinBox->setEnabled(true);
            m_pToPageSpinBox->setEnabled(true);
            break;
        default:
            break;
    }
}

void ExportDialog::fromPageChanged(int value)
{
    if (m_pToPageSpinBox->value() < value) {
        m_pToPageSpinBox->setValue(value);
    }
}

void ExportDialog::toPageChanged(int value)
{
    if (m_pFromPageSpinBox->value() > value) {
        m_pFromPageSpinBox->setValue(value);
    }
}
