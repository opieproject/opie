#include "dialer.h"

#include <qlayout.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapp.h>

Dialer::Dialer(const QString& number, QWidget *parent, const char *name)
: QDialog(parent, name, true)
{
	QVBoxLayout *vbox;
	QLabel *desc;

	desc = new QLabel(QObject::tr("Dialing number: %1").arg(number), this);
	progress = new QProgressBar(this);
	status = new QLabel("", this);
	status->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	cancel = new QPushButton(QObject::tr("Cancel"), this);

	vbox = new QVBoxLayout(this, 2);
	vbox->add(desc);
	vbox->add(progress);
	vbox->add(status);
	vbox->add(cancel);

	reset();

	connect(cancel, SIGNAL(clicked()), SLOT(slotCancel()));

	show();

	dial(number);
}

Dialer::~Dialer()
{
}

void Dialer::slotCancel()
{
	if(state != state_online) reset();
	close();
}

void Dialer::reset()
{
	switchState(state_init);
}

void Dialer::dial(const QString& number)
{
	send("ATZ");
	QString response = receive();

	switchState(state_options);

	send("ATM0L0");
	QString response2 = receive();

	switchState(state_dialing);

	send(QString("ATDT %1").arg(number));
	QString response3 = receive();

	switchState(state_online);
}

void Dialer::send(const QString& msg)
{

}

QString Dialer::receive()
{
	for(int i = 0; i < 200000; i++)
		qApp->processEvents();
	return QString::null;
}

void Dialer::switchState(int newstate)
{
	state = newstate;

	switch(state)
	{
		case state_init:
			status->setText(QObject::tr("Initializing..."));
			progress->setProgress(10);
			break;
		case state_options:
			status->setText(QObject::tr("Reset speakers"));
			progress->setProgress(20);
			break;
		case state_dialing:
			status->setText(QObject::tr("Dial number"));
			progress->setProgress(30);
			break;
		case state_online:
			status->setText(QObject::tr("Connection established"));
			progress->setProgress(100);
			cancel->setText(QObject::tr("Dismiss"));
			break;
	}
}

