/**
 * @file osortweekdaybuttons.cpp
 * Implementation of a function which knows how to sort days of the week
 * buttons depending on whether the week starts on Monday or Sunday.
 *
 * @author Erik Hovland
 * Copyright (C) 2007, Erik Hovland <erik@hovland.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "osortweekdaybuttons.h"
#include <qbuttongroup.h>

void sortWeekdayButtons( const char& days, const bool& doesWeekStartOnMonday,
                         QButtonGroup* weekButtons )
{
    if ( !weekButtons )
        return;
    for ( int day = 0x01, buttons = 0; buttons < 7; day = day << 1, buttons++ )
    {
        if ( days & day ) {
            if ( doesWeekStartOnMonday )
                weekButtons->setButton( buttons );
            else {
                if ( buttons == 6 )
                    weekButtons->setButton( buttons );
                else
                    weekButtons->setButton( buttons + 1 );
            }
        }
    }
}
