#include "ntp.h"
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtable.h>
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
    : SetDateTime( parent, name, fl )
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
  connect(PushButtonPredict, SIGNAL(clicked()), this, SLOT(preditctTime()));
// 	_nextCorrection = new QTimer( this );
//    connect( _nextCorrection, SIGNAL(timeout()), SLOT(correctClock()) );
  slotRunNtp();
  readLookups();
}

Ntp::~Ntp()
{

}

void Ntp::accept()
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
  }
}

void  Ntp::getNtpOutput(OProcess *proc, char *buffer, int buflen)
{
  QString lineStr, lineStrOld;
  lineStr = buffer;
  lineStr=lineStr.left(buflen);
  if (lineStr!=lineStrOld)
  {
//	        outPut->append(lineStr);
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
  cfg.setGroup("correction");
  cfg.writeEntry("time", time);
 	
  float timeShift = getTimeShift();
  if (timeShift == 0.0) return;
 	int secsSinceLast = time - lastLookup;
  TextLabelNewTime->setText(QDateTime::currentDateTime().toString());
  if ( lastLookup > 0 && secsSinceLast > 60*_minLookupDiff)
  {
	  lookupCount++;
   	cfg.writeEntry("count",lookupCount);
	  cfg.setGroup("lookup_"+QString::number(lookupCount));
    _shiftPerSec =  timeShift / secsSinceLast;
//    float nextCorr = _maxOffset / _shiftPerSec;
   	qDebug("secs since last lookup %i", secsSinceLast);qDebug("timeshift since last lookup %f", timeShift);qDebug("timeshift since per sec %f", _shiftPerSec);
		cfg.writeEntry("secsSinceLast",secsSinceLast);
		cfg.writeEntry("timeShift",QString::number(timeShift));
  }
}

//void Ntp::correctClock()
//{
//	qDebug("current time: %s",QDateTime::currentDateTime().toString().latin1());
//	Config cfg("ntp",Config::User);
//  cfg.setGroup("correction");
// 	int lastTime = cfg.readNumEntry("time",0);
//  if ( lastTime == 0 )
//  {
//   	return;
//  }
//  int now = TimeConversion::toUTC( QDateTime::currentDateTime() );
//  int corr = int((now - lastTime) * _shiftPerSec);
//  struct timeval myTv;
//  myTv.tv_sec = TimeConversion::toUTC( QDateTime::currentDateTime().addSecs(corr) );
//  myTv.tv_usec = 0;
//
//  if ( myTv.tv_sec != -1 )
//      ::settimeofday( &myTv, 0 );
//  Global::writeHWClock();
//  cfg.writeEntry("time",now);
//	qDebug("current time: %s",QDateTime::currentDateTime().toString().latin1());
//}

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

void Ntp::readLookups()
{
	Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
  int lookupCount = cfg.readNumEntry("count",-1);
  float last, shift, shiftPerSec;
  qDebug("lookupCount = %i",lookupCount);
  TableLookups->setNumCols( 3 );
  TableLookups->setNumRows( lookupCount);
  TableLookups->horizontalHeader()->setLabel(2,"secsSinceLast");
  TableLookups->horizontalHeader()->setLabel(1,"timeShift");
  TableLookups->horizontalHeader()->setLabel(0,"shift/s");
  int cw = TableLookups->width()/4;
  qDebug("column width %i",cw);
  TableLookups->setColumnWidth( 0, cw );
  TableLookups->setColumnWidth( 1, cw );
  TableLookups->setColumnWidth( 2, cw );
  for (int i=0; i < lookupCount; i++)
  {
	  cfg.setGroup("lookup_"+QString::number(i));
   	last = cfg.readEntry("secsSinceLast",0).toFloat();
    shift = QString(cfg.readEntry("timeShift",0)).toFloat();
   	qDebug("%i last %f",i,last);
   	qDebug("%i shift %f",i,shift);
    shiftPerSec =  shift / last;
    _shiftPerSec += shiftPerSec;
		TableLookups->setText( i,0,QString::number(shiftPerSec));
		TableLookups->setText( i,1,QString::number(shift));
	  TableLookups->setText( i,2,QString::number(last));
  }
  _shiftPerSec /= lookupCount+1;
  TextLabelShift->setText(QString::number(_shiftPerSec));
}

void Ntp::preditctTime()
{
	qDebug("current time: %s",QDateTime::currentDateTime().toString().latin1());
	Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
 	int lastTime = cfg.readNumEntry("time",0);
  setenv( "TZ", tz->currentZone(), 1 );
  int now = TimeConversion::toUTC( QDateTime::currentDateTime() );
  int corr = int((now - lastTime) * _shiftPerSec);
  QDateTime dt = QDateTime::currentDateTime().addSecs(corr);
  setTime(dt);
 	TextLabelPredTime->setText(dt.toString());
}
