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

#include "drawpad.h"

#include "drawpadcanvas.h"
#include "ellipsedrawmode.h"
#include "erasedrawmode.h"
#include "filldrawmode.h"
#include "linedrawmode.h"
#include "pointdrawmode.h"
#include "rectangledrawmode.h"

#include <qpe/global.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qfile.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qtooltip.h>

DrawPad::DrawPad(QWidget* parent, const char* name, WFlags f)
    : QMainWindow(parent, name, f)
{
    setCaption(tr("DrawPad"));

    // init members

    m_pDrawPadCanvas = new DrawPadCanvas(this, this);

    QFile file(Global::applicationFileName("drawpad", "drawpad.xml"));

    if (file.open(IO_ReadOnly)) {
        m_pDrawPadCanvas->load(&file);
        file.close();
    }

    setCentralWidget(m_pDrawPadCanvas);

    m_colors.resize(8);
    m_colors.at(0) = Qt::black;
    m_colors.at(1) = Qt::white;
    m_colors.at(2) = Qt::red;
    m_colors.at(3) = Qt::green;
    m_colors.at(4) = Qt::blue;
    m_colors.at(5) = Qt::cyan;
    m_colors.at(6) = Qt::magenta;
    m_colors.at(7) = Qt::yellow;

    // init menu

    setToolBarsMovable(false);

    QPEToolBar* menuToolBar = new QPEToolBar(this);
    QPEMenuBar* menuBar = new QPEMenuBar(menuToolBar);

    QPopupMenu *toolsPopupMenu = new QPopupMenu(menuBar);

    QAction* clearAllAction = new QAction(tr("Clear All"), QString::null, 0, this);
    connect(clearAllAction, SIGNAL(activated()), this, SLOT(clearAll()));
    clearAllAction->addTo(toolsPopupMenu);

    toolsPopupMenu->insertSeparator();

    QAction* setOptionsAction = new QAction(tr("Options"), tr("Options..."), 0, this);
    setOptionsAction->addTo(toolsPopupMenu);

    menuBar->insertItem(tr("Tools"), toolsPopupMenu);

    // init page toolbar

    QPEToolBar* pageToolBar = new QPEToolBar(this);

    QAction* newPageAction = new QAction(tr("New Page"), Resource::loadIconSet("new"), QString::null, 0, this);
    connect(newPageAction, SIGNAL(activated()), this, SLOT(newPage()));
    newPageAction->addTo(pageToolBar);

    QAction* clearPageAction = new QAction(tr("Clear Page"), Resource::loadIconSet("drawpad/clear"), QString::null, 0, this);
    connect(clearPageAction, SIGNAL(activated()), m_pDrawPadCanvas, SLOT(clearPage()));
    clearPageAction->addTo(pageToolBar);

    QAction* deletePageAction = new QAction(tr("Delete Page"), Resource::loadIconSet("trash"), QString::null, 0, this);
    connect(deletePageAction, SIGNAL(activated()), this, SLOT(deletePage()));
    deletePageAction->addTo(pageToolBar);

    QPEToolBar* emptyToolBar = new QPEToolBar(this);
    emptyToolBar->setHorizontalStretchable(true);

    // init navigation toolbar

    QPEToolBar* navigationToolBar = new QPEToolBar(this);

    m_pFirstPageAction = new QAction(tr("First Page"), Resource::loadIconSet("fastback"), QString::null, 0, this);
    connect(m_pFirstPageAction, SIGNAL(activated()), this, SLOT(goFirstPage()));
    m_pFirstPageAction->addTo(navigationToolBar);

    m_pPreviousPageAction = new QAction(tr("Previous Page"), Resource::loadIconSet("back"), QString::null, 0, this);
    connect(m_pPreviousPageAction, SIGNAL(activated()), this, SLOT(goPreviousPage()));
    m_pPreviousPageAction->addTo(navigationToolBar);

    m_pNextPageAction = new QAction(tr("Next Page"), Resource::loadIconSet("forward"), QString::null, 0, this);
    connect(m_pNextPageAction, SIGNAL(activated()), this, SLOT(goNextPage()));
    m_pNextPageAction->addTo(navigationToolBar);

    m_pLastPageAction = new QAction(tr("Last Page"), Resource::loadIconSet("fastforward"), QString::null, 0, this);
    connect(m_pLastPageAction, SIGNAL(activated()), this, SLOT(goLastPage()));
    m_pLastPageAction->addTo(navigationToolBar);

    updateNavigationToolBar();

    // init draw mode toolbar

    QPEToolBar* drawModeToolBar = new QPEToolBar(this);

    m_pPointDrawModeAction = new QAction(tr("Draw Point"), Resource::loadIconSet("drawpad/point.png"), QString::null, 0, this);
    m_pPointDrawModeAction->setToggleAction(true);
    connect(m_pPointDrawModeAction, SIGNAL(activated()), this, SLOT(setPointDrawMode()));
    m_pPointDrawModeAction->addTo(drawModeToolBar);

    m_pLineDrawModeAction = new QAction(tr("Draw Line"), Resource::loadIconSet("drawpad/line.png"), QString::null, 0, this);
    m_pLineDrawModeAction->setToggleAction(true);
    connect(m_pLineDrawModeAction, SIGNAL(activated()), this, SLOT(setLineDrawMode()));
    m_pLineDrawModeAction->addTo(drawModeToolBar);

    m_pRectangleDrawModeAction = new QAction(tr("Draw Rectangle"), Resource::loadIconSet("drawpad/rectangle.png"), QString::null, 0, this);
    m_pRectangleDrawModeAction->setToggleAction(true);
    connect(m_pRectangleDrawModeAction, SIGNAL(activated()), this, SLOT(setRectangleDrawMode()));
    m_pRectangleDrawModeAction->addTo(drawModeToolBar);

    m_pEllipseDrawModeAction = new QAction(tr("Draw Ellipse"), Resource::loadIconSet("drawpad/ellipse.png"), QString::null, 0, this);
    m_pEllipseDrawModeAction->setToggleAction(true);
    connect(m_pEllipseDrawModeAction, SIGNAL(activated()), this, SLOT(setEllipseDrawMode()));
    m_pEllipseDrawModeAction->addTo(drawModeToolBar);

    m_pFillDrawModeAction = new QAction(tr("Fill Region"), Resource::loadIconSet("drawpad/fill.png"), QString::null, 0, this);
    m_pFillDrawModeAction->setToggleAction(true);
    connect(m_pFillDrawModeAction, SIGNAL(activated()), this, SLOT(setFillDrawMode()));
    m_pFillDrawModeAction->addTo(drawModeToolBar);

    m_pEraseDrawModeAction = new QAction(tr("Erase Point"), Resource::loadIconSet("drawpad/erase.png"), QString::null, 0, this);
    m_pEraseDrawModeAction->setToggleAction(true);
    connect(m_pEraseDrawModeAction, SIGNAL(activated()), this, SLOT(setEraseDrawMode()));
    m_pEraseDrawModeAction->addTo(drawModeToolBar);

    m_pDrawMode = 0;
    setPointDrawMode();

    emptyToolBar = new QPEToolBar(this);
    emptyToolBar->setHorizontalStretchable(true);

    QPEToolBar* drawParametersToolBar = new QPEToolBar(this);

    QSpinBox* penWidthSpinBox = new QSpinBox(1, 9, 1, drawParametersToolBar);
    connect(penWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changePenWidth(int)));

    penWidthSpinBox->setValue(1);

    m_pPenColorToolButton = new QToolButton(drawParametersToolBar);
    m_pPenColorToolButton->setPixmap(Resource::loadPixmap("drawpad/pencolor.png"));

    QPopupMenu* penColorPopupMenu = new QPopupMenu(m_pPenColorToolButton);
    connect(penColorPopupMenu, SIGNAL(activated(int)), this, SLOT(changePenColor(int)));

    QPixmap penColorPixmap(14, 14);

    for (uint i = 0; i < m_colors.size(); i++) {
        penColorPixmap.fill(m_colors.at(i));
        penColorPopupMenu->insertItem(penColorPixmap, i);
    }

    QToolTip::add(m_pPenColorToolButton, tr("Pen Color"));
    m_pPenColorToolButton->setPopup(penColorPopupMenu);
    m_pPenColorToolButton->setPopupDelay(0);

    penColorPopupMenu->activateItemAt(0);

    m_pBrushColorToolButton = new QToolButton(drawParametersToolBar);
    m_pBrushColorToolButton->setPixmap(Resource::loadPixmap("drawpad/brushcolor.png"));

    QPopupMenu* brushColorPopupMenu = new QPopupMenu(m_pBrushColorToolButton);
    connect(brushColorPopupMenu, SIGNAL(activated(int)), this, SLOT(changeBrushColor(int)));

    QPixmap brushColorPixmap(14, 14);

    for (uint i = 0; i < m_colors.size(); i++) {
        brushColorPixmap.fill(m_colors.at(i));
        brushColorPopupMenu->insertItem(brushColorPixmap, i);
    }

    QToolTip::add(m_pBrushColorToolButton, tr("Fill Color"));
    m_pBrushColorToolButton->setPopup(brushColorPopupMenu);
    m_pBrushColorToolButton->setPopupDelay(0);

    brushColorPopupMenu->activateItemAt(1);
}

DrawPad::~DrawPad()
{
    QFile file(Global::applicationFileName("drawpad", "drawpad.xml"));

    if (file.open(IO_WriteOnly)) {
        m_pDrawPadCanvas->save(&file);
        file.close();
    }
}

void DrawPad::clearAll()
{
    m_pDrawPadCanvas->clearAll();
    updateNavigationToolBar();
}

void DrawPad::newPage()
{
    m_pDrawPadCanvas->newPage();
    updateNavigationToolBar();
}

void DrawPad::deletePage()
{
    m_pDrawPadCanvas->deletePage();
    updateNavigationToolBar();
}

void DrawPad::goFirstPage()
{
    m_pDrawPadCanvas->goFirstPage();
    updateNavigationToolBar();
}

void DrawPad::goPreviousPage()
{
    m_pDrawPadCanvas->goPreviousPage();
    updateNavigationToolBar();
}

void DrawPad::goNextPage()
{
    m_pDrawPadCanvas->goNextPage();
    updateNavigationToolBar();
}

void DrawPad::goLastPage()
{
    m_pDrawPadCanvas->goLastPage();
    updateNavigationToolBar();
}

void DrawPad::setPointDrawMode()
{
    if (m_pDrawMode) {
        delete m_pDrawMode;
    }

    m_pDrawMode = new PointDrawMode(this, m_pDrawPadCanvas);

    m_pPointDrawModeAction->setOn(true);
    m_pLineDrawModeAction->setOn(false);
    m_pRectangleDrawModeAction->setOn(false);
    m_pEllipseDrawModeAction->setOn(false);
    m_pFillDrawModeAction->setOn(false);
    m_pEraseDrawModeAction->setOn(false);
}

void DrawPad::setLineDrawMode()
{
    if (m_pDrawMode) {
        delete m_pDrawMode;
    }

    m_pDrawMode = new LineDrawMode(this, m_pDrawPadCanvas);

    m_pPointDrawModeAction->setOn(false);
    m_pLineDrawModeAction->setOn(true);
    m_pRectangleDrawModeAction->setOn(false);
    m_pEllipseDrawModeAction->setOn(false);
    m_pFillDrawModeAction->setOn(false);
    m_pEraseDrawModeAction->setOn(false);
}

void DrawPad::setRectangleDrawMode()
{
    if (m_pDrawMode) {
        delete m_pDrawMode;
    }

    m_pDrawMode = new RectangleDrawMode(this, m_pDrawPadCanvas);

    m_pPointDrawModeAction->setOn(false);
    m_pLineDrawModeAction->setOn(false);
    m_pRectangleDrawModeAction->setOn(true);
    m_pEllipseDrawModeAction->setOn(false);
    m_pFillDrawModeAction->setOn(false);
    m_pEraseDrawModeAction->setOn(false);
}

void DrawPad::setEllipseDrawMode()
{
    if (m_pDrawMode) {
        delete m_pDrawMode;
    }

    m_pDrawMode = new EllipseDrawMode(this, m_pDrawPadCanvas);

    m_pPointDrawModeAction->setOn(false);
    m_pLineDrawModeAction->setOn(false);
    m_pRectangleDrawModeAction->setOn(false);
    m_pEllipseDrawModeAction->setOn(true);
    m_pFillDrawModeAction->setOn(false);
    m_pEraseDrawModeAction->setOn(false);
}

void DrawPad::setFillDrawMode()
{
    if (m_pDrawMode) {
        delete m_pDrawMode;
    }

    m_pDrawMode = new FillDrawMode(this, m_pDrawPadCanvas);

    m_pPointDrawModeAction->setOn(false);
    m_pLineDrawModeAction->setOn(false);
    m_pRectangleDrawModeAction->setOn(false);
    m_pEllipseDrawModeAction->setOn(false);
    m_pFillDrawModeAction->setOn(true);
    m_pEraseDrawModeAction->setOn(false);
}

void DrawPad::setEraseDrawMode()
{
    if (m_pDrawMode) {
        delete m_pDrawMode;
    }

    m_pDrawMode = new EraseDrawMode(this, m_pDrawPadCanvas);

    m_pPointDrawModeAction->setOn(false);
    m_pLineDrawModeAction->setOn(false);
    m_pRectangleDrawModeAction->setOn(false);
    m_pEllipseDrawModeAction->setOn(false);
    m_pFillDrawModeAction->setOn(false);
    m_pEraseDrawModeAction->setOn(true);
}

void DrawPad::changePenWidth(int value)
{
    m_pen.setWidth(value);
}

void DrawPad::changePenColor(int index)
{
    m_pen.setColor(m_colors.at(index));

    QPainter painter;
    painter.begin(m_pPenColorToolButton->pixmap());
    painter.fillRect(QRect(0, 12, 14, 2), m_pen.color());
    painter.end();
}

void DrawPad::changeBrushColor(int index)
{
    m_brush = QBrush(m_colors.at(index));

    QPainter painter;
    painter.begin(m_pBrushColorToolButton->pixmap());
    painter.fillRect(QRect(0, 12, 14, 2), m_brush.color());
    painter.end();
}

void DrawPad::updateNavigationToolBar()
{
    m_pFirstPageAction->setEnabled(m_pDrawPadCanvas->goPreviousPageEnabled());
    m_pPreviousPageAction->setEnabled(m_pDrawPadCanvas->goPreviousPageEnabled());
    m_pNextPageAction->setEnabled(m_pDrawPadCanvas->goNextPageEnabled());
    m_pLastPageAction->setEnabled(m_pDrawPadCanvas->goNextPageEnabled());
}
