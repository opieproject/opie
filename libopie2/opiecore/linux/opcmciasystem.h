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
#include <qdict.h>
#include <qmap.h>

namespace Opie {
namespace Core {

class OPcmciaCard;

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
    typedef QDict<OPcmciaCard> CardMap;
    typedef QDictIterator<OPcmciaCard> CardIterator;

  public:
    /**
     * @returns the number of available interfaces
     */
    int count() const;
    /**
     * @returns a pointer to the (one and only) @ref OSystem instance.
     */
    static OPcmciaSystem* instance();
    /**
     * @returns an iterator usable for iterating through all sound cards.
     */
    CardIterator iterator() const;
    /**
     * @returns a pointer to the @ref OAudioInterface object for the specified @a interface or 0, if not found
     * @see OAudioInterface
     */
    OPcmciaCard* card( const QString& interface ) const;
    /**
     * @internal Rebuild the internal interface database
     * @note Sometimes it might be useful to call this from client code,
     * e.g. after issuing a cardctl insert
     */
    void synchronize();

  protected:
    OPcmciaSystem();

  private:
    static OPcmciaSystem* _instance;
    CardMap _interfaces;
    class Private;
    Private *d;
};


/*======================================================================================
 * OPcmciaCard
 *======================================================================================*/

class OPcmciaCard : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor. Normally you don't create @ref OPcmciaCard objects yourself,
     * but access them via @ref OPcmciaSystem::card().
     */
    OPcmciaCard( QObject* parent, const char* name );
    /**
     * Destructor.
     */
    virtual ~OPcmciaCard();

  protected:

  private:
    void init();
  private:
    class Private;
    Private *d;
};


}
}

#endif // OPCMCIASYSTEM_H
