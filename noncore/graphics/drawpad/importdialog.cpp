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

#include <qcheckbox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

ImportDialog::ImportDialog(QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("Import"));

    m_pFileSelector = new FileSelector("image/*", this, "fileselector");
    connect(m_pFileSelector, SIGNAL(fileSelected(const DocLnk&)), this, SLOT(fileChanged()));
    m_pFileSelector->setNewVisible(false);
    m_pFileSelector->setCloseVisible(false);

    m_pPreviewLabel = new QLabel(this);
    m_pPreviewLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_pPreviewLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pPreviewLabel->setFixedSize(114, 114);
    m_pPreviewLabel->setBackgroundMode(QWidget::PaletteMid);

    m_pAutomaticPreviewCheckBox = new QCheckBox(tr("Automatic preview"), this);
    m_pAutomaticPreviewCheckBox->setChecked(true);

    QPushButton* previewPushButton = new QPushButton(tr("Preview"), this);
    connect(previewPushButton, SIGNAL(clicked()), this, SLOT(preview()));

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    QHBoxLayout* previewLayout = new QHBoxLayout(4);
    QVBoxLayout* previewSecondLayout = new QVBoxLayout(4);

    previewSecondLayout->addWidget(m_pAutomaticPreviewCheckBox);
    previewSecondLayout->addWidget(previewPushButton);
    previewSecondLayout->addStretch();

    previewLayout->addWidget(m_pPreviewLabel);
    previewLayout->addLayout(previewSecondLayout);

    mainLayout->addWidget(m_pFileSelector);
    mainLayout->addLayout(previewLayout);

    preview();
}

ImportDialog::~ImportDialog()
{
}

const DocLnk* ImportDialog::selected()
{
    return m_pFileSelector->selected();
}

void ImportDialog::fileChanged()
{
    if (m_pAutomaticPreviewCheckBox->isChecked()) {
        preview();
    }
}

void ImportDialog::preview()
{
    const DocLnk* docLnk = m_pFileSelector->selected();

    if (docLnk) {
        QImage image(docLnk->file());

        int previewWidth = m_pPreviewLabel->contentsRect().width();
        int previewHeight = m_pPreviewLabel->contentsRect().height();

        float widthScale = 1.0;
        float heightScale = 1.0;

        if (previewWidth < image.width()) {
            widthScale = (float)previewWidth / float(image.width());
        }

        if (previewHeight < image.height()) {
            heightScale = (float)previewHeight / float(image.height());
        }

        float scale = (widthScale < heightScale ? widthScale : heightScale);
        QImage previewImage = image.smoothScale((int)(image.width() * scale) , (int)(image.height() * scale));

        QPixmap previewPixmap;
        previewPixmap.convertFromImage(previewImage);

        m_pPreviewLabel->setPixmap(previewPixmap);

        delete docLnk;
    }
}
