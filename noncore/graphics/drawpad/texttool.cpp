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

#include "texttool.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpixmap.h>

TextToolDialog::TextToolDialog(QWidget* parent, const char* name)
    : QDialog(parent, name, true)
{
    setCaption(tr("Insert Text"));

    m_pLineEdit = new QLineEdit(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 4);

    mainLayout->addWidget(m_pLineEdit);
}

TextToolDialog::~TextToolDialog()
{
}

QString TextToolDialog::text()
{
    return m_pLineEdit->text();
}

TextTool::TextTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : Tool(drawPad, drawPadCanvas)
{
}

TextTool::~TextTool()
{
}

void TextTool::mousePressEvent(QMouseEvent* e)
{
    TextToolDialog textToolDialog(m_pDrawPad);

    if (textToolDialog.exec() == QDialog::Accepted && !textToolDialog.text().isEmpty()) {
        QPainter painter;
        painter.begin(m_pDrawPadCanvas->currentPage());
        painter.setPen(m_pDrawPad->pen());
        painter.drawText(e->x(), e->y(), textToolDialog.text());
        painter.end();

        m_pDrawPadCanvas->viewport()->update();

        m_pDrawPadCanvas->backupPage();
    }
}

void TextTool::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e)
}

void TextTool::mouseMoveEvent(QMouseEvent* e)
{
    Q_UNUSED(e)
}
