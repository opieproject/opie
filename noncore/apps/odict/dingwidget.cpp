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
#include <qtextstream.h>

DingWidget::DingWidget( )
{
	methodname = QString::null;
	trenner = QString::null;
	lines = 0L;
}

void DingWidget::loadDict( QString name )
{
	lines.clear(); //as we will load a new list we have to
	               //remove the old one
	
	Config cfg(  "odict" );
	cfg.setGroup( "Method_" + name );
	QFile file( cfg.readEntry( "file" )  );

	if(  file.open(  IO_ReadOnly ) )
	{
		QTextStream stream(  &file );
		while ( !stream.eof() )
		{
			lines.append(  stream.readLine() );
		}
		file.close();
	}

	setDict( name );

	loadValues();
}

QString DingWidget::loadedDict() const
{ 
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

void DingWidget::setQueryWord( QString qword )
{
	queryword = qword;
}


void DingWidget::loadValues()
{
	if ( !methodname ) return;
	Config cfg(  "odict" );
	cfg.setGroup( "Method_" + methodname );
	trenner = cfg.readEntry( "Seperator" );
	
	lang1_name = cfg.readEntry( "Lang1" );
	lang2_name = cfg.readEntry( "Lang2" );
}

BroswerContent DingWidget::setText( QString word )
{
	queryword = word;
	return parseInfo();
}


BroswerContent DingWidget::parseInfo()
{
	QStringList search = lines.grep( queryword , isCaseSensitive );

	QString current;
	QString left;
	QString right;
	QRegExp reg_div( trenner );
	QRegExp reg_word( queryword );
	reg_word.setCaseSensitive( isCaseSensitive );
	QStringList toplist, bottomlist;
	QString substitute = "<strong>"+queryword+"</strong>";

	for( QStringList::Iterator it = search.begin() ; it != search.end() ; ++it )
	{
		current = *it;
		left = current.left( current.find( trenner ) );
	
		right = current.right( current.length() - current.find(trenner) - trenner.length() );

 		if ( left.contains( queryword , isCaseSensitive ) )
		{
			left.replace( queryword, substitute );
			left = left + " --> " + right;
			toplist.append( left );
		}
		else if( right.contains( queryword , isCaseSensitive ) )
		{
			right.replace( queryword, substitute );
			right = right + " --> " + left;
			bottomlist.append( right );
		}
	}

	s_strings.top = toplist.join( "<br>" );
	s_strings.bottom = bottomlist.join( "<br>" );
	
	return s_strings;
}

