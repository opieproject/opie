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

#include <opie2/odebug.h>

#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>

AppLnkItem::AppLnkItem(OListViewItem* parent, AppLnk *app)
	: ResultItem(parent)
{
	_app = app;
	setText(0, _app->name() );
	setPixmap(0, _app->pixmap() );
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
    text += "<br>`" + _app->file() + "`<br>";
    text += "<br>`" + _app->linkFile() + "`<br>";
    return text;
}

void AppLnkItem::action( int act )
{
	if (!_app->isValid()) Opie::Core::odebug << "INVALID" << oendl;
	if (act == 0) _app->execute();
	else if (act == 1){
 		QCopEnvelope e("QPE/Application/advancedfm", "setDocument(QString)");
		e << _app->linkFile();
	}
}

QIntDict<QString> AppLnkItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("execute") ) );
	result.insert( 1, new QString( QObject::tr("open in filemanager") ) );
	return result;
}
