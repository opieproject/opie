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

#include "wireless.h"

/* OPIE */

#include <opie2/onetutils.h>

/* QT */

#include <qvaluelist.h>
#include <qdict.h>
#include <qmap.h>
#include <qobject.h>
#include <qhostaddress.h>

class ONetworkInterface;
class OWirelessNetworkInterface;
class OChannelHopper;
class OMonitoringInterface;

/*======================================================================================
 * ONetwork
 *======================================================================================*/

/**
 * @brief A container class for all network interfaces
 *
 * This class provides access to all available network interfaces of your computer.
 *
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
     * @returns the number of available interfaces
     */
    int count() const;
    /**
     * @returns a pointer to the (one and only) @ref ONetwork instance.
     */
    static ONetwork* instance();
    /**
     * @returns an iterator usable for iterating through all network interfaces.
     */
    InterfaceIterator iterator() const;
    /**
     * @returns true, if the @a interface supports the wireless extension protocol.
     */
    bool isWirelessInterface( const char* interface ) const;
    /**
     * @returns a pointer to the @ref ONetworkInterface object for the specified @a interface or 0, if not found.
     * @see ONetworkInterface
     */
    ONetworkInterface* interface( const QString& interface ) const;
    /**
     * @internal Rebuild the internal interface database
     * @note Sometimes it might be useful to call this from client code,
     * e.g. after issuing a cardctl insert
     */
    void synchronize();
    /**
     * @returns the wireless extension version used at compile time.
     **/
    static short wirelessExtensionVersion();

  protected:
    ONetwork();

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
 *
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
    /**
     * @returns true if the interface is a loopback interface.
     */
    bool isLoopback() const;
    /**
     * @returns true if the interface is featuring supports the wireless extension protocol.
     */
    bool isWireless() const;
    /**
     * Associate the IP address @ addr with the interface.
     */
    void setIPV4Address( const QHostAddress& addr );
    /**
     * @returns the IPv4 address associated with the interface.
     */
    QString ipV4Address() const; //TODO: make this return an OHostAddress
    /**
     * Associate the MAC address @a addr with the interface.
     * @note It can be necessary to shut down the interface prior to calling this method.
     * @warning This is not supported by all drivers.
     */
    void setMacAddress( const OMacAddress& addr );
    /**
     * @returns the MAC address associated with the interface.
     */
    OMacAddress macAddress() const;
    /**
     * Associate the IPv4 @a netmask with the interface.
     */
    void setIPV4Netmask( const QHostAddress& netmask );
    /**
     * @returns the IPv4 netmask associated with the interface.
     */
    QString ipV4Netmask() const; //TODO: make this return an OHostAddress
    /**
     * @returns the data link type currently associated with the interface.
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
 *
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 */
class OChannelHopper : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor.
     */
    OChannelHopper( OWirelessNetworkInterface* );
    /**
     * Destructor.
     */
    virtual ~OChannelHopper();
    /**
     * @returns true, if the channel hopper is hopping channels
     */
    bool isActive() const;
    /**
     * @returns the last hopped channel
     */
    int channel() const;
    /**
     * Set the channel hopping @a interval.
     * An interval of 0 deactivates the channel hopper.
     */
    void setInterval( int interval );
    /**
     * @returns the channel hopping interval
     */
    int interval() const;

  signals:
    /**
     * This signal is emitted right after the channel hopper performed a hop
     */
    void hopped( int );

  protected:
    virtual void timerEvent( QTimerEvent* );

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
 *
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
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
     * @note Common channel range is within [1-14]. A value of 0 is not allowed.
     * @see channels()
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
    /**
     * Set the IEEE 802.11 operation @a mode.
     * Valid values are <ul><li>adhoc<li>managed<li>monitor<li>master
     * @warning Not all drivers support the all modes.
     * @note You might have to change the SSID to get the operation mode change into effect.
     */
    virtual void setMode( const QString& mode );
    /**
     * @returns the current IEEE 802.11 operation mode.
     * Possible values are <ul><li>adhoc<li>managed<li>monitor<li>master or <li>unknown
     */
    virtual QString mode() const;
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
    virtual void setMonitorMode( bool ); //FIXME: ==> setMode( "monitor" );
    /**
     * @returns true if the device is listening in IEEE 802.11 monitor mode
     */
    virtual bool monitorMode() const;  //FIXME: ==> mode()
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
     * @returns the @ref OChannelHopper of this interface or 0, if channel hopping has not been activated before
     */
    virtual OChannelHopper* channelHopper() const;
    /**
     * Set the station @a nickname.
     */
    virtual void setNickName( const QString& nickname );
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
    /**
     * @returns the MAC address of the Access Point if the
     * device is in infrastructure mode. @returns a (more or less random) CELL
     * address if the device is in adhoc mode.
     */
    virtual QString associatedAP() const;
    /**
     * Set the @a ssid (Service Set ID) string. This is used to decide
     * which network to associate with (use "any" to let the driver decide).
     */
    virtual void setSSID( const QString& ssid );
    /**
     * @returns the current SSID (Service Set ID).
     */
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
    OMonitoringInterface( ONetworkInterface*, bool _prismHeader );
    virtual ~OMonitoringInterface();

  public:
    virtual void setEnabled( bool );
    virtual bool enabled() const;
    virtual void setChannel( int );

    virtual QString name() const = 0;

  protected:
    OWirelessNetworkInterface* _if;
    bool _prismHeader;

};


/*======================================================================================
 * OCiscoMonitoring
 *======================================================================================*/


class OCiscoMonitoringInterface : public OMonitoringInterface
{
  public:
    OCiscoMonitoringInterface( ONetworkInterface*, bool _prismHeader );
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
    OWlanNGMonitoringInterface( ONetworkInterface*, bool _prismHeader );
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
    OHostAPMonitoringInterface( ONetworkInterface*, bool _prismHeader );
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
    OOrinocoMonitoringInterface( ONetworkInterface*, bool _prismHeader );
    virtual ~OOrinocoMonitoringInterface();

   public:
    virtual void setChannel( int );
    virtual void setEnabled( bool );
    virtual QString name() const;

};

#endif // ONETWORK_H

