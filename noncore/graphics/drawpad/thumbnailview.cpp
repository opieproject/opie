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

#include "drawpad.h"
#include "drawpadcanvas.h"
#include "newpagedialog.h"
#include "page.h"

#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/timestring.h>

#include <qapplication.h>
#include <qimage.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>

#define THUMBNAIL_SIZE 48

PageListBoxItem::PageListBoxItem(Page* page, QListBox* parent)
    : QListBoxItem(parent)
{
    m_pPage = page;

    QImage image = m_pPage->pixmap()->convertToImage();

    int previewWidth = THUMBNAIL_SIZE;
    int previewHeight = THUMBNAIL_SIZE;

    float widthScale = 1.0;
    float heightScale = 1.0;

    if (previewWidth < image.width()) {
        widthScale = (float)previewWidth / float(image.width());
    }

    if (previewHeight < image.height()) {
        heightScale = (float)previewHeight / float(image.height());
    }

    float scale = (widthScale < heightScale ? widthScale : heightScale);
    QImage thumbnailImage = image.smoothScale((int)(image.width() * scale) , (int)(image.height() * scale));

    m_thumbnail.convertFromImage(thumbnailImage);

    m_titleText = QObject::tr("Title:") + " " + m_pPage->title();
    m_dimensionText = QObject::tr("Dimension:") + " " + QString::number(m_pPage->pixmap()->width())
                      + "x" + QString::number(m_pPage->pixmap()->height());
    m_dateText = QObject::tr("Date:") + " " + dateTimeString(m_pPage->lastModified());

    QColor baseColor = parent->colorGroup().base();
    int h, s, v;
    baseColor.hsv(&h, &s, &v);

    if (v > 128) {
        m_alternateColor = baseColor.dark(115);
    } else if (baseColor != Qt::black) {
        m_alternateColor = baseColor.light(115);
    } else {
        m_alternateColor = QColor(32, 32, 32);
    }
}

PageListBoxItem::~PageListBoxItem()
{
}

int PageListBoxItem::height(const QListBox*) const
{
    return QMAX(THUMBNAIL_SIZE + 4, QApplication::globalStrut().height());
}

int PageListBoxItem::width(const QListBox* lb) const
{
    QFontMetrics fontMetrics = lb->fontMetrics();
    int maxtextLength = QMAX(fontMetrics.width(m_titleText),
                             QMAX(fontMetrics.width(m_dimensionText),
                                  fontMetrics.width(m_dateText)));

    return QMAX(THUMBNAIL_SIZE + maxtextLength + 8, QApplication::globalStrut().width());
}

void PageListBoxItem::paint(QPainter *painter)
{
    QRect itemRect = listBox()->itemRect(this);

    if (!selected() && (listBox()->index(this) % 2)) {
        painter->fillRect(0, 0, itemRect.width(), itemRect.height(), m_alternateColor);
    }

    painter->drawPixmap(2 + (THUMBNAIL_SIZE - m_thumbnail.width()) / 2,
                        2 + (THUMBNAIL_SIZE - m_thumbnail.height()) / 2,
                        m_thumbnail);

    QFont standardFont = painter->font();
    QFont boldFont = painter->font();
    boldFont.setBold(TRUE);

    QFontMetrics fontMetrics = painter->fontMetrics();
    QRect textRect(THUMBNAIL_SIZE + 6, 2,
                   itemRect.width() - THUMBNAIL_SIZE - 8,
                   itemRect.height() - 4);

    painter->setFont(boldFont);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, m_titleText);

    painter->setFont(standardFont);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_dimensionText);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignBottom, m_dateText);

    if (!selected() && !(listBox()->hasFocus() && listBox()->item(listBox()->currentItem()) == this)) {
        painter->drawLine(0, itemRect.height() - 1, itemRect.width() - 1, itemRect.height() - 1);
    }
}

Page* PageListBoxItem::page() const
{
    return m_pPage;
}

QString PageListBoxItem::dateTimeString(QDateTime dateTime)
{
    QString result;

    Config config("qpe");
    config.setGroup("Date");

    QChar separator = config.readEntry("Separator", "/")[0];
    DateFormat::Order shortOrder = (DateFormat::Order)config .readNumEntry("ShortOrder", DateFormat::DayMonthYear);

    for (int i = 0; i < 3; i++) {
        switch((shortOrder >> (i * 3)) & 0x0007) {
            case 0x0001:
                result += QString().sprintf("%02d", dateTime.date().day());
                break;
            case 0x0002:
                result += QString().sprintf("%02d", dateTime.date().month());
                break;
            case 0x0004:
                result += QString().sprintf("%04d", dateTime.date().year());
                break;
            default:
                break;
        }

        if (i < 2) {
            result += separator;
        }
    }

    result += QString().sprintf(" %02d:%02d", dateTime.time().hour(), dateTime.time().minute());

    return result;
}

PageListBox::PageListBox(DrawPadCanvas* drawPadCanvas, QWidget* parent, const char* name)
    : QListBox(parent, name)
{
    m_pDrawPadCanvas = drawPadCanvas;

    setVScrollBarMode(QScrollView::AlwaysOn);

    updateView();
}

PageListBox::~PageListBox()
{
}

void PageListBox::updateView()
{
    clear();

    if (m_pDrawPadCanvas) {
        QList<Page> pageList = m_pDrawPadCanvas->pages();
        QListIterator<Page> it(pageList);

        for (; it.current(); ++it) {
            new PageListBoxItem(it.current(), this);
        }
        
        select(m_pDrawPadCanvas->currentPage());
    }
}

void PageListBox::select(Page* page)
{
    uint i = 0;
    uint itemCount = count();

    while (i < itemCount) {
        PageListBoxItem* currentItem = (PageListBoxItem*)item(i);

        if (currentItem->page() == page) {
            setCurrentItem(currentItem);
            break;
        }

        i++;
    }
}

Page* PageListBox::selected() const
{
    Page* page;

    PageListBoxItem* selectedItem = (PageListBoxItem*)item(currentItem());

    if (selectedItem) {
        page = selectedItem->page();
    } else {
        page = NULL;
    }

    return page;
}

ThumbnailView::ThumbnailView(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas, QWidget* parent, const char* name)
    : QWidget(parent, name, Qt::WType_Modal | Qt::WType_TopLevel)
{
    inLoop = false;

    m_pDrawPad = drawPad;
    m_pDrawPadCanvas = drawPadCanvas;

    setCaption(tr("DrawPad - Thumbnail View"));

    QToolButton* newPageButton = new QToolButton(this);
    newPageButton->setIconSet(Resource::loadIconSet("new"));
    newPageButton->setAutoRaise(true);
    connect(newPageButton, SIGNAL(clicked()), this, SLOT(newPage()));

    QToolButton* clearPageButton = new QToolButton(this);
    clearPageButton->setIconSet(Resource::loadIconSet("drawpad/clear"));
    clearPageButton->setAutoRaise(true);
    connect(clearPageButton, SIGNAL(clicked()), this, SLOT(clearPage()));

    QToolButton* deletePageButton = new QToolButton(this);
    deletePageButton->setIconSet(Resource::loadIconSet("trash"));
    deletePageButton->setAutoRaise(true);
    connect(deletePageButton, SIGNAL(clicked()), this, SLOT(deletePage()));

    m_pMovePageUpButton = new QToolButton(this);
    m_pMovePageUpButton->setIconSet(Resource::loadIconSet("up"));
    m_pMovePageUpButton->setAutoRaise(true);
    connect(m_pMovePageUpButton, SIGNAL(clicked()), this, SLOT(movePageUp()));

    m_pMovePageDownButton = new QToolButton(this);
    m_pMovePageDownButton->setIconSet(Resource::loadIconSet("down"));
    m_pMovePageDownButton->setAutoRaise(true);
    connect(m_pMovePageDownButton, SIGNAL(clicked()), this, SLOT(movePageDown()));

    m_pPageListBox = new PageListBox(m_pDrawPadCanvas, this);
    connect(m_pPageListBox, SIGNAL(selectionChanged()), this, SLOT(changePage()));

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);
    QHBoxLayout* buttonLayout = new QHBoxLayout(0);

    buttonLayout->addWidget(newPageButton);
    buttonLayout->addWidget(clearPageButton);
    buttonLayout->addWidget(deletePageButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_pMovePageUpButton);
    buttonLayout->addWidget(m_pMovePageDownButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_pPageListBox);

    updateView();
}

ThumbnailView::~ThumbnailView()
{
    hide();
}

void ThumbnailView::updateView()
{
    m_pMovePageUpButton->setEnabled(m_pDrawPadCanvas->goPreviousPageEnabled());
    m_pMovePageDownButton->setEnabled(m_pDrawPadCanvas->goNextPageEnabled());
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

void ThumbnailView::newPage()
{
    QRect rect = m_pDrawPadCanvas->contentsRect();

    NewPageDialog newPageDialog(rect.width(), rect.height(), m_pDrawPad->pen().color(),
                                m_pDrawPad->brush().color(), this);

    if (newPageDialog.exec() == QDialog::Accepted) {
        m_pDrawPadCanvas->newPage(newPageDialog.selectedTitle(), newPageDialog.selectedWidth(),
                                  newPageDialog.selectedHeight(), newPageDialog.selectedColor());
        m_pPageListBox->updateView();
        updateView();
    }
}

void ThumbnailView::clearPage()
{
    QMessageBox messageBox(tr("Clear Page"), tr("Do you want to clear\nthe selected page?"),
                           QMessageBox::Information, QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
                           QMessageBox::NoButton, this);

    messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    messageBox.setButtonText(QMessageBox::No, tr("No"));

    if (messageBox.exec() == QMessageBox::Yes) {
        m_pDrawPadCanvas->clearPage();
        m_pPageListBox->updateView();
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
        m_pPageListBox->updateView();
        updateView();
    }
}

void ThumbnailView::movePageUp()
{
    m_pDrawPadCanvas->movePageUp();
    m_pPageListBox->updateView();
    updateView();
}

void ThumbnailView::movePageDown()
{
    m_pDrawPadCanvas->movePageDown();
    m_pPageListBox->updateView();
    updateView();
}

void ThumbnailView::changePage()
{
    m_pDrawPadCanvas->selectPage(m_pPageListBox->selected());
    updateView();
}
