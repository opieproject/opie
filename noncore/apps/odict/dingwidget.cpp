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
#include <stdlib.h> // for getenv

DingWidget::DingWidget(QWidget *parent, QString word, QTextBrowser *browser_top, QTextBrowser *browser_bottom) : QWidget(parent)
{
	queryword = word;
  	QString opie_dir = getenv("OPIEDIR");
	QFile file( opie_dir+"/noncore/apps/odict/eng_ita.dic" );
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

	lines = lines.grep( queryword );

	QString top, bottom;

	parseInfo( lines, top , bottom );
	browser_top->setText( top );
	browser_bottom->setText( bottom );
}

void DingWidget::parseInfo( QStringList &lines, QString &top, QString &bottom )
{

 	QString current;
 	QString left;
 	QString right;
	QRegExp reg_div( "\\" );
	QRegExp reg_word( queryword );
	//QString substitute = "<font color=red>"+queryword+"</font>";
	QString substitute = "<a href=''>"+queryword+"</a>";
	QStringList toplist, bottomlist;
	for( QStringList::Iterator it = lines.begin() ; it != lines.end() ; ++it )
	{
		current = *it;
		left = current.left( current.find(reg_div) );
 		right = current.right( current.length() - current.find(reg_div) - 1 );
 		if ( left.contains( reg_word ) )
		{
		  left.replace( queryword, substitute );
 		  toplist.append( "<tr><td>" + left + "</td><td>" + right + "</td></tr>" );
		}
		else
		{
		  right.replace( reg_word, substitute );
 		  bottomlist.append( "<tr><td>" + right + "</td><td>" + left + "</td></tr>" );
		}
	}
		
	//thats it, the lists are rendered. Lets put them in one string

	bottom = "<html><table><tbody>"+bottomlist.join( "<br>" )+"</tbody></table></html>";
	top =  "<html><table><tbody>"+toplist.join( "<br>" )+"</tbody></table></html>";
}
