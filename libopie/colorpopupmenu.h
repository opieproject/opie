/**********************************************************************
** ColorPopupMenu
**
** Popup menu for selecting colors
**
** Based on colorpanel by S. Prud'homme <prudhomme@laposte.net>
**
** Copyright (C) 2002, Dan Williams
**                    williamsdr@acm.org
**                    http://draknor.net
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef COLORPOPUPMENU_H
#define COLORPOPUPMENU_H

#include <qframe.h>
#include <qpopupmenu.h>

class QWidget;
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

class ColorPopupMenu : public QPopupMenu
{
    Q_OBJECT

public:
    ColorPopupMenu( const QColor& color, QWidget* parent = 0, const char* name = 0 );
    ~ColorPopupMenu();

private:
    QColor       m_color;
    QWidget*     colorPanel;
    QGridLayout* colorLayout;

    void addColor( const QColor& color, int row, int col );

signals:
    void colorSelected( const QColor& color );

protected slots:
    void buttonSelected( const QColor& color );
    void moreColorClicked();
};

#endif // COLORPOPUPMENUANEL_H
