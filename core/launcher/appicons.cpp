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


#include "appicons.h"

#include <qpe/qcopenvelope_qws.h>

#include <qtooltip.h>
#include <qpixmap.h>


AppIcons::AppIcons( QWidget *parent ) :
    QHBox(parent)
{
    buttons.setAutoDelete(TRUE);

#ifndef QT_NO_COP
    QCopChannel* channel = new QCopChannel("Qt/Tray", this);
    connect(channel, SIGNAL(received(const QCString&, const QByteArray&)),
	    this, SLOT(receive(const QCString&, const QByteArray&)));
#endif
}

void AppIcons::setIcon(int id, const QPixmap& pm)
{
    button(id)->setPixmap(pm);
}

class FlatButton : public QLabel {
    Q_OBJECT
public:
    FlatButton(QWidget* parent) : QLabel(parent) { }

    void mouseDoubleClickEvent(QMouseEvent* e)
    {
	emit clicked(e->pos(),e->button(),TRUE);
    }
    void mouseReleaseEvent(QMouseEvent* e)
    {
	if ( rect().contains(e->pos()) )
	    emit clicked(e->pos(),e->button(),FALSE);
    }

signals:
    void clicked(const QPoint&, int, bool);
};

QLabel* AppIcons::button(int id)
{
    QLabel* f = buttons.find(id);
    if ( !f ) {
	buttons.insert(id,f=new FlatButton(this));
	connect(f,SIGNAL(clicked(const QPoint&, int, bool)),this,SLOT(clicked(const QPoint&, int, bool)));
	f->show();
    }
    return f;
}

int AppIcons::findId(QLabel* b)
{
    QIntDictIterator<QLabel> it(buttons);
    for ( ; ; ++it )
	if ( it.current() == b ) return it.currentKey();
}

void AppIcons::clicked(const QPoint& relpos, int button, bool dbl)
{
#ifndef QT_NO_COP
    QLabel* s = (QLabel*)sender();
    if ( button == RightButton ) {
	QCopEnvelope("Qt/Tray","popup(int,QPoint)")
	    << findId(s) << s->mapToGlobal(QPoint(0,0));
    } else {
	QCopEnvelope("Qt/Tray",
		dbl ? "doubleClicked(int,QPoint)" : "clicked(int,QPoint)")
	    << findId(s) << relpos;
    }
#endif
}

void AppIcons::setToolTip(int id, const QString& tip)
{
    QToolTip::add(button(id),tip);
}

void AppIcons::remove(int id)
{
    buttons.remove(id);
}

void AppIcons::receive( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "remove(int)" ) {
	int id;
	stream >> id;
	remove(id);
    } else if ( msg == "setIcon(int,QPixmap)" ) {
	int id;
	QPixmap pm;
	stream >> id >> pm;
	setIcon(id,pm);
    } else if ( msg == "setToolTip(int,QString)" ) {
	int id;
	QString s;
	stream >> id >> s;
	setToolTip(id,s);
    }
}

#include "appicons.moc"
