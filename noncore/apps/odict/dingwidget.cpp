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


DingWidget::DingWidget( )
{
	methodname = QString::null;
	trenner = QString::null;
	lines = 0L;
}

void DingWidget::loadDict( QString name )
{
	qDebug( "MUSS ich wirklich aufgerufen werder? schreit loadDict" );
	qDebug( "Starte mit dem loadedDict(...)" );
	dictName = name;
	qDebug( "bin in loadedDict() und lade das Dict:" );
	qDebug( dictName );
  	QString opie_dir = getenv("OPIEDIR");
	QFile file( opie_dir+"/noncore/apps/odict/eng_ita.dic" );
	//FIXME:this should of course be not hardcoded ;)
	
	if(  file.open(  IO_ReadOnly ) )
	{
		QTextStream stream(  &file );
		while ( !stream.eof() )
		{
			lines.append(  stream.readLine() );
		}
		file.close();
	}
	qDebug( "loadedDict(...) ist beended" );
	loadValues();
}

QString DingWidget::loadedDict()
{ 
	qDebug( dictName );
	qDebug( "^ ^ ^ ^ ^ ^ ^ war der dictName" );
	return dictName;
}

void DingWidget::setCaseSensitive( bool caseS )
{
	isCaseSensitive = caseS;
}

void DingWidget::setDict( QString dict )
{
	methodname = dict;
}

void DingWidget::setCompleteWord( bool cword )
{
	isCompleteWord = cword;
}

void DingWidget::setQueryWord( QString qword )
{
	queryword = qword;
}

BroswerContent DingWidget::setText( QString word )
{
	queryword = word;
	qDebug( queryword );
	qDebug( "^ ^ ^ ^ ^ ^ ^ war das gesuchte Word");
	return parseInfo();
}

void DingWidget::loadValues()
{
	Config cfg(  "odict" );
	cfg.setGroup( "Method_" + methodname );
	trenner = cfg.readEntry( "Seperator" );
}

BroswerContent DingWidget::parseInfo()
{
	if ( isCompleteWord ) queryword = " " + queryword + " ";
	QStringList search = lines.grep( queryword , isCaseSensitive );

 	QString current;
 	QString left;
 	QString right;
	QRegExp reg_div( trenner );
	QRegExp reg_word( queryword );
	reg_word.setCaseSensitive( isCaseSensitive );
	QStringList toplist, bottomlist;
	QString substitute = "<strong>"+queryword+"</strong>";

	/*             Dieser Block ist von Patrik. Ich versuche einen neuen
	 *             Ansatz. Zum einen ist HTML scheiﬂe an dieser Stelle und 
	 *             zum andern funktioniert der Code nicht so wie er sollte.
	QString substitute = "<a href=''>"+queryword+"</a>";
	
	QString html_header      = "<html><table>";
	QString html_footer      = "</table></html>";
	QString html_table_left  = "<tr><td width='50'>";
	QString html_table_sep   = "</td><td>";
	QString html_table_right = "</td></tr>";
	 
	for( QStringList::Iterator it = search.begin() ; it != search.end() ; ++it )
	{
		current = *it;
		left = current.left( current.find(reg_div) );
 		right = current.right( current.length() - current.find(reg_div) - 1 );
 		if ( left.contains( reg_word ) )
		{
		  left.replace( queryword, substitute );
 		  toplist.append( html_table_left + left + html_table_sep + right + html_table_right );
		}
		else
		{
		  right.replace( reg_word, substitute );
 		  bottomlist.append( html_table_left + right + html_table_sep + left + html_table_right );
		}
	}
		
	//thats it, the lists are rendered. Lets put them in one string
	s_strings.bottom = html_header + bottomlist.join( "<br>" ) + html_footer;
	s_strings.top    = html_header + toplist.join( "<br>" )    + html_footer;
	*/

	for( QStringList::Iterator it = search.begin() ; it != search.end() ; ++it )
	{
		current = *it;
		left = current.left( current.find( trenner ) );
 		
		right = current.right( current.length() - current.find(trenner) -2 );

 		if ( left.contains( queryword , isCaseSensitive ) )
		{
			left.replace( queryword, substitute );
			left = left + "-->" + right;
			toplist.append( left );
		}
		else
		{
			right.replace( queryword, substitute );
			left = right + "-->" + left;
			bottomlist.append( right );
		}
	}

	s_strings.bottom = bottomlist.join( "\n" );
	s_strings.top = toplist.join( "\n" );

	return s_strings;
}
