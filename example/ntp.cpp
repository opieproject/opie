#include "ntp.h"
#include <qpushbutton.h>
#include <qregexp.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <opie/oprocess.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/timeconversion.h>
#include <qpe/tzselect.h>
#include <qpe/timestring.h>
#include <qpe/qpedialog.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


Ntp::Ntp( QWidget* parent,  const char* name, WFlags fl )
    : NtpBase( parent, name, fl )
{
	Config cfg("ntp",Config::User);
  cfg.setGroup("settings");
 	_maxOffset = cfg.readNumEntry("maxOffset",5);
  _minLookupDiff = cfg.readNumEntry("minLookupDiff",10);
  LineEditNtpServer->setText( cfg.readEntry("ntpServer") );
 	ntpProcess = new OProcess( );
  connect ( ntpProcess, SIGNAL(receivedStdout(OProcess*,char*,int)),
  					this, SLOT(getNtpOutput(OProcess*,char*,int)));
  connect ( ntpProcess, SIGNAL(processExited(OProcess*)),
  					this, SLOT(ntpFinished(OProcess*)));
  connect(runNtp, SIGNAL(clicked()), this, SLOT(slotRunNtp()));
 	_nextCorrection = new QTimer( this );
    connect( _nextCorrection, SIGNAL(timeout()), SLOT(correctClock()) );
  slotRunNtp();
}

Ntp::~Ntp()
{
	Config cfg("ntp",Config::User);
  cfg.setGroup("settings");
  cfg.writeEntry("ntpServer", LineEditNtpServer->text());
}


void Ntp::slotRunNtp()
{
	TextLabelStartTime->setText(QDateTime::currentDateTime().toString());
	ntpProcess->clearArguments();
	*ntpProcess << "ntpdate" << LineEditNtpServer->text();
	bool ret = ntpProcess->start(OProcess::NotifyOnExit,OProcess::AllOutput);
  if ( !ret ) {
     qDebug("Error while executing ntp");
     outPut->append("\nError while executing\n\n");
  }
}

void  Ntp::getNtpOutput(OProcess *proc, char *buffer, int buflen)
{
  QString lineStr, lineStrOld;
  lineStr = buffer;
  lineStr=lineStr.left(buflen);
  if (lineStr!=lineStrOld)
  {
	        outPut->append(lineStr);
         	_ntpOutput += lineStr;
  }
  lineStrOld = lineStr;
}

void  Ntp::ntpFinished(OProcess*)
{
	Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
  int lastLookup = cfg.readNumEntry("time",0);
  int lookupCount = cfg.readNumEntry("count",-1);
  int time = TimeConversion::toUTC( QDateTime::currentDateTime() );
  cfg.writeEntry("time", time);
 	
  float timeShift = getTimeShift();
 	int secsSinceLast = time - lastLookup;
  TextLabelNewTime->setText(QDateTime::currentDateTime().toString());
  if ( lastLookup > 0 && secsSinceLast > 60*_minLookupDiff)
  {
	  lookupCount++;
   	cfg.writeEntry("count",lookupCount);
	  cfg.setGroup("lookup_"+QString::number(lookupCount));
    _shiftPerSec =  timeShift / secsSinceLast;
    float nextCorr = _maxOffset / _shiftPerSec;
   	qDebug("secs since last lookup %i", secsSinceLast);qDebug("timeshift since last lookup %f", timeShift);qDebug("timeshift since per sec %f", _shiftPerSec);qDebug("in %f secs the time will be shifted by %i", nextCorr,_maxOffset);
		cfg.writeEntry("secsSinceLast",secsSinceLast);
		cfg.writeEntry("timeShift",QString::number(timeShift));
  }
}

void Ntp::correctClock()
{
	qDebug("current time: %s",QDateTime::currentDateTime().toString().latin1());
	Config cfg("ntp",Config::User);
  cfg.setGroup("correction");
 	int lastTime = cfg.readNumEntry("time",0);
  int now = TimeConversion::toUTC( QDateTime::currentDateTime() );
  int corr = int((now - lastTime) * _shiftPerSec);
	outPut->append( "time will be shifted by "+QString::number(corr)+ "secs");
  struct timeval myTv;
  myTv.tv_sec = TimeConversion::toUTC( QDateTime::currentDateTime().addSecs(corr) );
  myTv.tv_usec = 0;

  if ( myTv.tv_sec != -1 )
      ::settimeofday( &myTv, 0 );
  Global::writeHWClock();
  cfg.writeEntry("time",now);
	qDebug("current time: %s",QDateTime::currentDateTime().toString().latin1());
}

float Ntp::getTimeShift()
{
	QString _offset = "offset";
  QString _sec = "sec";
  QRegExp _reOffset = QRegExp(_offset);
  QRegExp _reEndOffset = QRegExp(_sec);
  int posOffset = _reOffset.match( _ntpOutput );
  int posEndOffset = _reEndOffset.match( _ntpOutput, posOffset );
  posOffset += _offset.length() + 1;
  QString diff = _ntpOutput.mid(posOffset, posEndOffset-posOffset-1);
  qDebug("%s", _ntpOutput.latin1());
  qDebug("diff = >%s<",diff.latin1());
  TextLabelTimeShift->setText(diff);
  return diff.toFloat();
}