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

class DrawPadCanvas;
class Tool;

class QAction;
class QColor;
class QToolButton;
class QWidgetStack;

class DrawPad : public QMainWindow
{
    Q_OBJECT

public:
    DrawPad(QWidget* parent = 0, const char* name = 0);
    ~DrawPad();

    Tool* tool() { return m_pTool; }
    QPen pen() { return m_pen; }
    QBrush brush() { return m_brush; }

private slots:
    void newPage();
    void clearPage();
    void deletePage();

    void setPointTool();
    void setLineTool();
    void setRectangleTool();
    void setFilledRectangleTool();
    void setEllipseTool();
    void setFilledEllipseTool();
    void setTextTool();
    void setFillTool();
    void setEraseTool();

    void changePenWidth(int value);
    void changePenColor(const QColor& color);
    void changeBrushColor(const QColor& color);

    void updateUndoRedoToolButtons();
    void updateNavigationToolButtons();
    void updateCaption();

    void deleteAll();
    void importPage();
    void exportPage();
    void thumbnailView();
    void pageInformation();

private:
    DrawPadCanvas* m_pDrawPadCanvas;

    Tool* m_pTool;
    QPen m_pen;
    QBrush m_brush;

    QAction* m_pUndoAction;
    QAction* m_pRedoAction;

    QAction* m_pFirstPageAction;
    QAction* m_pPreviousPageAction;
    QAction* m_pNextPageAction;
    QAction* m_pLastPageAction;

    QAction* m_pPointToolAction;
    QAction* m_pLineToolAction;
    QAction* m_pRectangleToolAction;
    QAction* m_pFilledRectangleToolAction;
    QAction* m_pEllipseToolAction;
    QAction* m_pFilledEllipseToolAction;
    QAction* m_pTextToolAction;
    QAction* m_pFillToolAction;
    QAction* m_pEraseToolAction;

    QToolButton* m_pLineToolButton;
    QToolButton* m_pRectangleToolButton;
    QToolButton* m_pEllipseToolButton;
    QToolButton* m_pPenColorToolButton;
    QToolButton* m_pBrushColorToolButton;
};

#endif // DRAWPAD_H
