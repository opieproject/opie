/*
                             This file is part of the Opie Project
              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#ifndef OINPUTSYSTEM_H
#define OINPUTSYSTEM_H

#include "linux_input.h"

/* QT */
#include <qobject.h>
#include <qdict.h>

namespace Opie {
namespace Core {

class OInputDevice;

/**
 * @brief A container class for all input devices
 *
 * This class provides access to all available input system devices of your computer.
 *
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 */
class OInputSystem : public QObject
{
  public:
    typedef QDict<OInputDevice> DeviceMap;
    typedef QDictIterator<OInputDevice> DeviceIterator;

     /**
     * @returns the number of available input devices
     */
    int count() const;
    /**
     * @returns a pointer to the (one and only) @ref OInputSystem instance.
     */
    static OInputSystem* instance();
    /**
     * @returns an iterator usable for iterating through all network interfaces.
     */
    DeviceIterator iterator() const;
    /**
     * @returns a pointer to the @ref OAudioInterface object for the specified @a interface or 0, if not found
     * @see OAudioInterface
     */
    OInputDevice* device( const QString& interface ) const;
    /**
     * @internal Rebuild the internal interface database
     * @note Sometimes it might be useful to call this from client code,
     */
    void synchronize();
    /**
     * @internal desctructor
     */    
     ~OInputSystem();
 
 protected:
    OInputSystem();
    
    static OInputSystem* _instance;
    DeviceMap _devices;
};


class OInputDevice : public QObject
{  
  public:
    
    enum Feature
    {
        Synchronous     = EV_SYN,
        Keys            = EV_KEY,
        Relative        = EV_REL,
        Absolute        = EV_ABS,
        Miscellaneous   = EV_MSC,
        Leds            = EV_LED,
        Sound           = EV_SND,
        AutoRepeat      = EV_REP,
        ForceFeedback   = EV_FF,
        PowerManagement = EV_PWR,
        ForceFeedbackStatus = EV_FF_STATUS,
    };
    
    enum Bus
    {
        PCI             = BUS_PCI,
        ISAPNP          = BUS_ISAPNP,
        HIL             = BUS_HIL,
        BLUETOOTH       = BUS_BLUETOOTH,
        ISA             = BUS_ISA,
        I8042           = BUS_I8042,
        XTKBD           = BUS_XTKBD,
        RS232           = BUS_RS232,
        GAMEPORT        = BUS_GAMEPORT,
        PARPORT         = BUS_PARPORT,
        AMIGA           = BUS_AMIGA,
        ADB             = BUS_ADB,
        I2C             = BUS_I2C,
        HOST            = BUS_HOST,
    };
  
  public:
    OInputDevice( QObject* parent, const char* name = 0 );
    ~OInputDevice();
    
  public:
    QString identity() const;
    QString path() const;
    QString uniq() const;
    bool    hasFeature( Feature ) const;
    
  private:
    int _fd;
    input_id _id;

};

}
}

#endif // OINPUTSYSTEM_H

