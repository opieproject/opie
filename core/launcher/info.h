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

#ifndef __INFO_H__
#define __INFO_H__


#include <qwidget.h>
#include <qpainter.h>
#include "background.h"

class QSimpleRichText;

class Info : public Background {
    Q_OBJECT
public:
    Info( Desktop *d );
    void menuClicked( );

signals:
    void giveInfo( );

protected:
    void mouseReleaseEvent( QMouseEvent *e );
    void paintEvent( QPaintEvent *pe );

private:
    QSimpleRichText* info;
    bool needsClear;
    bool menuHasBeenClicked;
};


extern Info *desktopInfo;


#endif // __INFO_H__

