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

#include <qpe/global.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include <qlabel.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qcopchannel_qws.h>

Multikey::Multikey( QWidget *parent ) : QLabel( parent ), current("EN")
{
    QCopChannel* swChannel = new QCopChannel("MultiKey/Switcher", this);
    connect( swChannel, SIGNAL(received(const QCString &, const QByteArray &)),
	     this, SLOT(message(const QCString &, const QByteArray &)));

    setFont( QFont( "Helvetica", 10, QFont::Normal ) );
    lang = 0;
    QCopEnvelope e("MultiKey/Keyboard", "getmultikey()");
    setText("EN");
    show();
}

void Multikey::mousePressEvent( QMouseEvent * )
{
    QCopEnvelope e("MultiKey/Keyboard", "setmultikey(QString)");
    lang = lang < sw_maps.count()-1 ? lang+1 : 0;
    //qDebug("Lang=%d, count=%d, lab=%s", lang, sw_maps.count(), labels[lang].ascii());
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
