//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "applnkitem.h"

#include <qpe/applnk.h>

AppLnkItem::AppLnkItem(OListViewItem* parent, AppLnk *app)
	: ResultItem(parent)
{
	_app = app;
	setText(0, _app->name() );
}


AppLnkItem::~AppLnkItem()
{
}


QString AppLnkItem::toRichText()
{
    QString text;
    text += "<b><h3>" + _app->name() + "</b></h3><br>";
    text += _app->comment() + "<br>";
    text += "<br>`" + _app->exec() + "`<br>";
    return text;
}

void AppLnkItem::action( int act )
{
	if (!_app->isValid()) qDebug("INVALID");
	if (act == 0) _app->execute();
}

QIntDict<QString> AppLnkItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("execute") ) );
	return result;
}
