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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qdialog.h>

#include <opie2/oprocess.h>
#include <opie2/otabwidget.h>

class TimeTabWidget;
class FormatTabWidget;
class SettingsTabWidget;
class NTPTabWidget;
class PredictTabWidget;


class QDateTime;
class QSocket;
class QTimer;

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0,  bool modal = FALSE,WFlags f = 0);
    ~MainWindow();
    static QString appName() { return QString::fromLatin1("systemtime"); }

protected:
    void accept();
    void reject();

private:
    Opie::Ui::OTabWidget *mainWidget;

    TimeTabWidget     *timeTab;
    FormatTabWidget   *formatTab;
    SettingsTabWidget *settingsTab;
    NTPTabWidget      *ntpTab;
    PredictTabWidget  *predictTab;

    bool ntpTabEnabled;
    bool predictTabEnabled;

    Opie::Core::OProcess *ntpProcess;
    QTimer   *ntpTimer;
    QSocket  *ntpSock;
    int       ntpDelay;
    bool      ntpInteractive;
    QString   ntpOutput;
    int       _lookupDiff;

    void runNTP();
    bool ntpDelayElapsed();

private slots:
    void slotSetTime( const QDateTime & );
    void slotQCopReceive( const QCString &, const QByteArray & );
    void slotDisplayNTPTab( bool );
    void slotDisplayPredictTab( bool );
    void slotGetNTPTime();
    void slotTimerGetNTPTime();
    void slotProbeNTPServer();
    void slotNtpOutput( Opie::Core::OProcess *, char *, int );
    void slotNtpFinished( Opie::Core::OProcess* );
    void slotNTPDelayChanged( int );
    void slotCheckNtp( int );
};

#endif
