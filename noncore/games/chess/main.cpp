/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
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
#include <qpe/qpeapplication.h>
#include <qpe/qpetoolbar.h>
#include <qmainwindow.h>
#include <qcanvas.h>
#include "chess.h"


class CanvasMainWindow : public QMainWindow {
public:
    CanvasMainWindow(QWidget* parent=0, const char* name=0, WFlags f=0)
	: QMainWindow(parent,name,f), canvas(232, 258) {
	view = new BoardView(&canvas, this, 0);
	setToolBarsMovable( FALSE );
	QPEToolBar* toolbar = new QPEToolBar(this);
	toolbar->setHorizontalStretchable( TRUE );
    }

private:
    QCanvas canvas;
    BoardView *view;
};


int main( int argc, char **argv ) {
    QPEApplication a(argc,argv);
    CanvasMainWindow *mw = new CanvasMainWindow();
    a.showMainWidget( mw );
    return a.exec();
}

