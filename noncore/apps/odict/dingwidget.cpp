/***************************************************************************
   application:             : ODict

   begin                    : December 2002
   copyright                : ( C ) 2002, 2003 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "dingwidget.h"

#include <qfile.h>
#include <qpe/config.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qtextbrowser.h>

DingWidget::DingWidget(QWidget *parent, QString word, QTextBrowser *browser) : QWidget(parent)
{
	QFile file( "/home/carsten/opie/opie/noncore/apps/odict/eng_ita.dic" );
	QStringList lines;

	if(  file.open(  IO_ReadOnly ) )
	{
		QTextStream stream(  &file );
		while ( !stream.eof() )
		{
			lines.append(  stream.readLine() );
		}
		file.close();
	}

	lines = lines.grep( word );

//X 	for( QStringList::Iterator it = lines.begin() ; it != lines.end() ; ++it )
//X 	{
//X 		qDebug( *it );
//X 	}
	browser->setText( parseInfo( lines ) );
}

QString DingWidget::parseInfo( QStringList &lines )
{
	QString parsed = 0;
	QStringList temp = lines;
	parsed = temp.first();
	return parsed;
}
