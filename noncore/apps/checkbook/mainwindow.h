/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qpixmap.h>

class CBInfo;
class CBInfoList;
class QAction;
class QListView;
class QString;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
		~MainWindow();
		static QString appName() { return QString::fromLatin1("checkbook"); };

	private:
		QListView *cbList;
		QString    cbDir;
		QAction   *actionOpen;
		QAction   *actionDelete;

		QString  currencySymbol;
		bool     showLocks;
		bool     showBalances;
		int      posName;

		CBInfoList *checkbooks;
		QString     tempFilename;
		QPixmap     lockIcon;
		QPixmap     nullIcon;

		void buildList();
		void addCheckbook( CBInfo * );
		void buildFilename( const QString & );

	private slots:
		void slotNew();
		void slotEdit();
		void slotDelete();
		void slotConfigure();
};

#endif
