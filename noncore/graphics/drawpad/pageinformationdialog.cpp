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

#include "pageinformationdialog.h"

#include "page.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>

PageInformationDialog::PageInformationDialog(Page* page, QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    m_pPage = page;

    setCaption(tr("Page Information"));

    QGroupBox* generalGroupBox = new QGroupBox(0, Qt::Vertical, tr("General"), this);

    QLabel* titleLabel = new QLabel(tr("Title:"), generalGroupBox);
    m_pTitleLineEdit = new QLineEdit(generalGroupBox);

    QGroupBox* sizeGroupBox = new QGroupBox(0, Qt::Vertical, tr("Size"), this);

    QLabel* widthLabel = new QLabel(tr("Width:"), sizeGroupBox);
    QLabel* widthValueLabel = new QLabel(QString::number(m_pPage->width()), sizeGroupBox);

    QLabel* heightLabel = new QLabel(tr("Height:"), sizeGroupBox);
    QLabel* heightValueLabel = new QLabel(QString::number(m_pPage->height()), sizeGroupBox);

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    generalGroupBox->layout()->setSpacing(4);
    sizeGroupBox->layout()->setSpacing(4);
    QGridLayout* generalLayout = new QGridLayout(generalGroupBox->layout(), 1, 1);
    QGridLayout* sizeLayout = new QGridLayout(sizeGroupBox->layout(), 2, 2);

    generalLayout->addWidget(titleLabel, 0, 0);
    generalLayout->addWidget(m_pTitleLineEdit, 0, 1);

    sizeLayout->addWidget(widthLabel, 0, 0);
    sizeLayout->addWidget(widthValueLabel, 0, 1);
    sizeLayout->addWidget(heightLabel, 1, 0);
    sizeLayout->addWidget(heightValueLabel, 1, 1);

    sizeLayout->setColStretch(1, 1);

    mainLayout->addWidget(generalGroupBox);
    mainLayout->addWidget(sizeGroupBox);
}

PageInformationDialog::~PageInformationDialog()
{
}

QString PageInformationDialog::selectedTitle()
{
    return (m_pTitleLineEdit->text());
}
