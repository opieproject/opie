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

#include <opie2/odebug.h>

#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>

#include <qtextstream.h>
#include <qfileinfo.h>

DocLnkItem::DocLnkItem(OListViewItem* parent, DocLnk *app)
	: ResultItem(parent)
{
	_doc = app;
	setText(0, _doc->name() );
	setPixmap(0, _doc->pixmap() );
}

DocLnkItem::~DocLnkItem()
{
}

QString DocLnkItem::toRichText()
{
    QString text;
    text += "<b><h3>" + _doc->name() + "</b></h3><br>";
    text += _doc->comment() + "<br>";
    text += QObject::tr("File: ") + _doc->file() + "<br>";
    text += QObject::tr("Link: ") + _doc->linkFile() + "<br>";
    text += QObject::tr("Mimetype: ") + _doc->type() + "<br>";
    if ( _doc->type().contains( "text" ) ){
	text += "<br><br><hr><br>";
  	QFile f(_doc->file());
  	if ( f.open(IO_ReadOnly) ) {
       		QTextStream t( &f );
        	while ( !t.eof() )
       	    		text += t.readLine() + "<br>";
        }
  	f.close();
    }
   /* text += "<br><br>`";
    text += _doc->exec();
    text += "`";*/
    QStringList list = _doc->mimeTypes();
   int i = 0;
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            text += QString::number( i++) + " - " + *it ;
        }
    return text;
}

void DocLnkItem::action( int act )
{
	odebug << "action" << act << oendl;
	if (!_doc->isValid()) odebug << "INVALID" << oendl;
	if (act == 0) _doc->execute();
	else if (act == 1){
 		QCopEnvelope e("QPE/Application/advancedfm", "setDocument(QString)");
		e << _doc->file();
	}
}

QIntDict<QString> DocLnkItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("open with ") + _doc->exec() ) );
	result.insert( 1, new QString( QObject::tr("open in filemanager") ) );
	return result;
}
