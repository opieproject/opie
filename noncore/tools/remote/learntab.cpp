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
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *bottomLayout = new QHBoxLayout(this);

    layout->insertSpacing(0,5);
    remotesBox = new QListBox(this, "remotesBox");
    layout->insertWidget(0, remotesBox, 1);

    layout->insertSpacing(-1,5);
    layout->insertLayout(-1, bottomLayout);
    layout->insertSpacing(-1,5);

    QPushButton *learn = new QPushButton(tr("Learn"), this, "learn");
    bottomLayout->insertSpacing(-1, 5);
    bottomLayout->insertWidget(-1, learn);
    bottomLayout->insertSpacing(-1, 5);
    QPushButton *del = new QPushButton(tr("Delete"), this, "delete");
    bottomLayout->insertWidget(-1, del);
    bottomLayout->insertSpacing(-1, 5);

    connect(learn, SIGNAL(clicked()), this, SLOT(learn()) );
    connect(del, SIGNAL(clicked()), this, SLOT(del()) );
}

void LearnTab::learn()
{
	printf("LearnTab::learn: learn pressed\n");
	RecordDialog *dialog = new RecordDialog(this);
	dialog->showMaximized();
	dialog->exec();
	delete dialog;
	updateRemotesList();
}

void LearnTab::del()
{
	QString remotename = remotesBox->currentText();
	
	if(remotename != "") {
		if (QMessageBox::warning(this, tr("Delete"), 
					tr("Are you sure you want\n to delete %1?").arg(remotename),
					QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
			LircHandler lh;
			lh.removeRemote(remotename);
			lh.reloadLircdConf();
			updateRemotesList();
		}
	}
}

void LearnTab::updateRemotesList()
{
	LircHandler lh;
	remotesBox->clear();
	if(lh.checkLircdConfValid(true))
		remotesBox->insertStringList(lh.getRemotes());
}
