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

#ifndef DRAWPADCANVAS_H
#define DRAWPADCANVAS_H

#include <qscrollview.h>

#include <qlist.h>
#include <qpointarray.h>

class DrawPad;

class QPixmap;

class DrawPadCanvas : public QScrollView
{ 
    Q_OBJECT

public:
    DrawPadCanvas(DrawPad* drawPad, QWidget* parent = 0, const char* name = 0);
    ~DrawPadCanvas();

    void load(QIODevice* ioDevice);
    void initialPage();
    void save(QIODevice* ioDevice);

    void importPage(const QString& fileName);
    void exportPage(uint fromPage, uint toPage, const QString& name, const QString& format);

    bool undoEnabled();
    bool redoEnabled();
    bool goPreviousPageEnabled();
    bool goNextPageEnabled();

    QPixmap* currentPage();
    uint pagePosition();
    uint pageCount();

public slots:
    void deleteAll();
    void newPage();
    void clearPage();
    void deletePage();

    void undo();
    void redo();

    void goFirstPage();
    void goPreviousPage();
    void goNextPage();
    void goLastPage();

signals:
    void pagesChanged();
    void pageBackupsChanged();

protected:
    void contentsMousePressEvent(QMouseEvent* e);
    void contentsMouseReleaseEvent(QMouseEvent* e);
    void contentsMouseMoveEvent(QMouseEvent* e);
    void drawContents(QPainter* p, int cx, int cy, int cw, int ch);

private:
    DrawPad* m_pDrawPad;
    QList<QPixmap> m_pages;
    QList<QPixmap> m_pageBackups;
};

#endif // DRAWPADCANVAS_H
