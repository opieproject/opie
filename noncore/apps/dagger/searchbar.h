/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <qtoolbar.h>

class QAction;
class QComboBox;
class QLineEdit;
class TextWidget;

class SearchBar : public QToolBar
{
    Q_OBJECT

public:
    SearchBar( QMainWindow *parent = 0x0 );

    void setCurrModule( TextWidget *currText );

private:
    TextWidget *m_currText;  // Pointer to current text

    QLineEdit *m_searchText; // Edit box to enter text to search for
    QAction   *m_actionFind; // Button to press to initiate search
    QAction   *m_actionPrev; // Button to view previous search result
    QComboBox *m_resultList; // List of results
    QAction   *m_actionNext; // Button to view next search result

private slots:
    void slotTextChanged( const QString &newText );
    void slotFind();
    void slotPrev();
    void slotNext();
    void slotCloseBtn();

signals:
    void sigResultClicked( const QString &key );
};

#endif
