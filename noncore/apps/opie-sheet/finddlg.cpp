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

#include "finddlg.h"

/* QT */
#include <qlabel.h>
#include <qradiobutton.h>

FindDialog::FindDialog(QWidget *parent)
        :QDialog(parent, 0, TRUE)
{
    // Main widget
    tabs=new QTabWidget(this);
    widgetFind=new QWidget(tabs);
    widgetOptions=new QWidget(tabs);
    tabs->addTab(widgetFind, tr("&Find && Replace"));
    tabs->addTab(widgetOptions, tr("&Options"));

    // Find tab
    QLabel *label=new QLabel(tr("&Search for:"), widgetFind);
    label->setGeometry(10, 10, 215, 20);
    editFind=new QLineEdit(widgetFind);
    editFind->setGeometry(10, 40, 215, 20);
    label->setBuddy(editFind);

    label=new QLabel(tr("&Replace with:"), widgetFind);
    label->setGeometry(10, 80, 215, 20);
    editReplace=new QLineEdit(widgetFind);
    editReplace->setGeometry(10, 110, 215, 20);
    editReplace->setEnabled(FALSE);
    label->setBuddy(editReplace);

    groupType=new QVButtonGroup(tr("&Type"), widgetFind);
    groupType->setGeometry(10, 150, 215, 90);
    QRadioButton *radio=new QRadioButton(tr("&Find"), groupType);
    radio=new QRadioButton(tr("&Replace"), groupType);
    radio=new QRadioButton(tr("Replace &all"), groupType);
    groupType->setButton(0);
    connect(groupType, SIGNAL(clicked(int)), this, SLOT(typeChanged(int)));

    // Options tab
    checkCase=new QCheckBox(tr("Match &case"), widgetOptions);
    checkCase->setGeometry(10, 10, 215, 20);
    checkSelection=new QCheckBox(tr("Current &selection only"), widgetOptions);
    checkSelection->setGeometry(10, 40, 215, 20);
    checkEntire=new QCheckBox(tr("&Entire cell"), widgetOptions);
    checkEntire->setGeometry(10, 70, 215, 20);

    // Main widget
    box=new QVBoxLayout(this);
    box->addWidget(tabs);

    setCaption(tr("Find & Replace"));
}

FindDialog::~FindDialog()
{}

void FindDialog::typeChanged(int id)
{
    editReplace->setEnabled(id>0);
}

int FindDialog::exec(Sheet *s)
{
    if (QDialog::exec()==QDialog::Accepted)
    {
        int id=groupType->id(groupType->selected());
        s->dataFindReplace(editFind->text(), editReplace->text(), checkCase->isChecked(), !checkSelection->isChecked(), checkEntire->isChecked(), id>0, id>1);
        return QDialog::Accepted;
    }
    return QDialog::Rejected;
}
