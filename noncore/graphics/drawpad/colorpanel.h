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

#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <qframe.h>
#include <qwidget.h>

class QGridLayout;

class ColorPanelButton : public QFrame
{
    Q_OBJECT

public:
    ColorPanelButton(const QColor& color, QWidget* parent = 0, const char* name = 0);
    ~ColorPanelButton();

    void setActive(bool active);

    void enterEvent(QEvent* e);
    void leaveEvent(QEvent* e);
    void paintEvent(QPaintEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

signals:
    void selected(const QColor&);

private:
    QColor m_color;
    bool m_active;
};

class ColorPanel : public QWidget
{ 
    Q_OBJECT

public:
    ColorPanel(QWidget* parent = 0, const char* name = 0);
    ~ColorPanel();

    void addColor(const QColor& color, int row, int col);

public slots:
    void buttonSelected(const QColor& color);

signals:
    void colorSelected(const QColor&);

private:
    QGridLayout* m_pGridLayout;
};

#endif // COLORPANEL_H
