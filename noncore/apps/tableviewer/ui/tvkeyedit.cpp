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
#include "tvkeyedit.h"
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <stdlib.h>
#include <qpushbutton.h>

/* QList view item... ?? that can store and update the values that I will
 * be changing */

class TVKEListViewItem : public QListViewItem
{
public:
    TVKEListViewItem(QString n, TVVariant::KeyType kt, int p, QListView *parent) : 
        QListViewItem(parent)
    {
        name = n;
        keyType = kt;
        position = p;
    }

    QString text(int i) const
    {
        if(i) {
            return TVVariant::typeToName(keyType);
        }
        return name;
    }

    /* always sort by key index, ignore i */
    QString key(int, bool) const
    {
        return QString().sprintf("%08d", position);
    }

    void setText(int i, const QString &)
    {
        ;
    }

    QString getName() const
    {
        return name;
    }

    void setName(QString n) 
    {
        name = n;
        repaint();
    }

    TVVariant::KeyType getKeyType() const
    {
        return keyType;
    }

    void setKeyType(TVVariant::KeyType k) 
    {
        keyType = k;
        repaint();
    }

    inline int getPos() const
    {
        return position;
    }

private:
    QString name;
    TVVariant::KeyType keyType;
    int position;
};

TVKeyEdit::TVKeyEdit(TableState *t, QWidget* parent, const char *name, 
	WFlags fl) : TVKeyEdit_gen(parent, name, true, fl)
{
    int i;
    ts = t;

    if(!ts) return;
    if(!ts->kRep) return;

    working_state = *ts->kRep;

    i = 1;
    keyTypeEdit->insertItem(TVVariant::typeToName((TVVariant::KeyType)i));
    i++;
    keyTypeEdit->insertItem(TVVariant::typeToName((TVVariant::KeyType)i));
    i++;
    keyTypeEdit->insertItem(TVVariant::typeToName((TVVariant::KeyType)i));
    i++;
    keyTypeEdit->insertItem(TVVariant::typeToName((TVVariant::KeyType)i));

    KeyListIterator it(*ts->kRep);
    while(it.current()) {
		if(t->kRep->validIndex(it.currentKey())) {
			new TVKEListViewItem(it.current()->name(),
					it.current()->type(),
					it.currentKey(),
					display);
		}
		++it;
	}
    num_keys = ts->kRep->getNumFields();
	if(display->childCount() > 0) {
		display->setCurrentItem(display->firstChild());
		setTerm(display->currentItem());
	} else {
		deleteKeyButton->setEnabled(FALSE);
		clearKeysButton->setEnabled(FALSE);
		keyNameEdit->setEnabled(FALSE);
		keyTypeEdit->setEnabled(FALSE);
	}

    display->setSorting(0);
#ifdef Q_WS_QWS
    showMaximized();
#endif
}

/*!
    Destroys the TVKeyEdit widget 
*/
TVKeyEdit::~TVKeyEdit()
{
}

/* SLOTS */
void TVKeyEdit::newTerm() 
{
    /* new item, make current Item */
    int i;

    i = working_state.addKey("<New Key>", TVVariant::String);
    //working_state.setNewFlag(i, TRUE);
	TVKEListViewItem *nItem = new TVKEListViewItem("<New Key>", 
                                                 TVVariant::String, 
                                                 i, 
                                                 display);
    display->setCurrentItem(nItem);
	setTerm(nItem);

    num_keys++;
	if(display->childCount() == 1) {
		deleteKeyButton->setEnabled(TRUE);
		clearKeysButton->setEnabled(TRUE);
		keyNameEdit->setEnabled(TRUE);
		keyTypeEdit->setEnabled(TRUE);
	}
}

void TVKeyEdit::updateTerm(const QString &newName) 
{
    /* TODO if name matches a deleted term, prompt for 
       renewing old data instead */
    TVKEListViewItem *i = (TVKEListViewItem *)display->currentItem();
    if(i) {
        i->setName(newName);
        working_state.setKeyName(i->getPos(), newName);
    }
}

void TVKeyEdit::updateTerm(int t) 
{
    /* t is an index to a combo in a menu, NOT a type */
    t++; /* menu counts from 0, types count from 1 */
    TVKEListViewItem *i = (TVKEListViewItem *)display->currentItem();
    if (i) {
	    i->setKeyType((TVVariant::KeyType)t);
		working_state.setKeyType(i->getPos(), (TVVariant::KeyType)t);
    }
}

/* deletes current term 
 * really just marks key as deleted so is now invalid.
 * the actual delete will happen when data is 'cleaned'
 * or when file is saved.
 */
   
void TVKeyEdit::deleteTerm() 
{
    TVKEListViewItem *i = (TVKEListViewItem *)display->currentItem();
    if (i) {
        working_state.setDeleteFlag(i->getPos(), TRUE);
        delete i;
    }
	if(!display->childCount()) {
		/* disable the delete and clear buttons, etc */
		deleteKeyButton->setEnabled(FALSE);
		clearKeysButton->setEnabled(FALSE);
		keyNameEdit->setEnabled(FALSE);
		keyTypeEdit->setEnabled(FALSE);
	}
}

/* clears all terminations */
void TVKeyEdit::clearTerms() 
{
    /* should pop up a warning */
    if (QMessageBox::warning(this, "Delete all keys", 
                         "Are you sure you want to\ndelete all the keys?",
                         "Yes", "No") == 0) 
    {
        while(display->currentItem())
            deleteTerm();
    }
}

void TVKeyEdit::setTerm(QListViewItem *target)
{
    /* need to update the widgets to show keys values */
    keyNameEdit->setText(((TVKEListViewItem *)target)->getName());
	int t = (int)(((TVKEListViewItem *)target)->getKeyType());
	t--;
	keyTypeEdit->setCurrentItem(t);
}

KeyList* TVKeyEdit::openEditKeysDialog(TableState *t, QWidget *parent = 0)
{
    if(!t)
        return 0;
    if(!t->kRep)
        return 0;

    TVKeyEdit *dlg = new TVKeyEdit(t, parent);

    if ((dlg->exec() == QDialog::Accepted) && 
	    (dlg->working_state != *t->kRep)) 
	{
		return (new KeyList(dlg->working_state));
    }
    return 0;
}
