/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "menubutton.h"
#include <qpopupmenu.h>

/*!
  \class MenuButton menubutton.h
  \brief The MenuButton class is a pushbutton with a menu.

  Similar in function to a QComboBox.
*/

/*!
  \fn void MenuButton::selected(int index)

  This signal is emitted when the item at \a index is selected.
*/

/*!
  \fn void MenuButton::selected(const QString& text)

  This signal is emitted when the item with the given \a text is selected.
*/


/*!
  Constructs a MenuButton with the given \a items added (see insertItems()).
  The standard \a parent an \a name arguments are passed to the base class.
*/
MenuButton::MenuButton( const QStringList& items, QWidget* parent, const char* name) :
    QPushButton(parent,name)
{
    init();
    insertItems(items);
}

/*!
  Constructs an empty MenuButton.
  The standard \a parent an \a name arguments are passed to the base class.

  \sa insertItems()
*/
MenuButton::MenuButton( QWidget* parent, const char* name) :
    QPushButton(parent,name)
{
    init();
}

void MenuButton::init()
{
    setAutoDefault(FALSE);
    pop = new QPopupMenu(this);
    nitems=0;
    connect(pop, SIGNAL(activated(int)), this, SLOT(select(int)));
    setPopup(pop);
    //setPopupDelay(0);
}

/*!
  Removes all items from the button and menu.
*/
void MenuButton::clear()
{
    delete pop;
    init();
}

/*!
  Inserts string \a items into the menu. The string "--" represents
  a separator.
*/
void MenuButton::insertItems( const QStringList& items )
{
    QStringList::ConstIterator it=items.begin();
    for (; it!=items.end(); ++it) {
	if ( (*it) == "--" )
	    insertSeparator();
	else
	    insertItem(*it);
    }
}

/*!
  Inserts an \a icon and \a text into the menu.
*/
void MenuButton::insertItem( const QIconSet& icon, const QString& text )
{
    pop->insertItem(icon, text, nitems++);
    if ( nitems==1 ) select(0);
}

/*!
  Inserts \a text into the menu.
*/
void MenuButton::insertItem( const QString& text )
{
    pop->insertItem(text, nitems++);
    if ( nitems==1 ) select(0);
}

/*!
  Inserts a visual separator into the menu.
*/
void MenuButton::insertSeparator()
{
    pop->insertSeparator();
}

/*!
  Selects the items with text \a s.
*/
void MenuButton::select(const QString& s)
{
    for (int i=0; i<nitems; i++) {
	if ( pop->text(i) == s ) {
	    select(i);
	    break;
	}
    }
}

/*!
  Selects the item at index \a s.
*/
void MenuButton::select(int s)
{
    cur = s;
    updateLabel();
    if ( pop->iconSet(cur) )
	setIconSet(*pop->iconSet(cur));
    emit selected(cur);
    emit selected(currentText());
}

/*!
  Returns the index of the current item.
*/
int MenuButton::currentItem() const
{
    return cur;
}

/*!
  Returns the text of the current item.
*/
QString MenuButton::currentText() const
{
    return pop->text(cur);
}

/*!
  Sets the label. If \a label is empty, the
  current item text is displayed, otherwise \a label should contain
  "%1", which will be replaced by the current item text.
*/
void MenuButton::setLabel(const QString& label)
{
    lab = label;
    updateLabel();
}

void MenuButton::updateLabel()
{
    QString t = pop->text(cur);
    if ( !lab.isEmpty() )
	t = lab.arg(t);
    setText(t);
}

