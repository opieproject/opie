/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Team <opie@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

/* OPIE */
#include <opie2/osoundsystem.h>
#include <opie2/odebug.h>

using namespace Opie::MM;

int main( int argc, char** argv )
{
    odebug << "OPIE Sound System Demo" << oendl;

    OSoundSystem* sound = OSoundSystem::instance();

    OSoundSystem::CardIterator it = sound->iterator();
/*
    while ( it.current() )
    {
        odebug << "DEMO: OSoundSystem contains Interface '" <<  it.current()->name() << "'" << oendl;
        ++it;
    }

*/
    OSoundCard* card = it.current();

    OMixerInterface* mixer = card->mixer();

    QStringList channels = mixer->allChannels();

    for ( QStringList::Iterator it = channels.begin(); it != channels.end(); ++it )
    {
        odebug << "OSSDEMO: Mixer has channel " <<  *it << "" << oendl;
        odebug << "OSSDEMO:              +--- volume " << ( mixer->volume( *it ) & 0xff )
               << " (left) | " << ( mixer->volume( *it ) >> 8 ) << " (right)" << oendl;
    }

    return 0;

}
