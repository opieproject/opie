#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include <opie/ofiledialog.h>

#include "file_layer.h"
#include "receive_layer.h"
#include "metafactory.h"
#include "mainwindow.h"

#include "transferdialog.h"

TransferDialog::TransferDialog(MainWindow *parent, const char *name)
: QDialog(0l, 0l, true), m_win(parent)
{
    m_lay = 0l;
	m_recvlay = 0l;
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox, *hbox2, *hbox3;
	QLabel *file, *mode, *progress, *status;
	QButtonGroup *group;
	QRadioButton *mode_send, *mode_receive;

	m_autocleanup = 0;

	group = new QButtonGroup(QObject::tr("Transfer mode"), this);
	mode_send = new QRadioButton(QObject::tr("Send"), group);
	mode_receive = new QRadioButton(QObject::tr("Receive"), group);
	group->insert(mode_send, id_send);
	group->insert(mode_receive, id_receive);
	vbox2 = new QVBoxLayout(group, 2);
	vbox2->addSpacing(10);
	hbox3 = new QHBoxLayout(vbox2, 2);
	hbox3->add(mode_send);
	hbox3->add(mode_receive);
	mode_send->setChecked(true);
	m_transfermode = id_send;

	file = new QLabel(QObject::tr("Send file"), this);
	mode = new QLabel(QObject::tr("Transfer protocol"), this);
	progress = new QLabel(QObject::tr("Progress"), this);
	status = new QLabel(QObject::tr("Status"), this);

	statusbar = new QLabel(QObject::tr("Ready"), this);
	statusbar->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	protocol = new QComboBox(this);
	QStringList list = m_win->factory()->fileTransferLayers();
	for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
		protocol->insertItem((*it));

	filename = new QLineEdit(this);

	progressbar = new QProgressBar(this);
	progressbar->setProgress(0);

	selector = new QPushButton("...", this);
	ok = new QPushButton(QObject::tr("Start transfer"), this);
	cancel = new QPushButton(QObject::tr("Cancel"), this);

	vbox = new QVBoxLayout(this, 2);
	vbox->add(group);
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
	connect(group, SIGNAL(clicked(int)), SLOT(slotMode(int)));
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
	if((m_transfermode == id_send) && (filename->text().isEmpty()))
	{
		QMessageBox::information(this,
			QObject::tr("Attention"),
			QObject::tr("No file has been specified."));
		return;
	}

	ok->setEnabled(false);

	cleanup();
	m_autocleanup = 0;

	if(m_transfermode == id_send) statusbar->setText(QObject::tr("Sending..."));
	else statusbar->setText(QObject::tr("Receiving..."));

	if(m_transfermode == id_send)
	{
		m_lay = m_win->factory()->newFileTransfer(protocol->currentText(), m_win->currentSession()->layer());
		m_lay->sendFile(filename->text());

		connect(m_lay, SIGNAL(progress(const QString&, int, int, int, int, int)),
			SLOT(slotProgress(const QString&, int, int, int, int, int)));
		connect(m_lay, SIGNAL(error(int, const QString&)), SLOT(slotError(int, const QString&)));
		connect(m_lay, SIGNAL(sent()), SLOT(slotSent()));
	}
	else
	{
		m_recvlay = m_win->factory()->newReceive(protocol->currentText(), m_win->currentSession()->layer());
		m_recvlay->receive();

		connect(m_recvlay, SIGNAL(progress(const QString&, int, int, int, int, int)),
			SLOT(slotProgress(const QString&, int, int, int, int, int)));
		connect(m_recvlay, SIGNAL(error(int, const QString&)), SLOT(slotError(int, const QString&)));
		connect(m_recvlay, SIGNAL(received(const QString&)), SLOT(slotReceived(const QString&)));
	}
}

void TransferDialog::cleanup()
{
	if(m_lay)
	{
		m_lay->cancel();
		delete m_lay;
		m_lay = 0l;
	}
	if(m_recvlay)
	{
		m_recvlay->cancel();
		delete m_recvlay;
		m_recvlay = 0l;
	}
}

void TransferDialog::slotCancel()
{
	ok->setEnabled(true);
	statusbar->setText(QObject::tr("Ready"));

	if((m_lay) || (m_recvlay))
	{
		cleanup();
		if(m_autocleanup) close();
		else
		{
			QMessageBox::information(this,
				QObject::tr("Cancelled"),
				QObject::tr("The file transfer has been cancelled."));
		}
	}
	else
	{
		close();
	}
}

void TransferDialog::slotProgress(const QString& file, int progress, int speed, int hours, int minutes, int seconds)
{
	progressbar->setProgress(progress);
}

void TransferDialog::slotError(int error, const QString& message)
{
	statusbar->setText(QObject::tr("Ready"));

	switch(error)
	{
			case FileTransferLayer::NotSupported:
				QMessageBox::critical(this,
					QObject::tr("Error"),
					QObject::tr("Operation not supported."));
				break;
			case FileTransferLayer::StartError:
				QMessageBox::critical(this,
					QObject::tr("Error"),
					QObject::tr("Transfer could not be started."));
				break;
			case FileTransferLayer::NoError:
				QMessageBox::critical(this,
					QObject::tr("Error"),
					QObject::tr("No error."));
				break;
			case FileTransferLayer::Undefined:
				QMessageBox::critical(this,
					QObject::tr("Error"),
					QObject::tr("Undefined error occured."));
				break;
			case FileTransferLayer::Incomplete:
				QMessageBox::critical(this,
					QObject::tr("Error"),
					QObject::tr("Incomplete transfer."));
				break;
			case FileTransferLayer::Unknown:
			default:
				QMessageBox::critical(this,
					QObject::tr("Error"),
					QObject::tr("Unknown error occured."));
				break;
	}

	m_autocleanup = 1;
}

void TransferDialog::slotSent()
{
	QMessageBox::information(this, QObject::tr("Sent"), QObject::tr("File has been sent."));
	ok->setEnabled(true);
	statusbar->setText(QObject::tr("Ready"));
	m_autocleanup = 1;
}

void TransferDialog::slotReceived(const QString& file)
{
	QMessageBox::information(this, QObject::tr("Sent"), QObject::tr("File has been received as %1.").arg(file));
	ok->setEnabled(true);
	statusbar->setText(QObject::tr("Ready"));
	m_autocleanup = 1;
}

void TransferDialog::slotMode(int id)
{
	if(id == id_send)
	{
		selector->setEnabled(true);
		filename->setEnabled(true);
	}
	else
	{
		selector->setEnabled(false);
		filename->setEnabled(false);
	}
	m_transfermode = id;
}

