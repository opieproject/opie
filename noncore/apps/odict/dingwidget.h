/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

class QString;
class QTextBrowser;
class QStringList;

#include <qstringlist.h>

#include <qstring.h>

class DingWidget
{
    public:
		DingWidget(QString word = 0, QTextBrowser* = 0, QTextBrowser* =0, QString activated_name=0);

		void setText();
		QStringList lines;
	private:
		QString search_word;
		void parseInfo( QStringList& , QString&, QString& );
		QString queryword;
		void loadValues();
		QString methodname;
		QString trenner;
		
		QTextBrowser *topbrowser, *bottombrowser;
};
