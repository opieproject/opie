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

/* A Class to parse an xml docment of the form
 * <database name="...">
 * <header>
 * <key name=key_name type=String>Displayed Name</key>
 * <key name=key2name>key2name</key>
 * <key name=key3name type=Date>Key 3</key>
 * <key name=key4name type=Int>key 4</key>
 * </header>
 * <record> 
 * <key_name>string</key_name>
 * <key4name>int</key4name>
 * <key2name>string</key2name>
 * </record>
 * <record>
 * ....
 * </record>
 * ....
 * </database>
 *
 * There is some room for improvment mostly around using better checking 
 * and the use of more advanced xml features.
 */

#ifndef __XMLSOURCE_H__
#define __XMLSOURCE_H__

#include <qxml.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qstack.h>
#include <qdict.h>
#include "datacache.h"
#include "common.h"


class DBXml : public DBAccess
{
public:
    /* create connection and either open or initialize */
    DBXml(DBStore *d);
    QString type();
    bool openSource(QIODevice *);
    bool saveSource(QIODevice *);
    /* does a db write */
    ~DBXml();
};


class DBXmlHandler : public QXmlDefaultHandler
{

public:
	DBXmlHandler(DBStore *ds);
	virtual ~DBXmlHandler();

	// return the error protocol if parsing failed
	QString errorProtocol();

	// overloaded handler functions
	bool startDocument();
	bool startElement(const QString& namespaceURI, const QString& localName,
					const QString& qName, const QXmlAttributes& atts);
	bool endElement(const QString& namespaceURI, const QString& localName,
					const QString& qName);
	bool characters(const QString& ch);

	QString errorString();

	bool warning(const QXmlParseException& exception);
	bool error(const QXmlParseException& exception);
	bool fatalError(const QXmlParseException& exception);

private:

	QStack<QString> stack;
    KeyList *current_keyrep;
    DataElem *current_data;
    TVVariant::KeyType last_key_type;

	QString errorProt;
	DBStore *data_store;

	enum State {
		StateInit,
        StateHeader,
        StateKey,
		StateDocument,
		StateRecord,
		StateField
	};

	State state;

    QDict<int> keyIndexList;
	int keyIndex;
    QString key;
};

#endif
