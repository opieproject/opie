#include "ntp.h"
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtable.h>
#include <qlabel.h>
#include <qsocket.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>
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
 	 	
	Config ntpSrvs("/etc/ntpservers",Config::File);
  ntpSrvs.setGroup("servers");
  int srvCount = ntpSrvs.readNumEntry("count", 0 );
  for (int i = 0; i < srvCount; i++)
  {
	  ntpSrvs.setGroup(QString::number(i));
   	ComboNtpSrv->insertItem( ntpSrvs.readEntry("name") );
  }
  if ( srvCount==0 ) ComboNtpSrv->insertItem(tr("time.fu-berlin.de"));

	Config cfg("ntp",Config::User);
  cfg.setGroup("settings");
// 	_maxOffset = cfg.readNumEntry("maxOffset",5);
//  _minLookupDiff = cfg.readNumEntry("minLookupDiff",10);
	SpinBoxMinLookupDelay->setValue( cfg.readNumEntry("minLookupDiff",41) );
	SpinBoxNtpDelay->setValue( cfg.readNumEntry("ntpRefreshFreq",42) );
  ComboNtpSrv->setCurrentItem( cfg.readNumEntry("ntpServer", 0) );

  ntpTimer = new QTimer(this);
	ntpTimer->start(SpinBoxNtpDelay->value()*100);

 	ntpProcess = new OProcess( );
  connect( SpinBoxNtpDelay, SIGNAL( valueChanged(int) ),
  					SLOT(slotNtpDelayChanged(int)) );

  ntpSock = new QSocket( this );
  connect( ntpSock, SIGNAL( error(int) ),
  					SLOT(slotCheckNtp(int)) );
  slotProbeNtpServer();

  connect ( ntpProcess, SIGNAL(receivedStdout(OProcess*,char*,int)),
  					this, SLOT(getNtpOutput(OProcess*,char*,int)));
  connect ( ntpProcess, SIGNAL(processExited(OProcess*)),
  					this, SLOT(ntpFinished(OProcess*)));
  connect(runNtp, SIGNAL(clicked()), this, SLOT(slotRunNtp()));
  connect(PushButtonPredict, SIGNAL(clicked()), this, SLOT(preditctTime()));
  connect(PushButtonSetPredTime, SIGNAL(clicked()), this, SLOT(setPredictTime()));
  slotCheckNtp(-1);
  readLookups();
}

Ntp::~Ntp()
{
	delete ntpProcess;
	Config ntpSrvs("/etc/ntpservers",Config::File);
  ntpSrvs.setGroup("servers");
  int srvCount = ComboNtpSrv->count();
  ntpSrvs.writeEntry("count", srvCount);
  for (int i = 0; i < srvCount; i++)
  {
	  ntpSrvs.setGroup(QString::number(i));
   	ntpSrvs.writeEntry( "name", ComboNtpSrv->text(i) );
  }
	Config cfg("ntp",Config::User);
  cfg.setGroup("settings");
  cfg.writeEntry("ntpServer", ComboNtpSrv->currentItem());
  cfg.writeEntry( "minLookupDiff", SpinBoxMinLookupDelay->value() );
 	cfg.writeEntry( "ntpRefreshFreq", SpinBoxNtpDelay->value() );
}


void Ntp::slotRunNtp()
{
	Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
  int lookupDiff = TimeConversion::toUTC(QDateTime::currentDateTime()) - cfg.readNumEntry("time",0);

  if ( lookupDiff < SpinBoxNtpDelay->value()*60 )
  {
  	switch (
   	 QMessageBox::warning(this, tr("Run ntp?"),
     tr("You asked for a delay of ")+SpinBoxNtpDelay->text()+tr(" minutes, but only ")+
     QString::number(lookupDiff%60)+tr(" minutes elapsed since last loopup.")+
     "<br>"+tr("Rerun ntp?"),
     QMessageBox::Ok,QMessageBox::Cancel)
   	) {
	    	case QMessageBox::Ok:     break;
  	   	case QMessageBox::Cancel: return;
    	  default: return;
      }
  }
	TextLabelStartTime->setText(QDateTime::currentDateTime().toString());
  MultiLineEditntpOutPut->append( "\n"+tr("Running:")+"\nntpdate "+ ComboNtpSrv->currentText()+"\n");
	ntpProcess->clearArguments();
	*ntpProcess << "ntpdate" << ComboNtpSrv->currentText();
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
	        MultiLineEditntpOutPut->append(lineStr);
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
  TextLabelTimeShift->setText(QString::number(timeShift)+tr(" seconds"));
  if ( lastLookup > 0 && secsSinceLast > 60* SpinBoxMinLookupDelay->value())
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
  TableLookups->horizontalHeader()->setLabel(1,"secsSinceLast");
  TableLookups->horizontalHeader()->setLabel(2,"timeShift");
  TableLookups->horizontalHeader()->setLabel(0,"shift/s");
  int cw = 50;//TableLookups->width()/4;
  qDebug("column width %i",cw);
  TableLookups->setColumnWidth( 0, cw+30 );
  TableLookups->setColumnWidth( 1, cw );
  TableLookups->setColumnWidth( 2, cw );
  TableLookups->sortColumn(0, false, true );
 // TableLookups->setSorting( true );
  for (int i=0; i < lookupCount; i++)
  {
	  cfg.setGroup("lookup_"+QString::number(i));
   	last = cfg.readEntry("secsSinceLast",0).toFloat();
    shift = QString(cfg.readEntry("timeShift",0)).toFloat();
//   	qDebug("%i last %f",i,last);
//   	qDebug("%i shift %f",i,shift);
    shiftPerSec =  shift / last;
    _shiftPerSec += shiftPerSec;
		TableLookups->setText( i,0,QString::number(shiftPerSec));
		TableLookups->setText( i,2,QString::number(shift));
	  TableLookups->setText( i,1,QString::number(last));
  }
  _shiftPerSec /= lookupCount+1;
  TextLabelShift->setText(QString::number(_shiftPerSec)+tr(" seconds"));
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
  TextLabelEstimatedShift->setText(QString::number(corr)+tr(" seconds"));
  predictedTime = QDateTime::currentDateTime().addSecs(corr);
 	TextLabelPredTime->setText(predictedTime.toString());
  TextLabelMainPredTime->setText(tr("Predicted time:")+"<br><b>"+predictedTime.toString()+"</b>");
}

void Ntp::setPredictTime()
{
  preditctTime();
 	setTime( predictedTime );
}

void Ntp::slotCheckNtp(int i)
{
	if (i == 0)
 	{
    TextLabelMainPredTime->hide();
    ButtonSetTime->setText( tr("Get time from network") );
	  connect( ButtonSetTime, SIGNAL(clicked()), SLOT(slotRunNtp()) );
		Config cfg("ntp",Config::User);
  	cfg.setGroup("lookups");
		int lookupDiff = TimeConversion::toUTC(QDateTime::currentDateTime()) - cfg.readNumEntry("time",0);
  	if ( lookupDiff > SpinBoxNtpDelay->value()*60 )
   	{
	  	disconnect(ntpTimer, SIGNAL( timeout() ), this, SLOT(slotProbeNtpServer()) );
    	connect(ntpTimer, SIGNAL( timeout() ), SLOT(slotRunNtp()) );
	  }else{
    	disconnect(ntpTimer, SIGNAL( timeout() ), this, SLOT(slotRunNtp()) );
		  connect(ntpTimer, SIGNAL( timeout() ), SLOT(slotProbeNtpServer()) );
    }
  }else{
	  preditctTime();
		ButtonSetTime->setText( tr("Set predicted time") );
  	connect( ButtonSetTime, SIGNAL(clicked()), SLOT(setPredictTime()) );
	  connect( ntpTimer, SIGNAL( timeout() ), SLOT(slotProbeNtpServer()) );
  }
}

void Ntp::slotProbeNtpServer()
{
	Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
	int lookupDiff = TimeConversion::toUTC(QDateTime::currentDateTime()) - cfg.readNumEntry("time",0);
  if ( lookupDiff > SpinBoxNtpDelay->value()*60 )
  ntpSock->connectToHost( ComboNtpSrv->currentText() ,123);
}

void Ntp::slotNtpDelayChanged(int delay)
{
  ntpTimer->changeInterval( delay*100 );
}
