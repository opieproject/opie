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

#include <qwidget.h>

#include <qlist.h>
#include <qpointarray.h>

class DrawPad;

class QPixmap;

class DrawPadCanvas : public QWidget
{ 
    Q_OBJECT

public:
    DrawPadCanvas(DrawPad* drawPad, QWidget* parent = 0, const char* name = 0, WFlags f = 0);
    ~DrawPadCanvas();

    void load(QIODevice* ioDevice);
    void save(QIODevice* ioDevice);

    bool undoEnabled();
    bool redoEnabled();
    bool goPreviousPageEnabled();
    bool goNextPageEnabled();

    QPixmap* currentPage();

public slots:
    void clearAll();
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
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void resizeEvent(QResizeEvent* e);
    void paintEvent(QPaintEvent* e);

private:
    DrawPad* m_pDrawPad;
    QList<QPixmap> m_pages;
    QList<QPixmap> m_pageBackups;
};

#endif // DRAWPADCANVAS_H
