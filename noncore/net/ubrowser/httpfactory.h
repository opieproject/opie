/*
Opie-uBrowser.  a very small web browser, using on QTextBrowser for html display/parsing
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <qmime.h>
#include <qsocket.h>
#include <qstring.h>
#include <qdragobject.h>
#include <qtextbrowser.h>

#include <stdio.h>

#include "httpcomm.h"

class HttpFactory : public QMimeSourceFactory
{
public:
	HttpFactory(QTextBrowser *newBrowser);
	const QMimeSource * data(const QString &abs_name) const;
	const QMimeSource * data(const QString &abs_or_rel_name, const QString & context) const;
private:
	QSocket *socket;
	HttpComm *comm;
	QTextDrag *text;
	QImageDrag *image;
	QTextBrowser *browser;
};
