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
#include "pickboard.h"
#include "pickboardpicks.h"
#include "pickboardcfg.h"

#include <qpe/global.h>

#include <qpainter.h>
#include <qlist.h>
#include <qbitmap.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qdialog.h>
#include <qscrollview.h>
#include <qpopupmenu.h>
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qwindowsystem_qws.h>

/* XPM */
static const char * const menu_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
".........",
".........",
"....a....",
"...aaa...",
"..aaaaa..",
".aaaaaaa.",
".........",
"........."};

class PickboardPrivate {
public:
    PickboardPrivate(Pickboard* parent)
    {
	picks = new PickboardPicks(parent);
	picks->initialise();
	menu = new QPushButton(parent);
	menu->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding));
	menu->setPixmap(QPixmap((const char **)menu_xpm));
	QObject::connect(menu,SIGNAL(clicked()),picks,SLOT(doMenu()));
	QObject::connect(picks,SIGNAL(key(ushort,ushort,ushort,bool,bool)),
	    parent,SIGNAL(key(ushort,ushort,ushort,bool,bool)));
    }

    PickboardPicks* picks;
    QPushButton* menu;
};

Pickboard::Pickboard(QWidget* parent, const char* name, WFlags f) :
    QFrame(parent,name,f)
{
    (new QHBoxLayout(this))->setAutoAdd(TRUE);
    d = new PickboardPrivate(this);
    setFont( QFont( "smallsmooth", 9 ) );
}

Pickboard::~Pickboard()
{
    delete d;
}

void Pickboard::resetState()
{
    d->picks->resetState();
}

