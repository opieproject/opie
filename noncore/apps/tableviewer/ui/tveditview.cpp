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


/* The edit view widget.  For each key in the DB display an
 * appropriate edit box, and a 'key' button to change that particular
 * key information (delete or edit).
 *
 * Bottem line should be a 'new key' button.  Should be able to scroll
 * in both directions.
 */

#include "tveditview.h"
#include "commonwidgets.h"

#include <qlayout.h>
#include <qgrid.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlist.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qsignalmapper.h>

TVEditView::TVEditView(TableState *s, DataElem *d, QWidget* parent, 
        const char *name, WFlags fl ) : QDialog(parent, name, true, fl)
{
    if (!name)
        setName("TVEditView");

    QVBoxLayout *layout = new QVBoxLayout(this, 0); /* only so that will resize
                                                       correctly in other 
                                                       widgets */

    toggles = new QSignalMapper(this);
    QScrollView *sv = new QScrollView(this, 0);
    sv->setResizePolicy(QScrollView::AutoOneFit);

    layout->addWidget(sv);

    editDisplay = new QGrid(3, sv, 0);
    editDisplay->setSpacing(3);
    sv->addChild(editDisplay);

    connect(toggles, SIGNAL(mapped(int)), this, SLOT(toggleEnabled(int)));

    setData(s, d);
#ifdef Q_WS_QWS
    showMaximized();
#endif
}

TVEditView::~TVEditView() 
{
}

/*! set up the widgets in the grid, Set up initial values */
void TVEditView::setData(TableState *t, DataElem *d) 
{

    /* TODO need to somehow clear old children... a delete of each
     * child? */
    keyIds.clear();

    KeyListIterator it(*t->kRep);

    int i = 0;
    while(it.current()) {
		if (t->kRep->validIndex(it.currentKey())) {
			new QLabel(it.current()->name(), editDisplay);
			keyIds.insert(i, it.currentKey());
			if (d->hasValidValue(it.currentKey())) {
				switch(it.current()->type()) {
					case TVVariant::String: {
						QLineEdit *edit = new QLineEdit(editDisplay, 0);
						edit->setText(d->getField(it.currentKey()).toString());
						edits.append(edit);
						break;
					}
					case TVVariant::Int:  {
					    IntEdit *edit = new IntEdit(editDisplay, 0);
					    edit->setValue(d->getField(it.currentKey()).toInt());
					    edits.append(edit);
						break;
					}
				case TVVariant::Time: {
						TimeEdit *edit = new TimeEdit(editDisplay, 0);
						edit->setTime(d->getField(it.currentKey()).toTime());
						edits.append(edit);
						break;
					}
				case TVVariant::Date: {
						DateEdit *edit = new DateEdit(editDisplay, 0);
						edit->setDate(d->getField(it.currentKey()).toDate());
						edits.append(edit);
						break;
					}
					default:
						edits.append(new QLabel("<B><I>Uknown key type</I></B>", editDisplay));
				}
				QCheckBox *tb = new QCheckBox(editDisplay);
				tb->setChecked(TRUE);
				toggles->setMapping(tb, i);
				connect(tb, SIGNAL(clicked()), toggles, SLOT(map()));
				buttons.append(tb);
			} else {
				/* No valid value.. set to null */
				switch(it.current()->type()) {
					case TVVariant::String: {
						QLineEdit *edit = new QLineEdit(editDisplay, 0);
						edit->setEnabled(false);
						edits.append(edit);
						break;
					}
					case TVVariant::Int:  {
						IntEdit *edit = new IntEdit(editDisplay, 0);
						edit->setEnabled(false);
						edits.append(edit);
						break;
					}
					case TVVariant::Time: {
						TimeEdit *edit = new TimeEdit(editDisplay, 0);
						edit->setEnabled(false);
						edits.append(edit);
						break;
					}
					case TVVariant::Date: {
						DateEdit *edit = new DateEdit(editDisplay, 0);
						edit->setEnabled(false);
						edits.append(edit);
						break;
					}
					default:
						edits.append(new QLabel("<B><I>Uknown key type</I></B>", editDisplay));
				}
				QCheckBox *tb = new QCheckBox(editDisplay);
				tb->setChecked(FALSE);
				toggles->setMapping(tb, i);
				connect(tb, SIGNAL(clicked()), toggles, SLOT(map()));
				buttons.append(tb);
			}
			i++;
		}
        ++it;
    }
	num_edits = i;
}

void TVEditView::toggleEnabled(int i) {

    if(edits.at(i)->isEnabled()) {
        edits.at(i)->setEnabled(false);
        buttons.at(i)->setChecked(FALSE);
    } else {
        edits.at(i)->setEnabled(true);
        buttons.at(i)->setChecked(TRUE);
    }
}

bool TVEditView::openEditItemDialog(TableState *ts, DataElem *d, 
					QWidget *parent)
{
    int i;
    int keyId;

    if(!ts) return 0;
    if(!d) return 0;
    if(!ts->kRep) return 0;

    TVEditView *dlg = new TVEditView(ts, d, parent);

    if (dlg->exec() == QDialog::Accepted ) {
        /* update the element, basically for each
           edits, if isEnabled, set Value, else unsetField */

        for(i = 0; i < dlg->num_edits; i++) {
            keyId = dlg->keyIds[i];
            if(dlg->edits.at(i)->isEnabled()) {
                switch(d->getFieldType(keyId)) {
                    case TVVariant::String: {
                        TVVariant value = TVVariant(
                                ((QLineEdit *)dlg->edits.at(i))->text());
                        d->setField(keyId, value);
                        break;
                    }
                    case TVVariant::Int:  {
                        TVVariant value = TVVariant(
                                ((IntEdit *)dlg->edits.at(i))->value());
                        d->setField(keyId, value);
                        break;
                    }
                    case TVVariant::Time: {
                        TVVariant value = TVVariant(
                                ((TimeEdit *)dlg->edits.at(i))->time());
                        d->setField(keyId, value);
                        break;
                    }
                    case TVVariant::Date: {
                        TVVariant value = TVVariant(
                                ((DateEdit *)dlg->edits.at(i))->date());
                        d->setField(keyId, value);
                        break;
                    }
                    default:
                        break;
                }
            } else {
                /* unset the field */
                d->unsetField(keyId);
            }
        }
	delete dlg;
	return TRUE;
    }

    return FALSE;
}
