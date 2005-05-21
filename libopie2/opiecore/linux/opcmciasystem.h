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
    /**
     * Constructor. Normally you don't create @ref OPcmciaSocket objects yourself,
     * but access them via @ref OPcmciaSystem::card().
     */
    OPcmciaSocket( int socket, QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~OPcmciaSocket();
    /**
     * @returns the corresponding socket number
     */
    int number() const;
    /**
     * @returns the identification string of the card in this socket, or "<Empty Socket>"
     */
    QString identity() const;
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

  protected:

  private:
    void init();
    bool command( const QString& cmd );
    int _socket;

  private:
    class Private;
    Private *d;
};


}
}

#endif // OPCMCIASYSTEM_H
