/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#ifndef SORTDLG_H
#define SORTDLG_H

#include "sheet.h"

/* QT */
#include <qdialog.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qvbuttongroup.h>

class SortDialog: public QDialog
{
    Q_OBJECT

    // QT objects
    QBoxLayout *box;
    QTabWidget *tabs;
    QWidget *widgetSort, *widgetOptions;
    QVButtonGroup *groupOrderA, *groupOrderB, *groupOrderC, *groupDirection;
    QCheckBox *checkCase;
    QComboBox *comboFieldA, *comboFieldB, *comboFieldC;

    // Other objects & variables
    int row1, col1, row2, col2, direction;
    Sheet *sheet;

    // Private functions
    QVButtonGroup *createOrderButtons(int y);
    QComboBox *createFieldCombo(const QString &caption, int y);
    void fillFieldCombo(QComboBox *combo);
    int compareItems(QTableItem *item1, QTableItem *item2, int descending=0, bool caseSensitive=TRUE);

private slots:
    void directionChanged(int id);

public:
    SortDialog(QWidget *parent=0);
    ~SortDialog();

    int exec(Sheet *s);
};

#endif
