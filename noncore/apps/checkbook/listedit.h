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

#ifndef LISTEDIT_H
#define LISTEDIT_H

#include <qwidget.h>
#include "tabledef.h"
class QListView;
class QLineEdit;
class QListViewItem;
class QPoint;
class QWidgetStack;
class QComboBox;

class ListEdit : public QWidget, public TableDef
{
	Q_OBJECT

	public:
		ListEdit( QWidget *, const char *sName);
		virtual ~ListEdit();

		QListView *_typeTable;
        QLineEdit *_typeEdit;
        QWidgetStack *_stack;
        QComboBox *_box;
        QListViewItem *_currentItem;
        int _currentColumn;
        
        // resolves dups and empty entries
        void fixTypes();
        void fixTypes(int iColumn);

        // stores content in string list
        void storeInList(QStringList &lst);

        // adds a column definition
        virtual void addColumnDef(ColumnDef *pDef);

        // adds data to table
        void addData(QStringList &lst);


    public slots:
        void slotClick(QListViewItem *, const QPoint &pnt, int col);
        void slotEditChanged(const QString &);
        void slotAdd();
        void slotDel();
        void slotActivated(const QString &);
};

#endif
