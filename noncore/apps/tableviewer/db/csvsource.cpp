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
#include "csvsource.h"
#include "common.h"
#include "datacache.h"
#include <qtextstream.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qregexp.h>

DBCsv::DBCsv(DBStore *d)
{
    dstore = d;
}

DBCsv::~DBCsv() 
{
}

QString DBCsv::type()
{
    return "csv";
}

QStringList readElem(QString in)
{
    QStringList out;

    if (in.isEmpty())
	return out;

    bool firstChar = TRUE;
    bool quotedElem = FALSE;
    uint index = 0;
    while(index < in.length()) {
	if(firstChar) {
	    /* skip whitespace */
	    while(index < in.length() && in[index] == ' ') 
		index++;
	    if(in[index] == '"') {
		quotedElem = TRUE;
	        index++;
	    }
	}
	/* real first char */
	QString elem;
	if(quotedElem) {
	    while(index < in.length() && in[index] != '"') {
		/* check for escape character */
		if (in[index] == '\\') {
		    if (index++ < in.length()) {
			elem.append(in[index]);
			index++;
		    }
		} else {
		    elem.append(in[index]);
		    index++;
		}
	    }
	} else {
	    while(index < in.length() && in[index] != ',') {
		if (in[index] == '\\') {
		    if (index++ < in.length()) {
			elem.append(in[index]);
			index++;
		    }
		} else {
		    elem.append(in[index]);
		    index++;
		}
	    }
	}
	/* we have our current elem */
	out << elem.stripWhiteSpace();
	firstChar = TRUE;
	quotedElem = FALSE;
	/* skip till a , or end of line */
	while (index < in.length() && in[index] != ',') index++;
	if(index == in.length())
	    return out;
	else 
	    index++;
    }
    return out;
}

bool DBCsv::openSource(QIODevice *inDev)
{
    QTextStream tsIn(inDev);
    QString in = tsIn.readLine().stripWhiteSpace();
    QStringList keys;

    keys = readElem(in);

    QMap<int,int> keyIndexes;

    KeyList *keyR = new KeyList();
    QStringList::Iterator i = keys.begin();

    uint fileIndex = 0;
    while(i != keys.end()) {
	if ((*i).isEmpty())
	    keyIndexes.insert(fileIndex, keyR->addKey("Unamed", TVVariant::String));
	else 
	    keyIndexes.insert(fileIndex, keyR->addKey(*i, TVVariant::String));
	i++;
	fileIndex++;
    }
    dstore->setKeys(keyR);

    in = tsIn.readLine().stripWhiteSpace();
    while(!in.isNull()) {
	QStringList elems = readElem(in);

	i = elems.begin();
	fileIndex = 0;
	DataElem *current_data = new DataElem(dstore);
	while(i != elems.end()) {
	    if(!(*i).isEmpty()) {
		current_data->setField(keyIndexes[fileIndex], *i);
	    }
	    fileIndex++;
	    i++;
	}
	dstore->addItem(current_data);
	in = tsIn.readLine().stripWhiteSpace();
    }

    return TRUE;
}

bool DBCsv::saveSource(QIODevice *outDev)
{
    /* try not to use the escape character when possible. */
    int i;
    DataElem *elem;
    KeyList *k;
    QTextStream outstream(outDev);

    k = dstore->getKeys();
    KeyListIterator it(*k);
    while(it.current()) {
	if(!it.current()->delFlag()) {
	    QString name = it.current()->name();

	    name.replace(QRegExp("\\"), "\\\\");
	    name.replace(QRegExp("\""), "\\\"");
	    if(name.find(',') != -1) {
		name.prepend('\"');
		name.append('\"');
	    }

	    outstream << name;
	}
	++it;
	if(it.current()) 
	    outstream << ", ";
    }
    outstream << "\n";

    dstore->first();

    do { 
	elem = dstore->getCurrentData();
	if(!elem)
	    break;
	it.toFirst();
	while(it.current()) {
	    i = it.currentKey();
	    if (elem->hasValidValue(i)) {
		QString name = elem->toQString(i);

		name.replace(QRegExp("\\"), "\\\\");
		name.replace(QRegExp("\""), "\\\"");
		if(name.find(',') != -1) {
		    name.prepend('\"');
		    name.append('\"');
		}

		outstream << name;
	    }
	    ++it;
	    if(it.current()) 
		outstream << ", ";
	}
	outstream << "\n";
    } while (dstore->next());

    return TRUE;
}

