/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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

#include "addresspicker.h"
#include "abtable.h"

#include <qlayout.h>

/*!
  \a tab is reparented for use in the picker. Take it back out if you want
  to regain ownership.
*/
AddressPicker::AddressPicker(AbTable* tab, QWidget* parent, const char* name, bool modal) :
    QDialog(parent,name,modal)
{
    QVBoxLayout* vb = new QVBoxLayout(this);
    tab->reparent(this,QPoint(0,0));
    table = tab;
    vb->addWidget(table);
}

void AddressPicker::setChoiceNames(const QStringList& list)
{
    table->setChoiceNames(list);
}

void AddressPicker::setSelection(int index, const QStringList& list)
{
    table->setChoiceSelection(index,list);
}

QStringList AddressPicker::selection(int index) const
{
    return table->choiceSelection(index);
}
