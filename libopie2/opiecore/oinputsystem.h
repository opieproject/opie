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
    OInputDevice( QObject* parent, const char* name = 0 );
    ~OInputDevice();

    #include "oinputsystemenums.h"    
    
  public:
    QString identity() const;
    QString path() const;
    QString uniq() const;
    bool    hasFeature( Feature ) const;
    bool    isHeld( Key ) const;
    
  private:
    int _fd;
    input_id _id;

};

}
}

#endif // OINPUTSYSTEM_H

