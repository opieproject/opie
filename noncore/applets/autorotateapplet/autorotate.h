/*
 * copyright   : (c) 2003 by Greg Gilbert
 * email       : greg@treke.net
 * based on the cardmon applet by Max Reiss
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 *************************************************************************/

#ifndef AUTOROTATE_H
#define AUTOROTATE_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

class AutoRotate : public QWidget {
    Q_OBJECT
public:
    AutoRotate( QWidget *parent = 0 );
    ~AutoRotate();

private slots:

protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );
private:
    bool isRotateEnabled();
    void setRotateEnabled(bool);
    QPixmap enabledPm;
    QPixmap disabledPm;
    void iconShow();
    QPopupMenu *popupMenu;
 };

#endif

