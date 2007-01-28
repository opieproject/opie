/*
Opie-Remote.  emulates remote controlls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2002 Thomas Stephens
 
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.
 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.
 
You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "learntab.h"

/* OPIE */
#include <qpe/qpeapplication.h>

LearnTab::LearnTab(QWidget *parent, const char *name):QWidget(parent,name)
{
    LircHandler lh;

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *bottomLayout = new QHBoxLayout(this);

    layout->insertSpacing(0,5);
    remotesBox = new QListBox(this, "remotesBox");
    layout->insertWidget(0, remotesBox, 1);
    remotesBox->insertStringList(lh.getRemotes());

    layout->insertSpacing(-1,5);
    layout->insertLayout(-1, bottomLayout);
    layout->insertSpacing(-1,5);

    QPushButton *add = new QPushButton("Add", this, "add");
    bottomLayout->insertSpacing(-1, 5);
    bottomLayout->insertWidget(-1, add);
    bottomLayout->insertSpacing(-1, 5);
    QPushButton *edit = new QPushButton("Edit", this, "edit");
    bottomLayout->insertWidget(-1, edit);
    bottomLayout->insertSpacing(-1, 5);
    QPushButton *del = new QPushButton("Delete", this, "delete");
    bottomLayout->insertWidget(-1, del);
    bottomLayout->insertSpacing(-1, 5);

    connect(add, SIGNAL(clicked()), this, SLOT(add()) );
    connect(edit, SIGNAL(clicked()), this, SLOT(edit()) );
    connect(del, SIGNAL(clicked()), this, SLOT(del()) );
}

void LearnTab::add()
{
    printf("LearnTab::add: add pressed\n");
    RecordDialog *dialog = new RecordDialog(this);
    QPEApplication::showDialog( dialog );
}

void LearnTab::edit()
{}

void LearnTab::del()
{}

