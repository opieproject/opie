#include <qlayout.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qsound.h>
#include <qtimer.h>
#include <qdir.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <stdlib.h>

#include "configfile.h"
#include "imapresponse.h"
#include "imaphandler.h"
#include "zaurusstuff.h"
#include "bend.h"

BenD::BenD(QWidget *parent, const char *name, WFlags fl)
	: QButton(parent, name, fl)
{
	_zaurus = false;
	if (QFile("/dev/sharp_buz").exists()) _zaurus = true;

	_config = new Config("mail");
	_config->setGroup("Settings");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addItem(new QSpacerItem(0,0));

	QLabel *pixmap = new QLabel(this);
	pixmap->setPixmap(Resource::loadPixmap("mail/mailchecker"));
	layout->addWidget(pixmap);

	layout->addItem(new QSpacerItem(0,0));

	hide();

	connect(this, SIGNAL(clicked()), SLOT(slotClicked()));

	if (!_config->readBoolEntry("Disabled", false)) {
		_intervalMs = _config->readNumEntry("CheckEvery", 5) * 60000;
		_intervalTimer = new QTimer();
		_intervalTimer->start(_intervalMs);
		connect(_intervalTimer, SIGNAL(timeout()), SLOT(slotCheck()));

		QTimer::singleShot(0, this, SLOT(slotCheck()));
	}
}

void BenD::drawButton(QPainter *) { }
void BenD::drawButtonText(QPainter *) { }

void BenD::slotClicked()
{
	QCopEnvelope e("QPE/System", "execute(QString)");
	e << QString("mail");

	if (_ledOn) {
		ZaurusStuff::blinkLedOff();
		_ledOn = false;
	}
}

void BenD::slotCheck()
{
	// Check wether the check interval has been changed.
	int newIntervalMs = _config->readNumEntry("CheckEvery", 5) * 60000;
	if (newIntervalMs != _intervalMs) {
		_intervalTimer->changeInterval(newIntervalMs);
		_intervalMs = newIntervalMs;
#ifndef QT_NO_DEBUG
		qWarning("BenD: Detected interval change");
#endif
	}

	QValueList<Account> acList = ConfigFile::getAccounts();
	QValueList<Account>::Iterator ot;
	for (ot = acList.begin(); ot != acList.end(); ot++) {
		if (!((*ot).imapServer().isEmpty() || 
		      (*ot).imapPort().isEmpty() || 
		      (*ot).user().isEmpty() ||
		      (*ot).pass().isEmpty())) {
			if (!((*ot).imapSsl() && 
			      (*ot).imapSslPort().isEmpty())) {
				IMAPHandler *handler = new IMAPHandler(*ot);
				handler->iStatus("INBOX", "RECENT");
				connect(handler, SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPStatus(IMAPResponse &)));
			}
		}
	}
}

void BenD::slotIMAPStatus(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPStatus(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		if (response.STATUS()[0].recent().toInt() > 0) {
			if (isHidden()) show();
			if (_config->readBoolEntry("BlinkLed", true)) 
				ZaurusStuff::blinkLedOn();
			if (_config->readBoolEntry("PlaySound", false)) {
				if (_zaurus) {
					ZaurusStuff::buzzerOn();
					QTimer::singleShot(3000, this, SLOT(slotSoundOff()));
				} else {
					QSound::play(Resource::findSound("mail/newmail"));
				}
			}
		} else {
			if (!isHidden()) hide();
			if (!_ledOn) {
				ZaurusStuff::blinkLedOff();
				_ledOn = false;
			}
		}
		response.imapHandler()->iLogout();
	} else qWarning("BenD: WARNING: Couldn't retrieve INBOX status.");
}

void BenD::slotSoundOff()
{
	ZaurusStuff::buzzerOff();
}

