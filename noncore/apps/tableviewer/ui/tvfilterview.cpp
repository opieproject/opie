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
#include "tvfilterview.h"
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qlabel.h>

TVFilterView::TVFilterView(TableState *t, QWidget* parent = 0, 
                           const char *name = 0, WFlags fl =0) : QDialog(parent, name, TRUE, fl)
{
    if ( !name ) 
	setName( "Filter View" );

    QVBoxLayout *vlayout = new QVBoxLayout(this);

    display = new QListView(this, "display");
    display->addColumn("Key");
    display->addColumn("Constraint");
    display->addColumn("Value");
    display->header()->setClickEnabled(FALSE);
    display->header()->setResizeEnabled(FALSE);

    vlayout->addWidget(display);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    newFilterButton = new QPushButton(this, "new Filter");
    newFilterButton->setMaximumSize(QSize(50, 32767));
    newFilterButton->setText("New");
    hlayout->addWidget(newFilterButton);

    deleteFilterButton = new QPushButton(this, "delete Filter");
    deleteFilterButton->setMaximumSize(QSize(50, 32767));
    deleteFilterButton->setText("Delete");
    hlayout->addWidget(deleteFilterButton);

    clearFilterButton = new QPushButton(this, "delete Filter");
    clearFilterButton->setMaximumSize(QSize(60, 32767));
    clearFilterButton->setText("Clear All");
    hlayout->addWidget(clearFilterButton);

    vlayout->addLayout(hlayout);

    QHBoxLayout *hlayout2 = new QHBoxLayout;

    keyNameCombo = new QComboBox(FALSE, this, "key name");
    keyNameCombo->setEnabled(FALSE);
    hlayout2->addWidget(keyNameCombo);

    QLabel *label = new QLabel(this);
    label->setText("has value");
    hlayout2->addWidget(label);

    keyEntry = new TVFilterKeyEntry(this, "key entry");
    keyEntry->setEnabled(FALSE);

    vlayout->addLayout(hlayout2);
    vlayout->addWidget(keyEntry);

    connect(newFilterButton, SIGNAL( clicked() ), this, SLOT( newTerm() ));
    connect(deleteFilterButton, SIGNAL( clicked() ), this, SLOT( deleteTerm()));
    connect(clearFilterButton, SIGNAL( clicked() ), this, SLOT( clearTerms()));

    connect(keyEntry, SIGNAL(valueChanged()), this, SLOT( updateTerm() ));
    connect(keyNameCombo, SIGNAL(activated(int)), this, SLOT( updateTerm() ));

    connect(display, SIGNAL(selectionChanged(QListViewItem*)), this, 
	    SLOT(setTerm(QListViewItem *)));

    ts = t;
    current = 0;
    terms.setAutoDelete(true);
    do_filter = false;

#ifdef Q_WS_QWS
    showMaximized();
#endif
}

/*!
    Destroys the TVFilterView widget 
*/
TVFilterView::~TVFilterView()
{
}

void TVFilterView::rebuildData()
{
}

void TVFilterView::reset()
{
    keyNameCombo->clear();
    keyIds.clear();
}

void TVFilterView::rebuildKeys() 
{
    int i;

    if (!ts) return;
    if(!ts->kRep) return;
    keyEntry->setTableState(ts);
    
    /* set up the list of keys that can be compared on */
    keyNameCombo->clear();
    KeyListIterator it(*ts->kRep);

    i = 0;
    while(it.current()) {
	    if(ts->kRep->validIndex(it.currentKey())) {
			keyNameCombo->insertItem(it.current()->name());
			keyIds.insert(i, it.currentKey());
			++i;
		}
		++it;
    }
}

bool TVFilterView::passesFilter(DataElem *d) {
    if (!filterActive()) return true;


    FilterTerm *t;
    
    for (t = terms.first(); t != 0; t = terms.next() ) {
        /* check against filter */
        switch(t->ct) {
            case ct_less:
                if (!d->lessThan(t->keyIndex, t->value))
                    return false;
                break;
            case ct_more:
                if (!d->moreThan(t->keyIndex, t->value))
                    return false;
                break;
            case ct_equal:
                if (!d->equalTo(t->keyIndex, t->value))
                    return false;
                break;
            case ct_contains:
                if (!d->contains(t->keyIndex, t->value))
                    return false;
                break;
            case ct_startswith:
                if (!d->startsWith(t->keyIndex, t->value))
                    return false;
                break;
            case ct_endswith:
                if (!d->endsWith(t->keyIndex, t->value))
                    return false;
                break;
            default:
                qWarning("TVFilterView::passesFilter() "
                         "unrecognized filter type");
                return false;
        }
    }
    return true;
}

bool TVFilterView::filterActive() const
{
    /* when button operated, also check the do_filter value
    return do_filter;
    */
    if (terms.isEmpty())
        return false;
    return true;
}

/* SLOTS */
void TVFilterView::newTerm() 
{
    if (!ts) return;

    FilterTerm *term = new FilterTerm;
    current = term;

    term->view = 0;

    updateTerm();

    display->setSelected(term->view, true);
    terms.append(term);

    keyEntry->setEnabled(true);
    keyNameCombo->setEnabled(true);
}

void TVFilterView::updateTerm() 
{
    FilterTerm *term;
    /* Read the widget values (keyname, compare type, value)
     * and build the lists */
    if (!ts) return;
    if (!current) return;

    QString keyString;
    QString cmpString;
    QString vString;

    term = current;

    /* create new list item, set initial values, enable widgets */
    term->keyIndex = keyIds[keyNameCombo->currentItem()];
    keyEntry->setKey(term->keyIndex); /* so the next two items make sense */
    term->ct = keyEntry->getCompareType(),
    term->value = keyEntry->getCompareValue();
   
    keyString = keyNameCombo->currentText();

    switch(term->ct) {
        case ct_less:
            cmpString = " less than ";
            break;
        case ct_more:
            cmpString = " more than ";
            break;
        case ct_equal:
            cmpString = " equal to ";
            break;
        case ct_contains:
            cmpString = " containing ";
            break;
        case ct_startswith:
            cmpString = " starting with ";
            break;
        case ct_endswith:
            cmpString = " ending with ";
            break;
        default:
            cmpString = " ERROR ";
    }

    vString = term->value.toString();

    /* remove old view */
    if (term->view) 
        delete(term->view);
    term->view = new QListViewItem(display, 0, keyString, cmpString, vString);
    display->setSelected(term->view, true);
}

/* deletes current term */
void TVFilterView::deleteTerm() 
{
    if(!current) return;
    if (current->view)
        delete(current->view);

    terms.removeRef(current);

    current = terms.first();

    if(terms.isEmpty()) {
        keyEntry->setEnabled(false);
        keyNameCombo->setEnabled(false);
    }
}

/* clears all terminations */
void TVFilterView::clearTerms() 
{
    while(current)
        deleteTerm();
}

void TVFilterView::setTerm(QListViewItem *target)
{
    /* Iterate through the list to find item with view=target.. 
     * set as current, delete */
    FilterTerm *term = current;

    for (current = terms.first(); current != 0; current = terms.next() )
        if (current->view == target)
            break;

    if (!current) {
        current = term; 
    }
}
