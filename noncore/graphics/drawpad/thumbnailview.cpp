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

#include "drawpadcanvas.h"
#include "page.h"

#include <qpe/resource.h>

#include <qapplication.h>
#include <qheader.h>
#include <qimage.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>

PageListViewItem::PageListViewItem(Page* page, QListView* parent)
    : QListViewItem(parent)
{
    m_pPage = page;

    QImage image = m_pPage->convertToImage();

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

    pixmap.fill(listView()->colorGroup().mid());
    bitBlt(&pixmap, (pixmap.width() - previewPixmap.width()) / 2,
           (pixmap.height() - previewPixmap.height()) / 2, &previewPixmap);

    setPixmap(0, pixmap);
}

PageListViewItem::~PageListViewItem()
{
}

Page* PageListViewItem::page() const
{
    return m_pPage;
}

PageListView::PageListView(DrawPadCanvas* drawPadCanvas, QWidget* parent, const char* name)
    : QListView(parent, name)
{
    m_pDrawPadCanvas = drawPadCanvas;

    header()->hide();
    setVScrollBarMode(QScrollView::AlwaysOn);
    setAllColumnsShowFocus(true);

    addColumn(tr("Thumbnail"));
    addColumn(tr("Information"));

    updateView();
}

PageListView::~PageListView()
{
}

void PageListView::updateView()
{
    clear();

    if (m_pDrawPadCanvas) {
        QList<Page> pageList = m_pDrawPadCanvas->pages();
        QListIterator<Page> it(pageList);

        for (; it.current(); ++it) {
            new PageListViewItem(it.current(), this);
        }
    }
}

void PageListView::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);

    setColumnWidth(1, contentsRect().width() - columnWidth(0) - verticalScrollBar()->width());
}

Page* PageListView::selected() const
{
    Page* page;

    PageListViewItem* item = (PageListViewItem*)selectedItem();

    if (item) {
        page = item->page();
    } else {
        page = NULL;
    }

    return page;
}


ThumbnailView::ThumbnailView(DrawPadCanvas* drawPadCanvas, QWidget* parent, const char* name)
    : QWidget(parent, name, Qt::WType_Modal | Qt::WType_TopLevel)
{
    inLoop = false;
    m_pDrawPadCanvas = drawPadCanvas;

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

    m_pPageListView = new PageListView(m_pDrawPadCanvas, this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    QHBoxLayout* buttonLayout = new QHBoxLayout(0);

    buttonLayout->addWidget(newPageButton);
    buttonLayout->addWidget(clearPageButton);
    buttonLayout->addWidget(deletePageButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(movePageUpButton);
    buttonLayout->addWidget(movePageDownButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_pPageListView);
}

ThumbnailView::~ThumbnailView()
{
    hide();
}

void ThumbnailView::hide()
{
    QWidget::hide();

    if (inLoop) {
        inLoop = false;
        qApp->exit_loop();
    }
}

void ThumbnailView::exec()
{
    show();

    if (!inLoop) {
        inLoop = true;
        qApp->enter_loop();
    }
}

void ThumbnailView::deletePage()
{
    QMessageBox messageBox(tr("Delete Page"), tr("Do you want to delete\nthe selected page?"),
                           QMessageBox::Information, QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
                           QMessageBox::NoButton, this);

    messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    messageBox.setButtonText(QMessageBox::No, tr("No"));

    if (messageBox.exec() == QMessageBox::Yes) {
        m_pDrawPadCanvas->deletePage();
    }
}
