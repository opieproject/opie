/**********************************************************************
** Copyright (C) 2004 Anton Kachalov mouse@altlinux.ru
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

#include "multikey.h"

/* OPIE */
#include <opie2/otaskbarapplet.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qdir.h>

using namespace Opie::Ui;
Multikey::Multikey(QWidget *parent) : QLabel(parent), popupMenu(this), current("EN")
{
    QCopChannel* swChannel = new QCopChannel("MultiKey/Switcher", this);
    connect( swChannel, SIGNAL(received(const QCString&,const QByteArray&)),
	     this, SLOT(message(const QCString&,const QByteArray&)));

    setFont( QFont( "Helvetica", 10, QFont::Normal ) );
    QPEApplication::setStylusOperation(this, QPEApplication::RightOnHold);
    lang = 0;
    QCopEnvelope e("MultiKey/Keyboard", "getmultikey()");
    setText("EN");
    popupMenu.insertItem("EN", 0);
    show();
}

void Multikey::mousePressEvent(QMouseEvent *ev)
{
    if (!sw_maps.count())
	return;

    if (ev->button() == RightButton) {

	QPoint p = mapToGlobal(QPoint(0, 0));
	QSize s = popupMenu.sizeHint();
	int opt = popupMenu.exec(QPoint(p.x() + (width() / 2) - (s.width() / 2),
					p.y() - s.height()), 0);

	if (opt == -1)
	    return;
	lang = opt;

	QCopEnvelope e("MultiKey/Keyboard", "setmultikey(QString)");
	e << sw_maps[lang];
        setText(labels[lang]);
    }
    QWidget::mousePressEvent(ev);
}

void Multikey::mouseReleaseEvent(QMouseEvent *ev)
{
    if (!sw_maps.count())
	return;

    lang = lang < sw_maps.count()-1 ? lang+1 : 0;
    QCopEnvelope e("MultiKey/Keyboard", "setmultikey(QString)");
    //odebug << "Lang=" << lang << ", count=" << sw_maps.count() << ", lab=" << labels[lang].ascii() << "" << oendl; 
    e << sw_maps[lang];
    setText(labels[lang]);
}

void Multikey::message(const QCString &message, const QByteArray &data)
{
    if ( message == "setsw(QString,QString)" ) {

	QDataStream stream(data, IO_ReadOnly);
	QString maps, current_map;
	stream >> maps >> current_map;
	QStringList sw = QStringList::split(QChar('|'), maps);
	sw.append(current_map);

	QDir map_dir(QPEApplication::qpeDir() + "/share/multikey/", "*.keymap");
	lang = 0;
	labels.clear();
	sw_maps.clear();
	popupMenu.clear();

	for (uint i = 0; i < sw.count(); ++i) {
	    QString keymap_map;
	    if (sw[i][0] != '/') {

		keymap_map =  map_dir.absPath() + "/" + sw[i];
	    } else {

		if ((map_dir.exists(QFileInfo(sw[i]).fileName(), false)
		     && i != sw.count()-1) || !QFile::exists(sw[i])) {
		    continue;
		}
		keymap_map = sw[i];
	    }

	    QFile map(keymap_map);
	    if (map.open(IO_ReadOnly)) {
		QString line;

		map.readLine(line, 1024);
		while (!map.atEnd()) {

		    if (line.find(QRegExp("^sw\\s*=\\s*")) != -1) {

			if (i != sw.count()-1) {
			    if (keymap_map == current_map) {
				lang = i;
			    }
			    sw_maps.append(keymap_map);
			    labels.append(line.right(line.length() - line.find(QChar('=')) - 1).stripWhiteSpace());
			    popupMenu.insertItem(labels[labels.count()-1], labels.count()-1);
			} else {
			    current = line.right(line.length() - line.find(QChar('=')) - 1).stripWhiteSpace();
			}
			break;
		    }
		    map.readLine(line, 1024);
		}
		map.close();
	    }
	}

	setText(current);
    }
}

int Multikey::position()
{
    return 10;
}

EXPORT_OPIE_APPLET_v1( Multikey )
