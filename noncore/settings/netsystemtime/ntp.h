#ifndef NTP_H
#define NTP_H
#include "settime.h"
#include <qdatetime.h>
#include <qtimer.h>

class OProcess;
class QString;


class Ntp : public SetDateTime
{
    Q_OBJECT

public:
    Ntp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Ntp();

    OProcess *ntpProcess;

protected:
    virtual void accept();

private:
		QString _ntpOutput;
  	int _maxOffset;
   	float _shiftPerSec;
  	QTimer *_nextCorrection;
    int _minLookupDiff;

   	float getTimeShift();
	  void readLookups();
private slots:
    void slotRunNtp();
    void getNtpOutput(OProcess *proc, char *buffer, int buflen);
    void ntpFinished(OProcess*);
    void correctClock();
	  void preditctTime();
};

#endif
