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
#include "doclnkitem.h"

#include <qpe/applnk.h>

DocLnkItem::DocLnkItem(OListViewItem* parent, DocLnk *app)
	: ResultItem(parent)
{
	_doc = app;
	setText(0, _doc->name() );
}


DocLnkItem::~DocLnkItem()
{
}


QString DocLnkItem::toRichText()
{
    QString text;
    text += "<b><h3>";
    text += _doc->name();
    text += "</b></h3><br>";
    text += _doc->comment();
    text += "<br><br>`";
    text += _doc->exec();
    text += "`";
    return text;
}

void DocLnkItem::editItem()
{
	_doc->execute();
}

void DocLnkItem::showItem()
{
/* 	QCopEnvelope e("QPE/Application/addressbook", "edit(int)");
	e << _contact->uid();*/
   ResultItem::showItem();
}

