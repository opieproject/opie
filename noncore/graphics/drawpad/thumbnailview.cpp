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

#include "thumbnailview.h"

#include "page.h"

#include <qpe/resource.h>

#include <qheader.h>
#include <qimage.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qtoolbutton.h>

ThumbnailView::ThumbnailView(QList<Page> pages, QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("Thumbnail"));

    QToolButton* newPageButton = new QToolButton(this);
    newPageButton->setIconSet(Resource::loadIconSet("new"));
    newPageButton->setAutoRaise(true);

    QToolButton* clearPageButton = new QToolButton(this);
    clearPageButton->setIconSet(Resource::loadIconSet("drawpad/clear"));
    clearPageButton->setAutoRaise(true);

    QToolButton* deletePageButton = new QToolButton(this);
    deletePageButton->setIconSet(Resource::loadIconSet("trash"));
    deletePageButton->setAutoRaise(true);

    QToolButton* movePageUpButton = new QToolButton(this);
    movePageUpButton->setIconSet(Resource::loadIconSet("up"));
    movePageUpButton->setAutoRaise(true);

    QToolButton* movePageDownButton = new QToolButton(this);
    movePageDownButton->setIconSet(Resource::loadIconSet("down"));
    movePageDownButton->setAutoRaise(true);

    m_pListView = new QListView(this);

    m_pListView->header()->hide();
    m_pListView->setAllColumnsShowFocus(true);

    m_pListView->addColumn(tr("Thumbnail"));
    m_pListView->addColumn(tr("Information"));

    m_pListView->setColumnAlignment(0, Qt::AlignHCenter | Qt::AlignVCenter);
    m_pListView->setColumnAlignment(1, Qt::AlignTop);

    QListIterator<Page> iterator(pages);

    for (; iterator.current(); ++iterator) {
        QImage image = iterator.current()->convertToImage();

        int previewWidth = 64;
        int previewHeight = 64;

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

        QPixmap pixmap(64, 64);

        pixmap.fill(colorGroup().mid());
        bitBlt(&pixmap, (pixmap.width() - previewPixmap.width()) / 2, 
               (pixmap.height() - previewPixmap.height()) / 2, &previewPixmap);

        QListViewItem* item = new QListViewItem(m_pListView);
        item->setPixmap(0, pixmap);
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    QHBoxLayout* buttonLayout = new QHBoxLayout(0);

    buttonLayout->addWidget(newPageButton);
    buttonLayout->addWidget(clearPageButton);
    buttonLayout->addWidget(deletePageButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(movePageUpButton);
    buttonLayout->addWidget(movePageDownButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_pListView);
}

ThumbnailView::~ThumbnailView()
{
}

void ThumbnailView::resizeEvent(QResizeEvent* e)
{
    QDialog::resizeEvent(e);

    m_pListView->setColumnWidth(1, m_pListView->contentsRect().width() - m_pListView->columnWidth(0)
                                   - m_pListView->verticalScrollBar()->width());
}
