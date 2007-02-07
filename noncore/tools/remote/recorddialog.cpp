/*
Opie-Remote.  emulates remote controlls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2007 Paul Eggleton & Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <stdlib.h>
#include <sys/stat.h>

#include <qfile.h>

#include "recorddialog.h"

#define RECORD_TMPDIR "/tmp/opie-remote"

using namespace Opie::Core;
using namespace Opie::Core;
RecordDialog::RecordDialog(QWidget *parent, const char *name)
    :QDialog(parent, name, TRUE)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hlayout = new QHBoxLayout(this);

	layout->insertSpacing(0,5);
	
	output = new QMultiLineEdit(this, "output");
	output->setReadOnly(true);
	output->setWordWrap(QMultiLineEdit::WidgetWidth);
	output->setText(tr("Please enter the name of the new remote, and press Next\n"));

	layout->insertWidget(-1, output);
	layout->insertSpacing(-1, 5);
	layout->insertLayout(-1, hlayout);
	layout->insertSpacing(-1, 5);

	hlayout->insertSpacing(0, 5);
	input = new QLineEdit(this, "input");
	hlayout->insertWidget(-1, input, 1);
	hlayout->insertSpacing(-1, 5);

	nextbtn = new QPushButton(tr("Next"), this, "next");
	nextbtn->setDefault(true);
	hlayout->insertWidget(-1, nextbtn);
	hlayout->insertSpacing(-1, 5);
	connect(nextbtn, SIGNAL(clicked()), this, SLOT(nextPressed()) );
	
	setCaption(tr("Learn Remote"));
	input->setFocus();
	
	stepnum = 0;
	outputenabled = false;
	writeok = true;
	ignorekill = false;
	
	record = new OProcess;
	
	// We need to stop lircd because irrecord accesses the IR device directly
	lh = new LircHandler();
	lh->stopLircd();
}

RecordDialog::~RecordDialog(void)
{
	delete lh;
	delete record;
}

void RecordDialog::nextPressed()
{
	if(stepnum == 0) {
		QString remotename = input->text().stripWhiteSpace();
		
		if(remotename == "") {
			QMessageBox::warning(this, tr("Error"),
											tr("Please enter a name"),
											QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}
		
		if(remotename.left(1) == ".") {
			QMessageBox::warning(this, tr("Error"),
											tr("Name cannot start with a dot"),
											QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}
		
		// Check for invalid chars
		QRegExp invalidchars = QRegExp("[~\\[\\]/'\"|\\\\]", TRUE, FALSE);
		if(remotename.find(invalidchars) > -1) {
			QMessageBox::warning(this, tr("Error"),
											tr("The name %1 contains\ninvalid characters").arg(remotename),
											QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}
		
		// Check if name already exists in config
		if(lh->checkRemoteExists(remotename)) {
			QMessageBox::warning(this, tr("Error"),
											tr("A remote named %1\nalready exists.").arg(remotename),
											QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}
		
		printf("RecordDialog::nextPressed: starting irrecord\n");
		input->setEnabled(false);
		
		connect(record, SIGNAL(receivedStdout(Opie::Core::OProcess*,char*,int)), this, SLOT(incoming(Opie::Core::OProcess*,char*,int)) );
		connect(record, SIGNAL(receivedStderr(Opie::Core::OProcess*,char*,int)), this, SLOT(incomingErr(Opie::Core::OProcess*,char*,int)) );
		connect(record, SIGNAL(processExited(Opie::Core::OProcess*)), this, SLOT(done(Opie::Core::OProcess*)) );
		connect(record, SIGNAL(wroteStdin(Opie::Core::OProcess*)), this, SLOT(writeFinished(Opie::Core::OProcess*)) );
		
		// Prepare temp path
		QString tmpdir = RECORD_TMPDIR;
		mkdir(tmpdir, 0700);
		remotefile = tmpdir + "/" + remotename;
		if(QFile::exists(remotefile))
			QFile::remove(remotefile);
		
		// Set up and start irrecord
		*record<<"irrecord"<<remotename.latin1();
		record->setWorkingDirectory(tmpdir);
		if(!record->start(OProcess::NotifyOnExit, OProcess::All))
		{
			QMessageBox::critical(this, tr("Error"),
											tr("Failed to start irrecord."),
											QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}
		stepnum = 1;
		writeToProcess("\n", 1);
		output->setText("Please get ready to press buttons on your remote.\n\nPress many different buttons, one at a time, holding each one down for approximately one second. Each button should generate at least one dot but in no case more than ten dots of output. Don't stop pressing buttons until at least two lines of dots appear.\n\nClick Next when you are ready to begin.");
	}
	else if(stepnum == 1) {
		output->setText("Press buttons now:\n");
		output->setCursorPosition(output->numLines(), 32767);
		outputenabled = true;
		decoding = true;
		writeToProcess("\n", 1);
		nextbtn->setEnabled(false);
		stepnum = 2;
	}
	else if(stepnum == 2) {
		QString inp = input->text() + "\n";
		writeToProcess(inp, inp.length());
		input->clear();
	}
}

void RecordDialog::incoming(OProcess *proc, char *buffer, int len)
{
	int line, col;
	
	if(outputenabled) {
		char *bufstr = strndup(buffer, len);
		output->getCursorPosition( &line, &col );
		output->insertAt( bufstr, line, col );
						
		if(decoding) {
			int pos = output->text().find("Now enter the names for the buttons");
			if(pos > -1) {
				output->setText(output->text().mid(pos));
				output->setCursorPosition(output->numLines(), 32767);
				decoding = false;
				input->clear();
				input->setEnabled(true);
				nextbtn->setEnabled(true);
				input->setFocus();
			}
		}
		
		free(bufstr);
	}
}

void RecordDialog::incomingErr(OProcess *proc, char *buffer, int len)
{
	char *bufstr = strndup(buffer, len);
	errors += bufstr;
	free(bufstr);
}

void RecordDialog::writeToProcess(const char *buffer, int len)
{
	while(!writeok) { }
	// FIXME leakage
	record->writeStdin(strndup(buffer, len), len);
}

void RecordDialog::writeFinished(Opie::Core::OProcess *proc)
{
	writeok = true;
}

void RecordDialog::done(OProcess *proc)
{
	if(proc->normalExit() && proc->exitStatus() == 0) {
		// Success
		
		// Merge in new remote to lircd.conf
		lh->mergeRemoteConfig(remotefile);
		
		nextbtn->hide();
		input->hide();
		output->setText("Learning completed successfully. Click OK to close.");
		stepnum = 4;
	}
	else if(!ignorekill) {
		QMessageBox::critical(this, tr("Error"), tr("Learning failed.\n") + errors, QMessageBox::Ok, QMessageBox::NoButton);
	}
}

void RecordDialog::accept()
{
	if(stepnum < 4)
		QMessageBox::warning(this, tr("Error"), tr("Please complete the learning\n process or tap X to cancel."), QMessageBox::Ok, QMessageBox::NoButton);
	else {
		if(checkClose())
			QDialog::accept();
	}
}

void RecordDialog::reject()
{
	if(checkClose())
		QDialog::reject();
}

bool RecordDialog::checkClose()
{
	if(!record->isRunning() || confirmClose()) {
		if(record->isRunning()) {
			ignorekill = true;
			record->kill();
		}
		// Restart lircd
		lh->startLircd();
		// Delete temp file if it exists
		if(QFile::exists(remotefile))
			unlink(remotefile);
		rmdir(RECORD_TMPDIR);
		
		return true;
	}
	return false;
}

bool RecordDialog::confirmClose()
{
	return (QMessageBox::warning(this, tr("Cancel"), 
					tr("Learning is not complete.\nAre you sure you want to close?"), 
					QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes);
}
