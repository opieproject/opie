#ifndef ZAURUSSTUFF_H
#define ZAURUSSTUFF_H

#include <qobject.h>

class ZaurusStuff : public QObject
{
	Q_OBJECT

public:
	static void blinkLedOn();
	static void blinkLedOff();
	static void buzzerOn();
	static void buzzerOff();

protected:
	ZaurusStuff();
	

};

#endif

