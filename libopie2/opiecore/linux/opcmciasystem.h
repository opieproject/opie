/*
                             This file is part of the Opie Project
              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
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

#ifndef OPCMCIASYSTEM_H
#define OPCMCIASYSTEM_H

#include "linux_pcmcia.h"

#include <qobject.h>
#include <qlist.h>

namespace Opie {
namespace Core {

class OPcmciaSocket;

/*======================================================================================
 * OPcmciaSystem
 *======================================================================================*/

/**
 * @brief A container class for the linux pcmcia subsystem
 *
 * This class provides access to all available pcmcia/cf cards on your device.
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */
class OPcmciaSystem : public QObject
{
  Q_OBJECT

  public:
    typedef QList<OPcmciaSocket> CardList;
    typedef QListIterator<OPcmciaSocket> CardIterator;

  public:
    /**
     * @returns the number of available sockets
     */
    int count() const;
    /**
     * @returns the number of populated sockets
     */
    int cardCount() const;
    /**
     * @returns a pointer to the (one and only) @ref OSystem instance.
     */
    static OPcmciaSystem* instance();
    /**
     * @returns an iterator usable for iterating through all sound cards.
     */
    CardIterator iterator() const;
    /**
     * @returns a pointer to the @ref OPcmciaSocket object correspinding to socket number n, or 0, if not found
     * @see OPcmciaSocket
     */
    OPcmciaSocket* socket( unsigned int number );
    /**
     * @internal Rebuild the internal database
     * @note Sometimes it might be useful to call this from client code,
     * e.g. after issuing a cardctl insert
     */
    void synchronize();

  protected:
    OPcmciaSystem();

  private:
    static OPcmciaSystem* _instance;
    CardList _interfaces;
    int _major;

  private:
    class Private;
    Private *d;
};


/*======================================================================================
 * OPcmciaSocket
 *======================================================================================*/

class OPcmciaSocket : public QObject
{
  Q_OBJECT
  public:

    enum OPcmciaSocketCardStatus
    {
        Unknown             = 0,
        Occupied            = CS_EVENT_CARD_DETECT,
        OccupiedCardBus     = CS_EVENT_CB_DETECT,
        WriteProtected      = CS_EVENT_WRITE_PROTECT,
        BatteryLow          = CS_EVENT_BATTERY_LOW,
        BatteryDead         = CS_EVENT_BATTERY_DEAD,
        Ready               = CS_EVENT_READY_CHANGE,
        Suspended           = CS_EVENT_PM_SUSPEND,
        Attention           = CS_EVENT_REQUEST_ATTENTION,
        InsertionInProgress = CS_EVENT_CARD_INSERTION,
        RemovalInProgress   = CS_EVENT_CARD_REMOVAL,
        ThreeVolts          = CS_EVENT_3VCARD,
        SupportsVoltage     = CS_EVENT_XVCARD,
    };

  public:
    /**
     * Constructor. Normally you don't create @ref OPcmciaSocket objects yourself,
     * but access them via @ref OPcmciaSystem::socket().
     */
    OPcmciaSocket( int major, int socket, QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~OPcmciaSocket();
    /**
     * @returns the corresponding socket number
     */
    int number() const;
    /**
     * @returns the card managers idea of the cards' identy, or "<Empty Socket>"
     */
    QString identity() const;
    /**
      * @returns the socket status
      */
    const OPcmciaSocketCardStatus status() const;
    /**
     * @returns true, if the card is unsupported by the cardmgr
     */
    bool isUnsupported() const;
    /**
     * @returns true, if the socket is empty
     */
    bool isEmpty() const;
    /**
     * @returns true, if the socket is suspended
     */
    bool isSuspended() const;
    /**
     * Eject card. @returns true, if operation succeeded
     * @note: This operation needs root privileges
     */
    bool eject();
    /**
     * Insert card. @returns true, if operation succeeded
     * @note: This operation needs root privileges
     */
    bool insert();
    /**
     * Suspend card. @returns true, if operation succeeded
     * @note: This operation needs root privileges
     */
    bool suspend();
    /**
     * Resume card. @returns true, if operation succeeded
     * @note: This operation needs root privileges
     */
    bool resume();
    /**
     * Reset card. @returns true, if operation succeeded
     * @note: This operation needs root privileges
     */
    bool reset();
    /**
     * @returns the product ID vector
     */
    QStringList productIdentityVector() const;
    /**
      * @returns the product ID string
      */
    QString productIdentity() const;
    /**
      * @returns the manufacturer ID string
      */
    QString manufacturerIdentity() const;
    /**
      * @returns the function string
      */
    QString function() const;

  private:
    void init();
    void cleanup();
    bool getTuple( cisdata_t tuple ) const;
    int _major;
    int _socket;
    int _fd;
    mutable ds_ioctl_arg_t _ioctlarg;

  private:
    class Private;
    Private *d;
};


}
}

#endif // OPCMCIASYSTEM_H
