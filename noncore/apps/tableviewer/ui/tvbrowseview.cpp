/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/ 
#include "tvbrowseview.h"
#include "browsekeyentry.h"
#include <qtoolbutton.h>
#include <qtextview.h>
#include <qtextbrowser.h>
#include <qlayout.h>

/*!
    \class TVBrowseView
    \brief The widget describing how to draw the browse view user interface

    This widget allows for the user to browse through the table, one element
    at a time, or search on a single key.  Its main goal is to show a 
    single element in a readable format and make it easy for the user to 
    rapidly find specific elements in the table.
*/

/*!
    Constructs a new TVBrowseView widget 
*/
TVBrowseView::TVBrowseView(TableState *t, QWidget* parent = 0, const char *name = 0, 
	WFlags fl =0)
{
    if (!name) 
	setName("BrowseView");

    setSizePolicy(QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, 0, 0, sizePolicy().hasHeightForWidth() ) );
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    textViewDisplay = new QTextBrowser(this, "textViewDisplay");
    vlayout->addWidget( textViewDisplay );

    keyEntry = new TVBrowseKeyEntry(this, "keyEntry");
    vlayout->addWidget( keyEntry );

    /* connect the signals down */

    connect(keyEntry, SIGNAL(searchOnKey(int, TVVariant)),
        this, SIGNAL(searchOnKey(int, TVVariant)));
    connect(keyEntry, SIGNAL(sortChanged(int)),
        this, SIGNAL(sortChanged(int)));

    ts = t;
    keyEntry->setTableState(t); 
}

/*!
    Destroys the TVBrowseView widget 
*/
TVBrowseView::~TVBrowseView()
{
}

void TVBrowseView::rebuildData()
{
    if(!ts)
        return;
    if(!ts->current_elem) {
		/* also disable buttons */
		textViewDisplay->setText("");
        return;
	}

    setDisplayText(ts->current_elem);
}

/* Reset to initial state */
void TVBrowseView::reset()
{
    textViewDisplay->setText("");
    keyEntry->reset();
}

/*!
    sets the data element to be displayed to element 
*/
void TVBrowseView::setDisplayText(const DataElem *element)
{
    QString rep = "";

    KeyListIterator it(*ts->kRep);

    while (it.current()) {
        if (element->hasValidValue(it.currentKey())) {
            if(it.currentKey() == ts->current_column) {
                rep += "<A name=\"ckey\"></A><B><FONT COLOR=#FF0000>" 
                    + it.current()->name()
                    + ":</FONT></B> ";
            } else {
                rep += "<B>" + it.current()->name() + ":</B> ";
            }
            rep += element->toQString(it.currentKey()) + "<BR>";
        }
        ++it;
    }

    textViewDisplay->setText(rep);
    textViewDisplay->scrollToAnchor("ckey");
}

void TVBrowseView::rebuildKeys() 
{
    keyEntry->rebuildKeys();
}
