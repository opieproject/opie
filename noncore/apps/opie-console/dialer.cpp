#include "dialer.h"

#include <qlayout.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapp.h>
#include <qtimer.h>

// State machine:          | When an error occurs, we don't have to
//                         | reset everything.
// (init) <------+         | But if the user wants to reset,
//   |           |         | we stop dialing immediately.
//   v           |         |
// (options) ----+         | Following the state machine is necessary
//   |       \             | to get determinable results.
//   v        ^            |
// (dial) ----+            |
//   |        ^            |
//   v        |            |
// (online) --+            |
//   |                     |
//   v                     |

Dialer::Dialer(const QString& number, QWidget *parent, const char *name)
: QDialog(parent, name, true)
{
	QVBoxLayout *vbox;
	QLabel *desc;

	usercancel = 0;
	m_number = number;

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

	connect(cancel, SIGNAL(clicked()), SLOT(slotCancel()));

	show();

	QTimer::singleShot(500, this, SLOT(slotAutostart()));
}

Dialer::~Dialer()
{
}

void Dialer::slotCancel()
{
	if(state != state_online) reset();
	else accept();
}

void Dialer::reset()
{
	switchState(state_cancel);
	usercancel = 1;
}

void Dialer::slotAutostart()
{
	state = state_preinit;
	dial(m_number);
}

void Dialer::dial(const QString& number)
{
	while(state != state_online)
	{
		if(!usercancel)
		{
			trydial(number);
		}
		else break;
	}

	if(usercancel)
	{
		reject();
	}
}

void Dialer::trydial(const QString& number)
{
	if(state != state_cancel)
	{
		switchState(state_preinit);
		// ...
		QString response = receive();
	}

	if(state != state_cancel)
	{
		switchState(state_init);
		send("ATZ");
		QString response2 = receive();
	}

	if(state != state_cancel)
	{
		switchState(state_options);

		send("ATM0L0");
		QString response3 = receive();
	}

	if(state != state_cancel)
	{
		switchState(state_dialtone);

		send("ATX1");
		QString response4 = receive();
	}

	if(state != state_cancel)
	{
		switchState(state_dialing);

		send(QString("ATDT %1").arg(number));
		QString response5 = receive();
	}
	
	if(state != state_cancel)
	{
		switchState(state_online);
	}
}

void Dialer::send(const QString& msg)
{

}

QString Dialer::receive()
{
	for(int i = 0; i < 200000;i++)
		qApp->processEvents();
	return QString::null;
}

void Dialer::switchState(int newstate)
{
	int oldstate = state;
	state = newstate;

	switch(state)
	{
		case state_cancel:
			status->setText(QObject::tr("Cancelling..."));
			progress->setProgress(0);
			break;
		case state_preinit:
			status->setText(QObject::tr("Searching modem"));
			progress->setProgress(10);
			break;
		case state_init:
			status->setText(QObject::tr("Initializing..."));
			progress->setProgress(20);
			break;
		case state_options:
			status->setText(QObject::tr("Reset speakers"));
			progress->setProgress(30);
			break;
		case state_dialtone:
			status->setText(QObject::tr("Turning off dialtone"));
			progress->setProgress(40);
			break;
		case state_dialing:
			if(oldstate != state_dialing) status->setText(QObject::tr("Dial number"));
			else status->setText(QObject::tr("Line busy, redialing number"));
			progress->setProgress(50);
			break;
		case state_online:
			status->setText(QObject::tr("Connection established"));
			progress->setProgress(100);
			cancel->setText(QObject::tr("Dismiss"));
			break;
	}
}

