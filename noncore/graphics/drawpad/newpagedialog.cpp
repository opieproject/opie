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

#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>

NewPageDialog::NewPageDialog(QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("New Page"));

    QLabel* widthLabel = new QLabel(tr("Width :"), this);
    QLabel* heightLabel = new QLabel(tr("Height :"), this);

    m_pWidthSpinBox = new QSpinBox(1, 1024, 1, this);
    m_pHeightSpinBox = new QSpinBox(1, 1024, 1, this);

    QGridLayout* gridLayout = new QGridLayout(this, 2, 2, 2, 2);

    gridLayout->addWidget(widthLabel, 0, 0);
    gridLayout->addWidget(heightLabel, 1, 0);
    gridLayout->addWidget(m_pWidthSpinBox, 0, 1);
    gridLayout->addWidget(m_pHeightSpinBox, 1, 1);
}

NewPageDialog::~NewPageDialog()
{
}

void NewPageDialog::setWidth(int width)
{
    m_pWidthSpinBox->setValue(width);
}

void NewPageDialog::setHeight(int height)
{
    m_pHeightSpinBox->setValue(height);
}

int NewPageDialog::width()
{
    return m_pWidthSpinBox->value();
}

int NewPageDialog::height()
{
    return m_pHeightSpinBox->value();
}
