/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qstringlist.h>
#include <qstring.h>

struct BroswerContent
{ 
	QString top;
	QString bottom;
};

class DingWidget
{
    public:
		DingWidget();

		BroswerContent setText( QString );
		QStringList lines;
		void setCaseSensitive( bool );
		void setCompleteWord( bool );
		void loadDict( QString );
		QString loadedDict();
		void setQueryWord( QString );
		void setDict( QString );

	private:
		BroswerContent parseInfo();

		BroswerContent s_strings;
		bool isCompleteWord;
		bool isCaseSensitive;

		QString dictName;
		
		QString search_word;
		void loadValues();
		QString queryword;
		QString methodname;
		QString trenner;
};
