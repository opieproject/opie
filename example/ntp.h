#ifndef NTP_H
#define NTP_H
#include "ntpbase.h"
#include <qdatetime.h>
#include <qtimer.h>

class OProcess;
class QString;


class Ntp : public NtpBase
{
    Q_OBJECT

public:
    Ntp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Ntp();

    OProcess *ntpProcess;

private:
		QString _ntpOutput;
  	int _maxOffset;
   	float _shiftPerSec;
  	QTimer *_nextCorrection;
   	float getTimeShift();
    int _minLookupDiff;
private slots:
    void slotRunNtp();
    void getNtpOutput(OProcess *proc, char *buffer, int buflen);
    void ntpFinished(OProcess*);
    void correctClock();
};

#endif
