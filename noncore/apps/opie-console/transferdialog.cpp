#include "transferdialog.h"

#include "qlayout.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qmessagebox.h"
#include "qprogressbar.h"

#include "opie/ofiledialog.h"

TransferDialog::TransferDialog(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox, *hbox2;
	QLabel *file, *mode, *progress, *status;
	QPushButton *selector, *ok, *cancel;

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
	ok = new QPushButton(QObject::tr("OK"), this);
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
	connect(cancel, SIGNAL(clicked()), SLOT(close()));
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

	statusbar->setText(QObject::tr("Sending..."));
	progressbar->setProgress(1);
}

