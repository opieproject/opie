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

class ONetworkInterface;
class OWirelessNetworkInterface;
class OChannelHopper;
class OMonitoringInterface;

/*======================================================================================
 * ONetwork
 *======================================================================================*/

/**
 * @brief A container class for all network devices.
 *
 * This class provides access to all available network devices of your computer.
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 */
class ONetwork : public QObject
{
  Q_OBJECT

 public:
    typedef QDict<ONetworkInterface> InterfaceMap;
    typedef QDictIterator<ONetworkInterface> InterfaceIterator;

  public:
    /**
     * @returns a pointer to the (one and only) @ref ONetwork instance.
     */
    static ONetwork* instance();
    /**
     * @returns an iterator usable for iterating through all network interfaces.
     */
    InterfaceIterator iterator() const;
    /**
     * @returns true, if the @p interface supports the wireless extension protocol.
     */
    bool isWirelessInterface( const char* interface ) const;
    /**
     * @returns a pointer to the @ref ONetworkInterface object for the specified @p interface or 0, if not found
     * @see ONetworkInterface
     */
    ONetworkInterface* interface( QString interface ) const;

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

/**
 * @brief A network interface wrapper.
 *
 * This class provides a wrapper for a network interface. All the cumbersume details of
 * Linux ioctls are hidden under a convenient high-level interface.
 * @warning Most of the setting methods contained in this class require the appropriate
 * process permissions to work.
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 */
class ONetworkInterface : public QObject
{
    friend class OMonitoringInterface;
    friend class OCiscoMonitoringInterface;
    friend class OWlanNGMonitoringInterface;
    friend class OHostAPMonitoringInterface;
    friend class OOrinocoMonitoringInterface;

  public:
    /**
     * Constructor. Normally you don't create @ref ONetworkInterface objects yourself,
     * but access them via @ref ONetwork::interface().
     */
    ONetworkInterface( QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~ONetworkInterface();
    /**
     * Associates a @a monitoring interface with this network interface.
     * @note This is currently only useful with @ref OWirelessNetworkInterface objects.
     */
    void setMonitoring( OMonitoringInterface* monitoring );
    /**
     * @returns the currently associated monitoring interface or 0, if no monitoring is associated.
     */
    OMonitoringInterface* monitoring() const;
    /**
     * Setting an interface to promiscuous mode enables the device to receive
     * all packets on the shared medium - as opposed to packets which are addressed to this interface.
     */
    bool setPromiscuousMode( bool );
    /**
     * @returns true if the interface is set to promiscuous mode.
     */
    bool promiscuousMode() const;
    /**
     * Setting an interface to up enables it to receive packets.
     */
    bool setUp( bool );
    /**
     * @returns true if the interface is up.
     */
    bool isUp() const;
    /*
     * @returns true if the interface is a loopback interface.
     */
    bool isLoopback() const;
    /*
     * @returns true if the interface is featuring supports the wireless extension protocol.
     */
    bool isWireless() const;
    /*
     * @returns the IPv4 address associated with this interface.
     */
    QString ipV4Address() const;
    /*
     * Associate the MAC address @a addr with the interface.
     * @note It can be necessary to shut down the interface prior to calling this method.
     * @warning This is not supported by all drivers.
     */
    void setMacAddress( const OMacAddress& addr );
    /*
     * @returns the MAC address associated with this interface.
     */
    OMacAddress macAddress() const;
    /*
     * @returns the data link type currently associated with this interface.
     * @see #include <net/if_arp.h> for possible values.
     */
    int dataLinkType() const;

  protected:
    const int _sfd;
    mutable ifreq _ifr;
    OMonitoringInterface* _mon;

  protected:
    struct ifreq& ifr() const;
    virtual void init();
    bool ioctl( int call ) const;
    bool ioctl( int call, struct ifreq& ) const;
};

/*======================================================================================
 * OChannelHopper
 *======================================================================================*/

/**
 * @brief A radio frequency channel hopper.
 *
 * This class provides a channel hopper for radio frequencies. A channel hopper frequently
 * changes the radio frequency channel of its associated @ref OWirelessNetworkInterface.
 * This is necessary when in monitoring mode and scanning for other devices, because
 * the radio frequency hardware can only detect packets sent on the same frequency.
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 */
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

/**
 * @brief A network interface wrapper for interfaces supporting the wireless extensions protocol.
 *
 * This class provides a high-level encapsulation of the Linux wireless extension API.
 */
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

    /**
     * Constructor.
     */
    OWirelessNetworkInterface( QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~OWirelessNetworkInterface();
    /**
     * Setting the @a channel of the interface changes the radio frequency (RF)
     * of the corresponding wireless network device.
     */
    virtual void setChannel( int channel ) const;
    /**
     * @returns the channel index of the current radio frequency.
     */
    virtual int channel() const;
    /**
     * @returns the current radio frequency (in MHz).
     */
    virtual double frequency() const;
    /**
     * @returns the number of radio frequency channels for the
     * corresponding wireless network device.
     * @note European devices usually have 14 channels, while American typically feature 11 channels.
     */
    virtual int channels() const;
    //virtual double frequency(int) const;

    virtual void setMode( Mode ) {}; //FIXME: Implement and document this
    virtual bool mode() const {}; //FIXME: Implement and document this

    /**
     * Setting the monitor mode on a wireless network interface enables
     * listening to IEEE 802.11 data and management frames which normally
     * are handled by the device firmware. This can be used to detect
     * other wireless network devices, e.g. Access Points or Ad-hoc stations.
     * @warning Standard wireless network drives don't support the monitor mode.
     * @warning You need a patched driver for this to work.
     * @note Enabling the monitor mode is highly driver dependent and requires
     * the proper @ref OMonitoringInterface to be associated with the interface.
     * @see OMonitoringInterface
     */
    virtual void setMonitorMode( bool );
    /**
     * @returns true if the device is listening in IEEE 802.11 monitor mode
     */
    virtual bool monitorMode() const;
    /**
     * Set the channel hopping @a interval. An @a interval of 0 disables channel hopping.
     * @see OChannelHopper
     */
    virtual void setChannelHopping( int interval = 0 );
    /**
     * @returns the channel hopping interval or 0, if channel hopping is disabled.
     */
    virtual int channelHopping() const;
    /**
     * Set the station @a nickname.
     */
    virtual void setNickName( const QString& nickname ) {}; //FIXME: Implement this
    /**
     * @returns the current station nickname.
     */
    virtual QString nickName() const;
    /**
     * Invoke the private IOCTL @a command with a @number of parameters on the network interface.
     * @see OPrivateIOCTL
     */
    virtual void setPrivate( const QString& command, int number, ... );
    /**
     * @returns true if the interface is featuring the private IOCTL @command.
     */
    virtual bool hasPrivate( const QString& command );
    virtual void getPrivate( const QString& command ); //FIXME: Implement and document this

    virtual bool isAssociated() const {}; //FIXME: Implement and document this
    virtual QString associatedAP() const; //FIXME: Implement and document this

    virtual void setSSID( const QString& );
    virtual QString SSID() const;

  protected:
    void buildChannelList();
    void buildPrivateList();
    virtual void init();
    struct iwreq& iwr() const;
    bool wioctl( int call ) const;
    bool wioctl( int call, struct iwreq& ) const;

  protected:
    mutable struct iwreq _iwr;
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
    OWirelessNetworkInterface* _if;

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

