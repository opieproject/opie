/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#ifndef MAILLIST_H
#define MAILLIST_H

#include <qobject.h>
#include <qlist.h>

struct dList{
	int serverId;
	uint size;
};

class MailList : public QObject
{
	Q_OBJECT

public:
	void clear();
	int count();
	int* first();
	int* next();
	void sizeInsert(int serverId, uint size);
	void moveFront(int serverId, uint size);
	bool remove(int serverId, uint size);
	void insert(int pos, int serverId, uint size);
	
private:
	QList<dList> sortedList;
	uint currentPos;
};

#endif
