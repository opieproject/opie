#ifndef NTP_H
#define NTP_H
#include "settime.h"
#include <qdatetime.h>

class OProcess;
class QString;
class QTimer;
class QSocket;
class QCopChannel;

class Ntp : public SetDateTime
{
  Q_OBJECT

public:
  Ntp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~Ntp();

public slots:
  void setDocument (const QString &);
protected:
  virtual void accept( );
  QDateTime predictedTime;
  void makeChannel();
protected slots:
  void receive(const QCString &msg, const QByteArray &arg);
private:
  QString _ntpOutput;
  float _shiftPerSec;
  int _lookupDiff;
  OProcess *ntpProcess;
  QTimer *ntpTimer;
  QSocket *ntpSock;
  QCopChannel *channel;
  bool _interactive;
  float getTimeShift();
  void readLookups();
  void ntpOutPut(QString);
  bool ntpDelayElapsed();
  QString getNtpServer();
  void saveConfig();
private slots:
  void slotTimerRunNtp();
  void slotButtonRunNtp();
  void slotRunNtp();
  void getNtpOutput(OProcess *proc, char *buffer, int buflen);
  void ntpFinished(OProcess*);
  void preditctTime();
  void slotCheckNtp(int);
  void setPredictTime();
  void showAdvancedFeatures(bool);
  void slotProbeNtpServer();
  void slotNtpDelayChanged(int);
};

#endif
