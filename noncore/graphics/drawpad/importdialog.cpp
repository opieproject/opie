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

#include "importdialog.h"

#include <qpe/applnk.h>
#include <qpe/fileselector.h>

#include <qlayout.h>

ImportDialog::ImportDialog(QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("Import"));

    m_pFileSelector = new FileSelector("image/*", this, "fileselector");
    m_pFileSelector->setNewVisible(false);
    m_pFileSelector->setCloseVisible(false);

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    mainLayout->addWidget(m_pFileSelector);
}

ImportDialog::~ImportDialog()
{
}

const DocLnk* ImportDialog::selected()
{
    return m_pFileSelector->selected();
}
