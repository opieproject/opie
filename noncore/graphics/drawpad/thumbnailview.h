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

#include <qwidget.h>
#include <qlistview.h>

class DrawPadCanvas;
class Page;

class PageListViewItem : public QListViewItem
{
public:
    PageListViewItem(Page* page, QListView* parent);
    ~PageListViewItem();

    Page* page() const;

private:
    Page* m_pPage;
};

class PageListView : public QListView
{
public:
    PageListView(DrawPadCanvas* drawPadCanvas, QWidget* parent = 0, const char* name = 0);
    ~PageListView();

    void updateView();

    void select(Page* page);
    Page* selected() const;

protected:
    void resizeEvent(QResizeEvent* e);

private:
    DrawPadCanvas* m_pDrawPadCanvas;
};

class ThumbnailView : public QWidget
{
    Q_OBJECT

public:
    ThumbnailView(DrawPadCanvas* drawPadCanvas, QWidget* parent = 0, const char* name = 0);
    ~ThumbnailView();
    
    void hide();
    void exec();

public slots:
    void deletePage();
    void changePage();

private:
    bool inLoop;
    DrawPadCanvas* m_pDrawPadCanvas;

    PageListView* m_pPageListView;
};

#endif // THUMBNAILVIEW_H
