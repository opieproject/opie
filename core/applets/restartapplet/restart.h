/**********************************************************************
** Copyright (C) 2002 L.J. Potter ljp@llornkcor.com
**  All rights reserved.
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

#ifndef __RESTART_APPLET_H__
#define __RESTART_APPLET_H__

#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qguardedptr.h>

class RestartApplet : public QWidget
{
    Q_OBJECT
public:
    RestartApplet( QWidget *parent = 0, const char *name=0 );
    ~RestartApplet();
public slots:
private:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );

private:
//    QPixmap snapshotPixmap;
private slots:

        
};


#endif // __RESTART_APPLET_H__

