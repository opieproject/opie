/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 OPIE team <opie@handhelds.org?>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "mainwindow.h"
#include "timetabwidget.h"
#include "formattabwidget.h"
#include "settingstabwidget.h"
#include "ntptabwidget.h"
#include "predicttabwidget.h"

#include <qpe/config.h>
#include <qpe/datebookdb.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpedialog.h>

#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qlayout.h>
#include <qmessagebox.h>
#include <qsocket.h>
#include <qstring.h>
#include <qtimer.h>

using namespace Opie::Ui;
using namespace Opie::Core;
MainWindow::MainWindow( QWidget *parent , const char *name,  bool modal, WFlags f )
    : QDialog( 0x0, 0x0, TRUE, 0 )
{
    setCaption( tr( "SystemTime" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 2 );
    layout->setSpacing( 4 );

    // Create main tabbed control
    mainWidget = new OTabWidget( this );

    // Default object pointers to null
    ntpProcess = 0x0;
    ntpTab = 0x0;

    // Add tab widgets
    mainWidget->addTab( timeTab = new TimeTabWidget( mainWidget ), "netsystemtime/DateTime", tr( "Time" ) );
    mainWidget->addTab( formatTab = new FormatTabWidget( mainWidget ), "netsystemtime/formattab", tr( "Format" ) );
    mainWidget->addTab( settingsTab = new SettingsTabWidget( mainWidget ), "SettingsIcon", tr( "Settings" ) );
    mainWidget->addTab( predictTab = new PredictTabWidget( mainWidget ), "netsystemtime/predicttab", tr( "Predict" ) );
    Config config( "ntp" );
    config.setGroup( "settings" );
    slotDisplayNTPTab( config.readBoolEntry( "displayNtpTab", FALSE ) );
    slotDisplayPredictTab( config.readBoolEntry( "displayPredictTab", FALSE ) );

    mainWidget->setCurrentTab( tr( "Time" ) );
    layout->addWidget( mainWidget );

    connect( qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
        this, SLOT(slotQCopReceive(const QCString&,const QByteArray&)) );


    // Create NTP socket
    ntpSock = new QSocket( this );
    connect( ntpSock, SIGNAL(error(int)),SLOT(slotCheckNtp(int)) );
    slotProbeNTPServer();

    // Create timer for automatic time lookups
    ntpTimer = new QTimer( this );

    // Connect everything together
    connect( timeTab, SIGNAL(getNTPTime()), this, SLOT(slotGetNTPTime()) );
    connect( timeTab, SIGNAL(tzChanged(const QString&)), predictTab, SLOT(slotTZChanged(const QString&)) );
    connect( timeTab, SIGNAL(getPredictedTime()), predictTab, SLOT(slotSetPredictedTime()) );
    connect( formatTab, SIGNAL(show12HourTime(int)), timeTab, SLOT(slotUse12HourTime(int)) );
    connect( formatTab, SIGNAL(dateFormatChanged(const DateFormat&)),
             timeTab, SLOT(slotDateFormatChanged(const DateFormat&)) );
    connect( formatTab, SIGNAL(weekStartChanged(int)), timeTab, SLOT(slotWeekStartChanged(int)) );
    connect( settingsTab, SIGNAL(ntpDelayChanged(int)), this, SLOT(slotNTPDelayChanged(int)) );
    connect( settingsTab, SIGNAL(displayNTPTab(bool)), this, SLOT(slotDisplayNTPTab(bool)) );
    connect( settingsTab, SIGNAL(displayPredictTab(bool)), this, SLOT(slotDisplayPredictTab(bool)) );
    connect( predictTab, SIGNAL(setTime(const QDateTime&)), this, SLOT(slotSetTime(const QDateTime&)) );

    // Do initial time server check
    slotNTPDelayChanged( config.readNumEntry( "ntpRefreshFreq", 1440 ) );
    slotCheckNtp( -1 );

    // Display app
    //showMaximized();
    (void)new QPEDialogListener(this);
}

MainWindow::~MainWindow()
{
    if ( ntpProcess )
        delete ntpProcess;
}

void MainWindow::accept()
{
    // Turn off the screensaver (Note: needs to be encased in { } so that it deconstructs and sends)
    {
        QCopEnvelope disableScreenSaver( "QPE/System", "setScreenSaverIntervals(int,int,int)" );
        disableScreenSaver << 0 << 0 << 0;
    }

    // Update the systemtime
    timeTab->saveSettings( TRUE );

    // Save format options
    formatTab->saveSettings( TRUE );

    // Save settings options
    settingsTab->saveSettings();

    // Since time has changed quickly load in the DateBookDB to allow the alarm server to get a better
    // grip on itself (example re-trigger alarms for when we travel back in time).
    DateBookDB db;

    // Turn back on the screensaver
    QCopEnvelope enableScreenSaver( "QPE/System", "setScreenSaverIntervals(int,int,int)" );
    enableScreenSaver << -1 << -1 << -1;

    // Exit app
    qApp->quit();
}

void MainWindow::reject()
{
    // Reset time settings
    timeTab->saveSettings( FALSE );

    // Send notifications but do not save settings
    formatTab->saveSettings( FALSE );

    // Exit app
    qApp->quit();
}

void MainWindow::runNTP()
{
    if ( !ntpDelayElapsed() && ntpInteractive )
    {
        QString msg = tr( "You asked for a delay of %1 minutes, but only %2 minutes elapsed since last lookup.<br>Continue?" ).arg( QString::number( ntpDelay ) ).arg( QString::number( _lookupDiff / 60 ) );

        switch (
            QMessageBox::warning( this, tr( "Continue?" ), msg, QMessageBox::Yes, QMessageBox::No )
            )
        {
            case QMessageBox::Yes: break;
            case QMessageBox::No:  return;
            default: return;
        }
    }

    QString srv = settingsTab->ntpServer();

    // Send information to time server tab if enabled
    if ( ntpTabEnabled )
    {
        ntpTab->setStartTime( QDateTime::currentDateTime().toString() );
        QString output = tr( "Running:\nntpdate " );
        output.append( srv );
        ntpTab->addNtpOutput( output );
    }

    if ( !ntpProcess )
    {
        ntpProcess = new OProcess();
        connect( ntpProcess, SIGNAL(receivedStdout(Opie::Core::OProcess*,char*,int)),
                 this, SLOT(slotNtpOutput(OProcess*,char*,int)) );
        connect( ntpProcess, SIGNAL(processExited(Opie::Core::OProcess*)),
                 this, SLOT(slotNtpFinished(OProcess*)) );
    }

    else
        ntpProcess->clearArguments();

    *ntpProcess << "ntpdate"  << srv;
    bool ret = ntpProcess->start( OProcess::NotifyOnExit, OProcess::AllOutput );
    if ( !ret )
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "Error while getting time from network." ) );
        if ( ntpTabEnabled )
            ntpTab->addNtpOutput( tr( "Error while executing ntpdate" ) );
    }
}

bool MainWindow::ntpDelayElapsed()
{
    // Determine if time elapsed is greater than time delay
    Config config( "ntp" );
    config.setGroup( "lookups" );
    _lookupDiff = TimeConversion::toUTC( QDateTime::currentDateTime() ) - config.readNumEntry( "time", 0 );
    if ( _lookupDiff < 0 )
        return true;
    return ( _lookupDiff - ( ntpDelay * 60) ) > 0;
}

void MainWindow::slotSetTime( const QDateTime &dt )
{
    timeTab->setDateTime( dt );
}

void MainWindow::slotQCopReceive( const QCString &msg, const QByteArray & )
{
    if ( msg == "ntpLookup(QString)" )
    {
        ntpInteractive = false;
        runNTP();
    }
    if ( msg == "setPredictedTime(QString)" )
    {
        //setPredictTime();
    }
}

void MainWindow::slotDisplayNTPTab( bool display )
{
    ntpTabEnabled = display;

    // Create widget if it hasn't needed
    if ( display && !ntpTab )
    {
        ntpTab = new NTPTabWidget( mainWidget );
        connect( ntpTab, SIGNAL(getNTPTime()), this, SLOT(slotGetNTPTime()) );
    }

    // Display/hide tab
    display ? mainWidget->addTab( ntpTab, "netsystemtime/ntptab", tr( "Time Server" ) )
            : mainWidget->removePage( ntpTab );
}

void MainWindow::slotDisplayPredictTab( bool display )
{
    predictTabEnabled = display;

    // Create widget if it hasn't needed
    if ( display && !predictTab )
    {
    }
    // Display/hide tab
    display ? mainWidget->addTab( predictTab, "netsystemtime/predicttab", tr( "Predict" ) )
            : mainWidget->removePage( predictTab );
}

void MainWindow::slotGetNTPTime()
{
    ntpInteractive = TRUE;
    runNTP();
}

void MainWindow::slotTimerGetNTPTime()
{
    ntpInteractive = FALSE;
    runNTP();
}

void MainWindow::slotProbeNTPServer()
{
    ntpSock->connectToHost( settingsTab->ntpServer(), 123 );
}

void MainWindow::slotNtpOutput( OProcess *, char *buffer, int buflen )
{
    QString output = QString( buffer ).left( buflen );
    ntpOutput.append( output );

    if ( ntpTabEnabled )
        ntpTab->addNtpOutput( output );
}

void MainWindow::slotNtpFinished( OProcess *p )
{
    QString output;
    QDateTime dt = QDateTime::currentDateTime();

    //  Verify run was successful
    if ( p->exitStatus() != 0 || !p->normalExit() )
    {
        if ( isVisible() && ntpInteractive )
        {
            output = tr( "Error while getting time from\n server: " );
            output.append( settingsTab->ntpServer() );
            QMessageBox::critical(this, tr( "Error" ), output );
        }
    //      slotCheckNtp(-1);
        return;
    }

    // Set controls on time tab to new time value
    timeTab->setDateTime( dt );

    // Write out lookup information
    Config config( "ntp" );
    config.setGroup( "lookups" );
    int lastLookup = config.readNumEntry( "time", 0 );
    int lookupCount = config.readNumEntry( "count", 0 );
    bool lastNtp = config.readBoolEntry( "lastNtp", FALSE );
    int time = TimeConversion::toUTC( QDateTime::currentDateTime() );
    config.writeEntry( "time", time );

    // Calculate new time/time shift
    QString _offset = "offset";
    QString _sec = "sec";
    QRegExp _reOffset = QRegExp( _offset );
    QRegExp _reEndOffset = QRegExp( _sec );
    int posOffset = _reOffset.match(  ntpOutput );
    int posEndOffset = _reEndOffset.match(  ntpOutput, posOffset );
    posOffset += _offset.length() + 1;
    QString diff =  ntpOutput.mid( posOffset, posEndOffset - posOffset - 1 );

    float timeShift = diff.toFloat();
    if ( timeShift == 0.0 )
        return;
    int secsSinceLast = time - lastLookup;
    output = tr( "%1 seconds").arg(QString::number( timeShift ));

    // Display information on time server tab
    if ( ntpTabEnabled )
    {
        ntpTab->setTimeShift( output );
        ntpTab->setNewTime( dt.toString() );
    }

    if ( lastNtp && lastLookup > 0 && secsSinceLast > 60 * ntpDelay )
    {
        QString grpname = QString( "lookup_" ).append( QString::number( lookupCount ) );
        config.setGroup( grpname );
        lookupCount++;
        predictTab->setShiftPerSec( timeShift / secsSinceLast );
        config.writeEntry( "secsSinceLast", secsSinceLast );
        config.writeEntry( "timeShift", QString::number( timeShift ) );
        config.setGroup( "lookups" );
        config.writeEntry( "count", lookupCount );
        config.writeEntry( "lastNtp", TRUE );
    }
}

void MainWindow::slotNTPDelayChanged( int delay )
{
    ntpTimer->changeInterval( delay * 1000 * 60 );
    ntpDelay = delay;
}

void MainWindow::slotCheckNtp( int i )
{
    if ( i == 0 )
    {
        if ( ntpDelayElapsed() )
        {
            runNTP();
            disconnect( ntpTimer, SIGNAL(timeout()), this, SLOT(slotProbeNTPServer()) );
            connect( ntpTimer, SIGNAL(timeout()), SLOT(slotTimerGetNTPTime()) );
        }
        else
        {
            disconnect(ntpTimer, SIGNAL(timeout()), this, SLOT(slotTimerGetNTPTime()) );
            connect(ntpTimer, SIGNAL(timeout()), SLOT(slotProbeNTPServer()) );
        }
    }
    else
    {
        predictTab->slotPredictTime();
        if ( i > 0 )
        {
            QString output = tr( "Could not connect to server " );
            output.append( settingsTab->ntpServer() );
            ntpOutput.append( output );
            if ( ntpTabEnabled )
                ntpTab->addNtpOutput( output );
        }
        connect( ntpTimer, SIGNAL(timeout()), SLOT(slotProbeNTPServer()) );
    }
}
