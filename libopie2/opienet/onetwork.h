/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 by the Wellenreiter team:
                             Martin J. Muench <mjm@remote-exploit.org>
                             Max Moser <mmo@remote-exploit.org
                             Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef ONETWORK_H
#define ONETWORK_H

/* QT */

#include <qvaluelist.h>
#include <qdict.h>
#include <qmap.h>
#include <qobject.h>
#include <qhostaddress.h>

/* OPIE */

#include <opie2/onetutils.h>

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
#ifndef IW_MAX_PRIV_DEF
#define IW_MAX_PRIV_DEF        128
#endif

// ML: Yeah, I hate to include kernel headers, but it's necessary here
// ML: Here comes an ugly hack to prevent <linux/wireless.h> including <linux/if.h>
// ML: which conflicts with the user header <net/if.h>
// ML: We really a user header for the Wireless Extensions, something like <net/wireless.h>
// ML: I will drop Jean an mail on that subject

#include <net/if.h>
#define _LINUX_IF_H
#include <linux/wireless.h>

#ifndef SIOCIWFIRSTPRIV
#define SIOCIWFIRSTPRIV SIOCDEVPRIVATE
#endif

class ONetworkInterface;
class OWirelessNetworkInterface;
class OChannelHopper;
class OMonitoringInterface;

typedef struct ifreq ifreqstruct;
typedef struct iwreq iwreqstruct;
typedef struct iw_event iweventstruct;
typedef struct iw_freq iwfreqstruct;
typedef struct iw_priv_args iwprivargsstruct;
typedef struct iw_range iwrangestruct;

/*======================================================================================
 * ONetwork
 *======================================================================================*/

class ONetwork : public QObject
{
  Q_OBJECT

 public:
    typedef QDict<ONetworkInterface> InterfaceMap;
    typedef QDictIterator<ONetworkInterface> InterfaceIterator;

  public:
    static ONetwork* instance();
    InterfaceIterator iterator() const;
    bool isWirelessInterface( const char* ) const;
    ONetworkInterface* interface( QString ) const;

  protected:
    ONetwork();
    void synchronize();

  private:
    static ONetwork* _instance;
    InterfaceMap _interfaces;
};


/*======================================================================================
 * ONetworkInterface
 *======================================================================================*/

class ONetworkInterface : public QObject
{
    friend class OMonitoringInterface;
    friend class OCiscoMonitoringInterface;
    friend class OWlanNGMonitoringInterface;
    friend class OHostAPMonitoringInterface;
    friend class OOrinocoMonitoringInterface;

  public:
    ONetworkInterface( QObject* parent, const char* name );
    virtual ~ONetworkInterface();

    void setMonitoring( OMonitoringInterface* );
    OMonitoringInterface* monitoring() const;
    bool setPromiscuousMode( bool );
    bool promiscuousMode() const;
    bool setUp( bool );
    bool isUp() const;
    bool isLoopback() const;
    bool isWireless() const;
    QString ipV4Address() const;
    OMacAddress macAddress() const;

  protected:
    const int _sfd;
    mutable ifreqstruct _ifr;
    OMonitoringInterface* _mon;

  protected:
    ifreqstruct& ifr() const;
    virtual void init();
    bool ioctl( int call ) const;
    bool ioctl( int call, ifreqstruct& ) const;
};

/*======================================================================================
 * OChannelHopper
 *======================================================================================*/

class OChannelHopper : public QObject
{
  public:
    OChannelHopper( OWirelessNetworkInterface* );
    virtual ~OChannelHopper();
    bool isActive() const;
    int channel() const;
    virtual void timerEvent( QTimerEvent* );
    void setInterval( int );
    int interval() const;

  private:
    OWirelessNetworkInterface* _iface;
    int _interval;
    int _tid;
    QValueList<int> _channels;
    QValueList<int>::Iterator _channel;

};


/*======================================================================================
 * OWirelessNetworkInterface
 *======================================================================================*/

class OWirelessNetworkInterface : public ONetworkInterface
{
    friend class OMonitoringInterface;
    friend class OCiscoMonitoringInterface;
    friend class OWlanNGMonitoringInterface;
    friend class OHostAPMonitoringInterface;
    friend class OOrinocoMonitoringInterface;

    friend class OPrivateIOCTL;

  public:
    enum Mode { AdHoc, Managed, Monitor };

    OWirelessNetworkInterface( QObject* parent, const char* name );
    virtual ~OWirelessNetworkInterface();

    virtual void setChannel( int ) const;
    virtual int channel() const;
    virtual double frequency() const;
    virtual int channels() const;
    //virtual double frequency(int) const;

    virtual void setMode( Mode ) {};
    virtual bool mode() const {};

    virtual void setMonitorMode( bool );
    virtual bool monitorMode() const;

    virtual void setChannelHopping( int interval = 0 );
    virtual int channelHopping() const;

    virtual void setNickName( const QString& ) {};
    virtual QString nickName() const;

    virtual void setPrivate( const QString&, int, ... );
    virtual void getPrivate( const QString& );

    virtual bool isAssociated() const {};
    virtual QString associatedAP() const;

    virtual void setSSID( const QString& );
    virtual QString SSID() const;

  protected:
    void buildChannelList();
    void buildPrivateList();
    virtual void init();
    iwreqstruct& iwr() const;
    bool wioctl( int call ) const;
    bool wioctl( int call, iwreqstruct& ) const;

  protected:
    mutable iwreqstruct _iwr;
    QMap<int,int> _channels;

  private:
    OChannelHopper* _hopper;
};


/*======================================================================================
 * OMonitoringInterface
 *======================================================================================*/


class OMonitoringInterface
{
  public:
    OMonitoringInterface();
    OMonitoringInterface( ONetworkInterface* );
    virtual ~OMonitoringInterface();

  public:
    virtual void setEnabled( bool );
    virtual bool enabled() const;
    virtual void setChannel( int );

    virtual QString name() const = 0;

  protected:
    bool _enabled;
    const OWirelessNetworkInterface* _if;

};


/*======================================================================================
 * OCiscoMonitoring
 *======================================================================================*/


class OCiscoMonitoringInterface : public OMonitoringInterface
{
  public:
    OCiscoMonitoringInterface( ONetworkInterface* );
    virtual ~OCiscoMonitoringInterface();

    virtual void setEnabled( bool );
    virtual QString name() const;
    virtual void setChannel( int );

};

/*======================================================================================
 * OWlanNGMonitoringInterface
 *======================================================================================*/

class OWlanNGMonitoringInterface : public OMonitoringInterface
{
  public:
    OWlanNGMonitoringInterface( ONetworkInterface* );
    virtual ~OWlanNGMonitoringInterface();

  public:
    virtual void setEnabled( bool );
    virtual QString name() const;
    virtual void setChannel( int );

};

/*======================================================================================
 * OHostAPMonitoringInterface
 *======================================================================================*/

class OHostAPMonitoringInterface : public OMonitoringInterface
{
  public:
    OHostAPMonitoringInterface( ONetworkInterface* );
    virtual ~OHostAPMonitoringInterface();

  public:
    virtual void setEnabled( bool );
    virtual QString name() const;
 };

/*======================================================================================
 * OOrinocoMonitoringInterface
 *======================================================================================*/

class OOrinocoMonitoringInterface : public OMonitoringInterface
{
  public:
    OOrinocoMonitoringInterface( ONetworkInterface* );
    virtual ~OOrinocoMonitoringInterface();

   public:
    virtual void setChannel( int );
    virtual void setEnabled( bool );
    virtual QString name() const;

};

#endif // ONETWORK_H

