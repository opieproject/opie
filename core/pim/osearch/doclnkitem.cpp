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
#include <qfile.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <qpe/qcopenvelope_qws.h>

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
    text += "<b><h3>" + _doc->name() + "</b></h3><br>";
    text += _doc->comment() + "<br>";
    text += "File: " + _doc->file() + "<br>";
    text += "Mimetype: " + _doc->type() + "<br>";
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
	qDebug("action %i",act);
	if (!_doc->isValid()) qDebug("INVALID");
	if (act == 0) _doc->execute();
	else if (act == 1){
		QFileInfo file( _doc->file() );
		qDebug("opening %s in filemanager", file.dirPath().latin1());
 		QCopEnvelope e("QPE/Application/advancedfm", "setDocument(QString)");
 		e << file.dirPath();
// 		QCopEnvelope e("QPE/Application/advancedfm", "setDocument(QString)");
 //		e << _doc->file();
	}
}

QIntDict<QString> DocLnkItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("open with ") + _doc->exec() ) );
	result.insert( 1, new QString( QObject::tr("open in filemanager") ) );
	return result;
}
