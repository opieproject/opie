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

#ifndef DRAWPAD_H
#define DRAWPAD_H

#include <qmainwindow.h>

#include <qpen.h>

class DrawMode;
class DrawPadCanvas;

class QAction;
class QColor;
class QToolButton;
class QWidgetStack;

class DrawPad : public QMainWindow
{ 
    Q_OBJECT

public:
    DrawPad(QWidget* parent = 0, const char* name = 0, WFlags f = WType_TopLevel);
    ~DrawPad();

    DrawMode* drawMode() { return m_pDrawMode; }
    QPen pen() { return m_pen; }
    QBrush brush() { return m_brush; }

private slots:
    void setPointDrawMode();
    void setLineDrawMode();
    void setRectangleDrawMode();
    void setEllipseDrawMode();
    void setFillDrawMode();
    void setEraseDrawMode();

    void changePenWidth(int value);
    void changePenColor(const QColor& color);
    void changeBrushColor(const QColor& color);
    void choosePenColor();
    void chooseBrushColor();

    void updateUndoRedoToolButtons();
    void updateNavigationToolButtons();

private:
    DrawPadCanvas* m_pDrawPadCanvas;

    DrawMode* m_pDrawMode;
    QPen m_pen;
    QBrush m_brush;

    QAction* m_pUndoAction;
    QAction* m_pRedoAction;

    QAction* m_pFirstPageAction;
    QAction* m_pPreviousPageAction;
    QAction* m_pNextPageAction;
    QAction* m_pLastPageAction;

    QAction* m_pPointDrawModeAction;
    QAction* m_pLineDrawModeAction;
    QAction* m_pRectangleDrawModeAction;
    QAction* m_pEllipseDrawModeAction;
    QAction* m_pFillDrawModeAction;
    QAction* m_pEraseDrawModeAction;

    QToolButton* m_pPenColorToolButton;
    QToolButton* m_pBrushColorToolButton;
};

#endif // DRAWPAD_H
