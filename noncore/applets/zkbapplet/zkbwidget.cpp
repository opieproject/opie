#include <opie2/otaskbarapplet.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <stdio.h>
#include <unistd.h>
#include "zkbwidget.h"
#include "zkbcfg.h"

using namespace Opie::Ui;
ZkbWidget::ZkbWidget(QWidget* parent):QLabel(parent),keymap(0),
	disabled(Resource::loadPixmap("zkb-disabled")) {

	labels = new QPopupMenu();
	connect(labels, SIGNAL(activated(int)), this,
		SLOT(labelChanged(int)));

	loadKeymap();

	channel = new QCopChannel("QPE/zkb", this);
	connect(channel, SIGNAL(received(const QCString&,const QByteArray&)),
		this, SLOT(signalReceived(const QCString&,const QByteArray&)));
    setFixedWidth ( AppLnk::smallIconSize()  );
    setFixedHeight ( AppLnk::smallIconSize()  );
}

ZkbWidget::~ZkbWidget() {
}

int ZkbWidget::position()
{
	return 8;
}

bool ZkbWidget::loadKeymap() {
	ZkbConfig c(QPEApplication::qpeDir()+"/share/zkb");
	QFontMetrics fm(font());

	if (keymap != 0) {
		delete keymap;
		keymap = 0;
	}

	Keymap* km = new Keymap();

	if (!c.load("zkb.xml", *km, "")) {
		delete km;
		setPixmap(disabled);
		return false;
	}

	connect(km, SIGNAL(stateChanged(const QString&)),
		this, SLOT(stateChanged(const QString&)));

	qwsServer->setKeyboardFilter(km);

	Keymap* oldkm = keymap;
	keymap = km;

	if (oldkm != 0) {
		delete oldkm;
	}

	setText(keymap->getCurrentLabel());

	labels->clear();
	QStringList l = keymap->listLabels();
	labels->insertItem(disabled, 0, 0);
	int n = 1;
	w = 0;
	for(QStringList::Iterator it = l.begin(); it != l.end();
		++it, n++) {

//		printf("label: %s\n", (const char*) (*it).utf8());

		labels->insertItem(*it, n, n);
		int lw = fm.width(*it);
		if (lw > w) {
			w = lw;
		}
	}

	if (w == 0) {
		hide();
	} else {
		show();
	}
	return true;
}

QSize ZkbWidget::sizeHint() const {
    return QSize(AppLnk::smallIconSize(),AppLnk::smallIconSize());
}

void ZkbWidget::stateChanged(const QString& s) {
//	odebug << "stateChanged: " << s.utf8() << "\n" << oendl; 
	setText(s);
}

void ZkbWidget::labelChanged(int id) {
	if (id == 0) {
		keymap->disable();
		setPixmap(disabled);
		return;
	}

	keymap->enable();

	QStringList l = keymap->listLabels();
	QString lbl = l[id-1];

//	printf("labelChanged: %s\n", (const char*) lbl.utf8());
	State* state = keymap->getStateByLabel(lbl);
	if (state != 0) {
		keymap->setCurrentState(state);
		setText(lbl);
	}
}

void ZkbWidget::mouseReleaseEvent(QMouseEvent*) {
	QSize sh = labels->sizeHint();
	QPoint p = mapToGlobal(QPoint((width()-sh.width())/2,-sh.height()));
	labels->exec(p);
}

void ZkbWidget::signalReceived(const QCString& msg, const QByteArray& data) {
	QDataStream stream(data, IO_ReadOnly);

	if (msg == "enable()") {
		keymap->enable();
	} else if (msg == "disable()") {
		keymap->disable();
	} else if (msg == "reload()") {
		QCopEnvelope("QPE/System", "busy()");
		QTimer::singleShot(0, this, SLOT(reload()));
	} else if (msg == "switch(QString)") {
		QString lbl;
		stream >> lbl;

		if (keymap != 0) {
			State* state = keymap->getStateByLabel(lbl);
			if (state != 0) {
				keymap->setCurrentState(state);
				setText(lbl);
			}
		}
	} else if (msg == "debug(QString)") {
		QString flag;
		stream >> flag;
	}
}

void ZkbWidget::reload() {
	loadKeymap();
	QCopEnvelope("QPE/System", "notBusy()");
}

EXPORT_OPIE_APPLET_v1( ZkbWidget )


