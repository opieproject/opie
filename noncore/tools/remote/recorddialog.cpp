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

#include "recorddialog.h"

RecordDialog::RecordDialog(QWidget *parent, const char *name)
    :QDialog(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hlayout = new QHBoxLayout(this);

	layout->insertSpacing(0,5);
	output = new QTextView("Please enter the name of the new remote, and press Return\n", 0, this, "output");
	layout->insertWidget(-1, output);
	layout->insertSpacing(-1, 5);
	layout->insertLayout(-1, hlayout);
	layout->insertSpacing(-1, 5);

	hlayout->insertSpacing(0, 5);
	input = new QLineEdit(this, "input");
	hlayout->insertWidget(-1, input, 1);
	hlayout->insertSpacing(-1, 5);

	QPushButton *ret = new QPushButton("Return", this, "return");
	hlayout->insertWidget(-1, ret);
	hlayout->insertSpacing(-1, 5);
	connect(ret, SIGNAL(clicked()), this, SLOT(retPressed()) );
	where = 0;

	record = new OProcess;
}

void RecordDialog::retPressed()
{
	printf("RecordDialog::retPressed: ret pressed\n");

	if(where == 0)
	{
		connect(record, SIGNAL(receivedStdout(OProcess *, char *, int)), this, SLOT(incoming(OProcess *, char *, int)) );
		connect(record, SIGNAL(receivedStderr(OProcess *, char *, int)), this, SLOT(incoming(OProcess *, char *, int)) );
		connect(record, SIGNAL(processExited(OProcess *)), this, SLOT(done(OProcess *)) );
		printf("RecordDialog::retPressed: starting irrecord\n");
		QString file = "/tmp/" + input->text();
		*record<<"irrecord"<<file.latin1();
		if(!record->start(OProcess::NotifyOnExit, OProcess::AllOutput))
		{
			QMessageBox *mb = new QMessageBox("Error!",
											"Could not start irrecord.  You must<br>use an lirc ipkg that includes<br>irrecord",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
			mb->exec();
			return;
		}
//		record->resume();
		where = 1;
	}
}

void RecordDialog::incoming(OProcess *proc, char *buffer, int len)
{
//	output->setText(output->text() + QString(buffer).truncate(len-1));
	printf("RecordDialog::incoming: got text from irrecord\n");
}

void RecordDialog::done(OProcess *proc)
{
}
