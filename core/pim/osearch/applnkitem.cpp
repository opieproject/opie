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
    text += "<b><h3>";
    text += _app->name();
    text += "</b></h3><br>";
    text += _app->comment();
    text += "<br><br>`";
    text += _app->exec();
    text += "`";
    return text;
}

void AppLnkItem::editItem()
{
	_app->execute();
}

void AppLnkItem::showItem()
{
/* 	QCopEnvelope e("QPE/Application/addressbook", "edit(int)");
	e << _contact->uid();*/
   ResultItem::showItem();
}

