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

#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <qlistbox.h>
#include <qwidget.h>

class DrawPad;
class DrawPadCanvas;
class Page;

#include <qdatetime.h>

class QToolButton;

class PageListBoxItem : public QListBoxItem
{
public:
    PageListBoxItem(Page* page, QListBox* parent);
    ~PageListBoxItem();

    int height(const QListBox* lb) const;
    int width(const QListBox* lb) const;
    void paint(QPainter *painter);

    Page* page() const;

private:
    QString dateTimeString(QDateTime dateTime);

    Page* m_pPage;

    QPixmap m_thumbnail;

    QString m_titleText;
    QString m_dimensionText;
    QString m_dateText;

    QColor m_alternateColor;
};

class PageListBox : public QListBox
{
public:
    PageListBox(DrawPadCanvas* drawPadCanvas, QWidget* parent = 0, const char* name = 0);
    ~PageListBox();

    void updateView();

    void select(Page* page);
    Page* selected() const;

private:
    DrawPadCanvas* m_pDrawPadCanvas;
};

class ThumbnailView : public QWidget
{
    Q_OBJECT

public:
    ThumbnailView(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas, QWidget* parent = 0, const char* name = 0);
    ~ThumbnailView();

    void updateView();

    void hide();
    void exec();

public slots:
    void newPage();
    void clearPage();
    void deletePage();
    void movePageUp();
    void movePageDown();
    void changePage();

private:
    bool inLoop;

    DrawPad* m_pDrawPad;
    DrawPadCanvas* m_pDrawPadCanvas;

    QToolButton* m_pMovePageUpButton;
    QToolButton* m_pMovePageDownButton;

    PageListBox* m_pPageListBox;
};

#endif // THUMBNAILVIEW_H
