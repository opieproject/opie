/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
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
#include <opie2/odevice.h>
#endif

#include <signal.h>

class QTimerEvent;
class QPixmap;
namespace Opie {namespace Net {class OPacket;}}
namespace Opie {namespace Net {class OWaveLanManagementPacket;}}
namespace Opie {namespace Net {class OWaveLanControlPacket;}}
namespace Opie {namespace Net {class OWaveLanDataPacket;}}
namespace Opie {namespace Net {class OEthernetPacket;}}
namespace Opie {namespace Net {class OARPPacket;}}
namespace Opie {namespace Net {class OMacAddress;}}
namespace Opie {namespace Net {class OIPPacket;}}
namespace Opie {namespace Net {class OPacket;}}
namespace Opie {namespace Net {class OWirelessNetworkInterface;}}
namespace Opie {namespace Net {class OPacketCapturer;}}
class PacketView;
class WellenreiterConfigWindow;
class MLogWindow;
class GPS;

class Wellenreiter : public WellenreiterBase {
    Q_OBJECT

  public:
    Wellenreiter( QWidget* parent = 0 );
    ~Wellenreiter();

    void setConfigWindow( WellenreiterConfigWindow* cw );
    MScanListView* netView() const { return netview; };
    MLogWindow* logWindow() const { return logwindow; };
    PacketView* hexWindow() const { return hexwindow; };
    bool isDaemonRunning() const { return sniffing; };
    QString captureFileName() const { return dumpname; };

  public:
    QString dumpname;
    bool sniffing;
    static Wellenreiter* instance;
    static void signalHandler( int sig );

  protected:
    virtual void timerEvent( QTimerEvent* );

  public slots:
    void initialTimer();

    void channelHopped(int);
    void receivePacket(Opie::Net::OPacket*);
    void startClicked();
    void stopClicked();

    void joinNetwork(const QString&,const QString&,int,const QString&);

  signals:
    void startedSniffing();
    void stoppedSniffing();

  private:
    void handleManagementFrame( Opie::Net::OPacket* p, Opie::Net::OWaveLanManagementPacket* );
    void handleManagementFrameBeacon( Opie::Net::OPacket* p, Opie::Net::OWaveLanManagementPacket* );
    void handleManagementFrameProbeRequest( Opie::Net::OPacket* p, Opie::Net::OWaveLanManagementPacket* );
    void handleManagementFrameProbeResponse( Opie::Net::OPacket* p, Opie::Net::OWaveLanManagementPacket* );
    void handleControlFrame( Opie::Net::OPacket* p, Opie::Net::OWaveLanControlPacket* control );
    void handleWlanData( Opie::Net::OPacket* p, Opie::Net::OWaveLanDataPacket* data, Opie::Net::OMacAddress& from, Opie::Net::OMacAddress& to );
    void handleEthernetData( Opie::Net::OPacket* p, Opie::Net::OEthernetPacket* data, Opie::Net::OMacAddress& from, Opie::Net::OMacAddress& to );
    void handleARPData( Opie::Net::OPacket* p, Opie::Net::OARPPacket* arp, Opie::Net::OMacAddress& from, Opie::Net::OMacAddress& to );
    void handleIPData( Opie::Net::OPacket* p, Opie::Net::OIPPacket* ip, Opie::Net::OMacAddress& from, Opie::Net::OMacAddress& to );
    void handleNotification( Opie::Net::OPacket* p );
    void doAction( const QString& action, const QString& protocol, Opie::Net::OPacket* p );
    QObject* childIfToParse( Opie::Net::OPacket* p, const QString& protocol );
    bool checkDumpPacket( Opie::Net::OPacket* p );
    void registerSignalHandler();

  private:
    #ifdef QWS
    Opie::Core::OSystem _system;                // Opie Operating System identifier
    #endif

    Opie::Net::OWirelessNetworkInterface* iface;
    Opie::Net::OPacketCapturer* pcap;
    WellenreiterConfigWindow* configwindow;
    GPS* gps;

    //void readConfig();
    //void writeConfig();
};



#endif
