/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qwidget.h>

class QString;
class QTextBrowser;
class QString;
class QStringList;

class DingWidget : public QWidget
{
	Q_OBJECT
	
    public:
		DingWidget(QWidget *parent = 0, QString word = 0, QTextBrowser* = 0, QTextBrowser* =0);

	private:
		void parseInfo( QStringList& , QString&, QString& );
};
