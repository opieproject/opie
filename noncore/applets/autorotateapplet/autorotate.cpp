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


#include "autorotate.h"

#include <qpe/resource.h>

#include <opie/odevice.h>

#include <qpe/applnk.h>
#include <qpe/config.h>

#include <qcopchannel_qws.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qapplication.h>

using namespace Opie;

AutoRotate::AutoRotate(QWidget * parent):QWidget(parent),
    enabledPm( Resource::loadPixmap("autorotate/rotate") ), 
    disabledPm( Resource::loadPixmap("autorotate/norotate") )
{
    setFixedWidth ( AppLnk::smallIconSize()  );
    setFixedHeight ( AppLnk::smallIconSize()  );

    repaint(true);
    popupMenu = 0;
    show();
}

AutoRotate::~AutoRotate()
{
    if (popupMenu) {
        delete popupMenu;
    }
}

void AutoRotate::mousePressEvent(QMouseEvent *)
{
    QPopupMenu *menu = new QPopupMenu(this);

    if (isRotateEnabled())
	    menu->insertItem("Disable Rotation",1);
    else
	    menu->insertItem("Enable Rotation",1);


    QPoint p = mapToGlobal(QPoint(0, 0));
    QSize s = menu->sizeHint();
    int opt = menu->exec(QPoint(p.x() + (width() / 2) - (s.width() / 2),
				p.y() - s.height()), 0);

    if (opt==1) {
        if (isRotateEnabled())
            setRotateEnabled(false);
        else
            setRotateEnabled(true);
                
        repaint(true);
    }

    delete menu;
}

void AutoRotate::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    if ( isRotateEnabled() ) {
	    p.drawPixmap(0, 0, enabledPm  );
    } else {
	    p.drawPixmap(0, 0, disabledPm  );
    }
}

void AutoRotate::setRotateEnabled(bool status)
{
    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );
    cfg.writeEntry( "rotateEnabled",status );

}
bool AutoRotate::isRotateEnabled()
{
    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );

    bool res = cfg.readBoolEntry( "rotateEnabled" );

    if (res ) 
        qDebug("Enabled");
    else
        qDebug("Disabled");
    return res;
}

