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

#ifndef TEXTTOOL_H
#define TEXTTOOL_H

#include "tool.h"

#include <qdialog.h>

class QLineEdit;

class TextToolDialog : public QDialog
{
public:
    TextToolDialog(QWidget* parent = 0, const char* name = 0);
    ~TextToolDialog();

    QString text();

private:
    QLineEdit* m_pLineEdit;
};

class TextTool : public Tool
{
public:
    TextTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~TextTool();

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
};

#endif // TEXTTOOL_H
