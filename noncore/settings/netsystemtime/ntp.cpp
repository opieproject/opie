#include "ntp.h"
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtable.h>
#include <qlabel.h>
#include <qsocket.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qmultilineedit.h>
#include <opie/oprocess.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/timeconversion.h>
#include <qpe/tzselect.h>
#include <qpe/timestring.h>
#include <qpe/qpedialog.h>
#include <qpe/datebookdb.h>
#include <qpe/qcopenvelope_qws.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


Ntp::Ntp( QWidget* parent,  const char* name, WFlags fl )
  : SetDateTime( parent, name, fl )
{
  _interactive = false;
  Config ntpSrvs(QPEApplication::qpeDir()+"etc/ntpservers",Config::File);
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
  SpinBoxMinLookupDelay->setValue( cfg.readNumEntry("minLookupDiff",720) );
  SpinBoxNtpDelay->setValue( cfg.readNumEntry("ntpRefreshFreq",1440) );
  ComboNtpSrv->setCurrentItem( cfg.readNumEntry("ntpServer", 0) );

  //make tab order

  TabWidgetMain->removePage( tabMain );
  TabWidgetMain->removePage( tabManualSetTime );
  TabWidgetMain->removePage( TabSettings );
  TabWidgetMain->removePage( tabPredict );
  TabWidgetMain->removePage( tabNtp );

  TabWidgetMain->insertTab( tabMain, tr( "Main" ) );
  TabWidgetMain->insertTab( TabSettings, tr( "Settings" ) );
  TabWidgetMain->insertTab( tabPredict, tr( "Predict" ) );
  TabWidgetMain->insertTab( tabNtp, tr( "NTP" ) );
  NtpBaseLayout->addWidget( TabWidgetMain, 0, 0 );



  bool advMode = cfg.readBoolEntry("advancedFeatures", false );
  showAdvancedFeatures(advMode);
  CheckBoxAdvSettings->setChecked( advMode );
  connect( CheckBoxAdvSettings, SIGNAL( toggled( bool ) ),
	   SLOT( showAdvancedFeatures( bool ) ) );

  makeChannel();

  ntpTimer = new QTimer(this);

  ntpProcess = new OProcess( );
  connect( SpinBoxNtpDelay, SIGNAL( valueChanged(int) ),
	   SLOT(slotNtpDelayChanged(int)) );

  ntpSock = new QSocket( this );
  connect( ntpSock, SIGNAL( error(int) ),SLOT(slotCheckNtp(int)) );
  slotProbeNtpServer();

  connect ( ntpProcess, SIGNAL(receivedStdout(OProcess*,char*,int)),
	    this, SLOT(getNtpOutput(OProcess*,char*,int)));
  connect ( ntpProcess, SIGNAL(processExited(OProcess*)),
	    this, SLOT(ntpFinished(OProcess*)));
  connect(runNtp, SIGNAL(clicked()), this, SLOT(slotButtonRunNtp()));
  connect(PushButtonPredict, SIGNAL(clicked()), this, SLOT(preditctTime()));
  connect(PushButtonSetPredTime, SIGNAL(clicked()), this, SLOT(setPredictTime()));
  slotCheckNtp(-1);
  readLookups();
}

Ntp::~Ntp()
{
  delete ntpProcess;
}

void Ntp::saveConfig(){
  int srvCount = ComboNtpSrv->count();
  bool serversChanged = true;
  int curSrv = ComboNtpSrv->currentItem();
  QString edit = ComboNtpSrv->currentText();
  for (int i = 0; i < srvCount; i++){
       if ( edit ==  ComboNtpSrv->text(i)) serversChanged = false;
  }
  if (serversChanged){
    Config ntpSrvs(QPEApplication::qpeDir()+"etc/ntpservers",Config::File);
    ntpSrvs.setGroup("servers");
    ntpSrvs.writeEntry("count", ++srvCount);
    ntpSrvs.setGroup("0");
    ntpSrvs.writeEntry( "name", edit );
    curSrv = 0;
    for (int i = 1; i < srvCount; i++){
      qDebug("ntpSrvs[%i/%i]=%s",i,srvCount,ComboNtpSrv->text(i).latin1());
      ntpSrvs.setGroup(QString::number(i));
      ntpSrvs.writeEntry( "name", ComboNtpSrv->text(i-1) );
    }
  }
  Config cfg("ntp",Config::User);
  cfg.setGroup("settings");
  cfg.writeEntry("ntpServer", curSrv );
  cfg.writeEntry( "minLookupDiff", SpinBoxMinLookupDelay->value() );
  cfg.writeEntry( "ntpRefreshFreq", SpinBoxNtpDelay->value() );
  cfg.writeEntry( "advancedFeatures", CheckBoxAdvSettings->isChecked() );
}

bool Ntp::ntpDelayElapsed()
{
  Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
  _lookupDiff = TimeConversion::toUTC(QDateTime::currentDateTime()) - cfg.readNumEntry("time",0);
  if (_lookupDiff < 0) return true;
  int i =_lookupDiff - (SpinBoxNtpDelay->value()*60);
  return i > -60;
}

QString Ntp::getNtpServer()
{
  return ComboNtpSrv->currentText();
}

void Ntp::slotButtonRunNtp()
{
  _interactive = true;
  slotRunNtp();
}

void Ntp::slotTimerRunNtp()
{
  _interactive = false;
  slotRunNtp();
}


void Ntp::slotRunNtp()
{
  if ( !ntpDelayElapsed() && CheckBoxAdvSettings->isChecked() )
    {
      switch (
	      QMessageBox::warning(this, tr("Run NTP?"),
				   tr("You asked for a delay of ")+SpinBoxNtpDelay->text()+tr(" minutes, but only ")+
				   QString::number(_lookupDiff/60)+tr(" minutes elapsed since last lookup.")+
				   "<br>"+tr("Rerun NTP?"),
				   QMessageBox::Ok,QMessageBox::Cancel)
	      ) {
      case QMessageBox::Ok:     break;
      case QMessageBox::Cancel: return;
      default: return;
      }
    }
  TextLabelStartTime->setText(QDateTime::currentDateTime().toString());
  ntpOutPut( tr("Running:")+"\nntpdate "+getNtpServer() );

  ntpProcess->clearArguments();
  *ntpProcess << "ntpdate" << getNtpServer();
  bool ret = ntpProcess->start(OProcess::NotifyOnExit,OProcess::AllOutput);
  if ( !ret ) {
    QMessageBox::critical(this, tr("ntp error"),
		       tr("Error while getting time form network!"));
    qDebug("Error while executing ntpdate");
    ntpOutPut( tr("Error while executing ntpdate"));
  }
}

void  Ntp::getNtpOutput(OProcess *proc, char *buffer, int buflen)
{
  if (! proc ) qDebug("Ntp::getNtpOutput OProcess is null");
  QString lineStr, lineStrOld;
  lineStr = buffer;
  lineStr=lineStr.left(buflen);
  if (lineStr!=lineStrOld)
    {
      ntpOutPut(lineStr);
      _ntpOutput += lineStr;
    }
  lineStrOld = lineStr;
}

void  Ntp::ntpFinished(OProcess *p)
{
  qDebug("p->exitStatus() %i",p->exitStatus());
  if (p->exitStatus()!=0 || !p->normalExit())
    {
      if ( isVisible() && _interactive ){
        QMessageBox::critical(this, tr("ntp error"),tr("Error while getting time form\n server")+getNtpServer()+"\n"+_ntpOutput );
      }
//      slotCheckNtp(-1);
      return;
    }

  Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
  int lastLookup = cfg.readNumEntry("time",0);
  int lookupCount = cfg.readNumEntry("count",0);
  bool lastNtp = cfg.readBoolEntry("lastNtp",false);
  int time = TimeConversion::toUTC( QDateTime::currentDateTime() );
  cfg.writeEntry("time", time);

  float timeShift = getTimeShift();
  if (timeShift == 0.0) return;
  int secsSinceLast = time - lastLookup;
  TextLabelNewTime->setText(QDateTime::currentDateTime().toString());
  TextLabelTimeShift->setText(QString::number(timeShift)+tr(" seconds"));
  if ( lastNtp && lastLookup > 0 && secsSinceLast > 60* SpinBoxMinLookupDelay->value())
    {
      cfg.setGroup("lookup_"+QString::number(lookupCount));
      lookupCount++;
      _shiftPerSec =  timeShift / secsSinceLast;
      qDebug("secs since last lookup %i", secsSinceLast);qDebug("timeshift since last lookup %f", timeShift);qDebug("timeshift since per sec %f", _shiftPerSec);
      cfg.writeEntry("secsSinceLast",secsSinceLast);
      cfg.writeEntry("timeShift",QString::number(timeShift));
      cfg.setGroup("lookups");
      cfg.writeEntry("count",lookupCount);
      cfg.writeEntry("lastNtp",true);
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
  int lookupCount = cfg.readNumEntry("count",0);
  float last, shift, shiftPerSec;
  qDebug("lookupCount = %i",lookupCount);
  TableLookups->setNumCols( 3 );
  TableLookups->setNumRows( lookupCount);
  TableLookups->horizontalHeader()->setLabel(1,tr("last [h]"));
  TableLookups->horizontalHeader()->setLabel(2,tr("offset [s]"));
  TableLookups->horizontalHeader()->setLabel(0,tr("shift [s/h]"));
  int cw = TableLookups->width()/4;
  qDebug("column width %i",cw);
  cw = 50;
  TableLookups->setColumnWidth( 0, cw+30 );
  TableLookups->setColumnWidth( 1, cw );
  TableLookups->setColumnWidth( 2, cw );
  TableLookups->sortColumn(0, false, true );
  // TableLookups->setSorting( true );
  _shiftPerSec = 0;
  for (int i=0; i < lookupCount; i++)
    {
      cfg.setGroup("lookup_"+QString::number(i));
      last = cfg.readEntry("secsSinceLast",0).toFloat();
      shift = QString(cfg.readEntry("timeShift",0)).toFloat();
      shiftPerSec =  shift / last;
      qDebug("%i shift %f",i,shiftPerSec);
      _shiftPerSec += shiftPerSec;
      TableLookups->setText( i,0,QString::number(shiftPerSec*60*60));
      TableLookups->setText( i,2,QString::number(shift));
      TableLookups->setText( i,1,QString::number(last/(60*60)));
    }
  _shiftPerSec /= lookupCount;
  TextLabelShift->setText(QString::number(_shiftPerSec*60*60)+tr(" s/h"));
}

void Ntp::preditctTime()
{
  Config cfg("ntp",Config::User);
  cfg.setGroup("lookups");
  int lastTime = cfg.readNumEntry("time",0);
  cfg.writeEntry("lastNtp",true);
  setenv( "TZ", tz->currentZone(), 1 );
  int now = TimeConversion::toUTC( QDateTime::currentDateTime() );
  int corr = int((now - lastTime) * _shiftPerSec);
  TextLabelEstimatedShift->setText(QString::number(corr)+tr(" seconds"));
  predictedTime = QDateTime::currentDateTime().addSecs(corr);
  TextLabelPredTime->setText(predictedTime.toString());
//  TextLabelMainPredTime->setText(tr("Predicted time:")+"<br><b>"+predictedTime.toString()+"</b>");
}

void Ntp::setPredictTime()
{
  qDebug("Ntp::setPredictTime");
  preditctTime();
  timeButton->setTime( predictedTime );
}

void Ntp::slotCheckNtp(int i)
{
  qDebug(" Ntp::slotCheckNtp(%i)",i);
  if (i == 0)
    {
//      TextLabelMainPredTime->hide();
      ButtonSetTime->setText( tr("Get time from network") );
      connect( ButtonSetTime, SIGNAL(clicked()), SLOT(slotButtonRunNtp()) );
      if ( ntpDelayElapsed() )
      {
        slotRunNtp();
        disconnect(ntpTimer, SIGNAL( timeout() ), this, SLOT(slotProbeNtpServer()) );
        connect(ntpTimer, SIGNAL( timeout() ), SLOT(slotTimerRunNtp()) );
      }else{
        disconnect(ntpTimer, SIGNAL( timeout() ), this, SLOT(slotTimerRunNtp()) );
        connect(ntpTimer, SIGNAL( timeout() ), SLOT(slotProbeNtpServer()) );
      }
    }else{
      preditctTime();
      ButtonSetTime->setText( tr("Set predicted time: ")+predictedTime.toString() );
      if (i>0)ntpOutPut(tr("Could not connect to server ")+getNtpServer());
      connect( ButtonSetTime, SIGNAL(clicked()), SLOT(setPredictTime()) );
      connect( ntpTimer, SIGNAL( timeout() ), SLOT(slotProbeNtpServer()) );
    }
}

void Ntp::slotProbeNtpServer()
{
  ntpSock->connectToHost( getNtpServer() ,123);
}

void Ntp::slotNtpDelayChanged(int delay)
{
  ntpTimer->changeInterval( delay*1000*60 );
}

void Ntp::ntpOutPut(QString out)
{

  MultiLineEditntpOutPut->append(out);
  MultiLineEditntpOutPut->setCursorPosition(MultiLineEditntpOutPut->numLines() + 1,0,FALSE);
}


void Ntp::makeChannel()
{
  channel = new QCopChannel( "QPE/Application/netsystemtime", this );
  connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	   this, SLOT(receive(const QCString&, const QByteArray&)) );
}



void Ntp::receive(const QCString &msg, const QByteArray &arg)
{
  qDebug("QCop(Ntp) "+msg+" "+QCString(arg));
  if ( msg == "ntpLookup(QString)" )
    {
      _interactive = false;
      slotRunNtp();
    }
  if ( msg == "setPredictedTime(QString)" )
    {
      setPredictTime();
    }else{
      qDebug("Ntp::receive: Huh what do ya want");
    }
}

void Ntp::setDocument(const QString &fileName)
{
  qDebug("Ntp::setDocument( %s )",fileName.latin1());
}

void Ntp::showAdvancedFeatures(bool advMode)
{
  if  (advMode) {

    if ( tabNtp->isVisible() ) {
        TabWidgetMain->addTab( tabPredict, tr( "Predict" ) );
        TabWidgetMain->addTab( tabNtp, tr( "NTP" ) );
    }
    TextLabel1_2_2->show();
    TextLabel2_3->show();
    TextLabel3_3_2->show();
    TextLabel1_2->show();
    SpinBoxMinLookupDelay->show();
    TextLabel2->show();
    TextLabel3_3->show();
    SpinBoxNtpDelay->show();
    Line1->show();
  }else{
    TabWidgetMain->removePage( tabPredict );
    TabWidgetMain->removePage( tabNtp );
    TextLabel1_2_2->hide();
    TextLabel2_3->hide();
    TextLabel3_3_2->hide();
    TextLabel1_2->hide();
    SpinBoxMinLookupDelay->hide();
    TextLabel2->hide();
    TextLabel3_3->hide();
    SpinBoxNtpDelay->hide();
    Line1->hide();
  };
  TabWidgetMain->show();
}


void Ntp::accept( ){
    qDebug("saving");
    //SetTimeDate
    commitTime();
  	writeSettings();
    updateSystem();
    // Ntp
    saveConfig();
    qApp->quit();
}

void Ntp::reject( ){
    qDebug("_oldTimeZone %s",_oldTimeZone.latin1());
    if (!_oldTimeZone.isEmpty()){
      qDebug("reverting timezone");
      tzChange(_oldTimeZone);
      commitTime();
    }
}
