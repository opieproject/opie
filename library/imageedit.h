/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef IMAGEEDIT_H
#define IMAGEEDIT_H

#include <qscrollview.h>
#include <qpixmap.h>

class ImageEditPrivate;
class ImageEdit : public QScrollView
{
    Q_OBJECT

public:
    ImageEdit( QWidget *parent = 0, const char *name = 0 );
    ~ImageEdit();
    
    void setPixmap( const QPixmap &pm );
    QPixmap pixmap() const;
    
protected:
    void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    void contentsMousePressEvent( QMouseEvent *e );
    void contentsMouseMoveEvent( QMouseEvent *e );
    void contentsMouseReleaseEvent( QMouseEvent *e );
    void viewportResizeEvent( QResizeEvent *e );

private:
    QPoint lastPos;
    QPixmap buffer;
    ImageEditPrivate *d;
    void enlargeBuffer( const QSize& sz );
};

#endif
