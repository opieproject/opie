#include "transferdialog.h"

#include "filetransfer.h"
#include "io_serial.h"

#include "qlayout.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qmessagebox.h"
#include "qprogressbar.h"

#include "opie/ofiledialog.h"

TransferDialog::TransferDialog(QWidget *parent, const char *name)
: QDialog(/*parent, name*/NULL, NULL, true)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox, *hbox2;
	QLabel *file, *mode, *progress, *status;
	QPushButton *selector;

	transfer = NULL;

	file = new QLabel(QObject::tr("Send file"), this);
	mode = new QLabel(QObject::tr("Transfer mode"), this);
	progress = new QLabel(QObject::tr("Progress"), this);
	status = new QLabel(QObject::tr("Status"), this);

	statusbar = new QLabel(QObject::tr("ready"), this);
	statusbar->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	protocol = new QComboBox(this);
	protocol->insertItem("XModem");
	protocol->insertItem("YModem");
	protocol->insertItem("ZModem");

	filename = new QLineEdit(this);

	progressbar = new QProgressBar(this);
	progressbar->setProgress(0);

	selector = new QPushButton("...", this);
	ok = new QPushButton(QObject::tr("Start transfer"), this);
	cancel = new QPushButton(QObject::tr("Cancel"), this);

	vbox = new QVBoxLayout(this, 2);
	vbox->add(file);
	hbox = new QHBoxLayout(vbox, 0);
	hbox->add(filename);
	hbox->add(selector);
	vbox->add(mode);
	vbox->add(protocol);
	vbox->add(progress);
	vbox->add(progressbar);
	vbox->add(status);
	vbox->add(statusbar);
	vbox->addStretch(1);
	hbox2 = new QHBoxLayout(vbox, 2);
	hbox2->add(ok);
	hbox2->add(cancel);

	setCaption(QObject::tr("File transfer"));
	show();

	connect(selector, SIGNAL(clicked()), SLOT(slotFilename()));
	connect(ok, SIGNAL(clicked()), SLOT(slotTransfer()));
	connect(cancel, SIGNAL(clicked()), SLOT(slotCancel()));
}

TransferDialog::~TransferDialog()
{
}

void TransferDialog::slotFilename()
{
	QString f;
	
	f = OFileDialog::getOpenFileName(0);
	if(!f.isNull()) filename->setText(f);
}

void TransferDialog::slotTransfer()
{
	if(filename->text().isEmpty())
	{
		QMessageBox::information(this,
			QObject::tr("Attention"),
			QObject::tr("No file has been specified."));
		return;
	}

	ok->setEnabled(false);

	statusbar->setText(QObject::tr("Sending..."));
	progressbar->setProgress(1);

	FileTransfer::Type transfermode = FileTransfer::SX;
	if(protocol->currentText() == "YModem") transfermode == FileTransfer::SY;
	if(protocol->currentText() == "ZModem") transfermode == FileTransfer::SZ;

	// dummy profile
	Profile profile("Dummy", "serial", "vt102", Profile::White, Profile::Black, Profile::VT102);

	transfer = new FileTransfer(transfermode, new IOSerial(profile));
	transfer->sendFile(filename->text());
}

void TransferDialog::slotCancel()
{
	ok->setEnabled(true);

	if(transfer)
	{
		transfer->cancel();
		delete transfer;
		transfer = NULL;
		QMessageBox::information(this,
			QObject::tr("Cancelled"),
			QObject::tr("The file transfer has been cancelled."));
	}
	else
	{
		close();
	}
}

