/**********************************************************************
** Copyright (C) 2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

// !!! IMPORTANT !!!
// This interface is still experimental and subject to change.

#ifndef WINDOWDECORATIONINTERFACE_H
#define WINDOWDECORATIONINTERFACE_H

#include <qpe/qcom.h>
#include <qpixmap.h>
#include <qpalette.h>
#include <qwsmanager_qws.h>

// {11A45864-4CBA-4DDA-9846-FF234FD307CC} 
#ifndef IID_WindowDecoration
#define IID_WindowDecoration QUuid( 0x11a45864, 0x4cba, 0x4dda, 0x98, 0x46, 0xff, 0x23, 0x4f, 0xd3, 0x07, 0xcc)
#endif

struct WindowDecorationInterface : public QUnknownInterface
{
    virtual ~WindowDecorationInterface() {}

    struct WindowData {
	QRect rect;
	QPalette palette;
	QString caption;
	enum Flags { Maximized=0x01, Dialog=0x02, Active=0x04 };
	Q_UINT32 flags;
	Q_UINT32 reserved;
    };

    enum Metric { TitleHeight, LeftBorder, RightBorder, TopBorder, BottomBorder, OKWidth, CloseWidth, HelpWidth, MaximizeWidth, CornerGrabSize };
    virtual int metric( Metric m, const WindowData * ) const;

    enum Area { Border, Title, TitleText };
    virtual void drawArea( Area a, QPainter *, const WindowData * ) const;

    enum Button { OK, Close, Help, Maximize };
    virtual void drawButton( Button b, QPainter *, const WindowData *, int x, int y, int w, int h, QWSButton::State ) const;

    virtual QRegion mask( const WindowData * ) const;

    virtual QString name() const = 0;
    virtual QPixmap icon() const = 0;
};

#endif
