#ifndef NTP_H
#define NTP_H
#include "settime.h"
#include <qdatetime.h>

class OProcess;
class QString;


class Ntp : public SetDateTime
{
    Q_OBJECT

public:
    Ntp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Ntp();

protected:
		QDateTime predictedTime;


private:
		QString _ntpOutput;
   	float _shiftPerSec;
    OProcess *ntpProcess;

   	float getTimeShift();
	  void readLookups();
private slots:
    void slotRunNtp();
    void getNtpOutput(OProcess *proc, char *buffer, int buflen);
    void ntpFinished(OProcess*);
	  void preditctTime();
	  void slotCheckNtp(int);
	  void setPredictTime();
};

#endif
