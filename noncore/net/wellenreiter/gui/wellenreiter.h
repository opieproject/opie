/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef WELLENREITER_H
#define WELLENREITER_H

#include "wellenreiterbase.h"

#ifdef QWS
#include <opie/odevice.h>
using namespace Opie;
#endif

class QTimerEvent;
class QPixmap;
class OPacket;
class OWaveLanManagementPacket;
class OWaveLanDataPacket;
class OPacketCapturer;
class OWirelessNetworkInterface;
class ManufacturerDB;
class WellenreiterConfigWindow;
class MLogWindow;
class MHexWindow;

class Wellenreiter : public WellenreiterBase {
    Q_OBJECT

  public:
    Wellenreiter( QWidget* parent = 0 );
    ~Wellenreiter();

    void setConfigWindow( WellenreiterConfigWindow* cw );
    MScanListView* netView() const { return netview; };
    MLogWindow* logWindow() const { return logwindow; };
    MHexWindow* hexWindow() const { return hexwindow; };
    bool isDaemonRunning() const { return sniffing; };

    bool sniffing;

  protected:
    virtual void timerEvent( QTimerEvent* );

  public slots:
    void channelHopped(int);
    void receivePacket(OPacket*);
    void startClicked();
    void stopClicked();

  signals:
    void startedSniffing();
    void stoppedSniffing();

  private:
    void handleBeacon( OPacket* p, OWaveLanManagementPacket* beacon );
    void handleData( OPacket* p, OWaveLanDataPacket* data );
    void handleNotification( OPacket* p );
    void doAction( const QString& action, const QString& protocol, OPacket* p );
    QObject* childIfToParse( OPacket* p, const QString& protocol );

  private:
    #ifdef QWS
    OSystem _system;                // Opie Operating System identifier
    #endif

    OWirelessNetworkInterface* iface;
    OPacketCapturer* pcap;
    ManufacturerDB* manufacturerdb;
    WellenreiterConfigWindow* configwindow;

    //void readConfig();
    //void writeConfig();
};



#endif
