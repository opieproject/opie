#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlayout.h>

#include <qpe/applnk.h>
#include <qpe/fileselector.h>
#include <qpe/resource.h>

#include <stdlib.h>

#include "attachdiag.h"

AttachDiag::AttachDiag(QWidget* parent = 0, const char* name = 0, bool modal, WFlags fl = 0)
	: QDialog(parent, name, modal, fl)
{
	setCaption(tr("Attach File"));

	QGridLayout *layout = new QGridLayout(this);
	layout->setSpacing(3);
	layout->setMargin(4);

	_fileSelector = new FileSelector("*", this, "FileSelector");
	_fileSelector->setCloseVisible(false);
	_fileSelector->setNewVisible(false);

	layout->addMultiCellWidget(_fileSelector, 0, 0, 0, 1);

	QPushButton *attachButton = new QPushButton(this);
	attachButton->setText(tr("Ok"));
	attachButton->setIconSet(Resource::loadPixmap("enter"));

	layout->addWidget(attachButton, 1, 0);

	QPushButton *cancelButton = new QPushButton(this);
	cancelButton->setText(tr("Cancel"));
	cancelButton->setIconSet(Resource::loadPixmap("editdelete"));

	layout->addWidget(cancelButton, 1, 1);

	connect(attachButton, SIGNAL(clicked()), SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), SLOT(close()));

	if (_fileSelector->fileCount() == 0) {
		attachButton->setEnabled(false);
		_fileSelector->setEnabled(false);
	}
}

DocLnk AttachDiag::selectedFile()
{
	return *_fileSelector->selected();
}

DocLnk AttachDiag::getFile(QWidget *parent)
{
	AttachDiag attach(parent, 0, true);
	attach.showMaximized();
	attach.show();

	if (QDialog::Accepted == attach.exec()) {
		return attach.selectedFile();
	}

	return DocLnk();
}	


