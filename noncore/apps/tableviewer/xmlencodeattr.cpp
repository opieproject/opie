/*
 * xmlencodeattr.h
 *
 * copyright   : (c) 2003 by Joseph Wenninger
 *               except for a small modification it's identical to qdom.cpp:encodeAttr
 * email       : jowenn@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xmlencodeattr.h"
QString encodeAttr( const QString& str )
{
    QString tmp( str );
    uint len = tmp.length();
    uint i = 0;
    while ( i < len ) {
        if ( tmp[(int)i] == '<' ) {
            tmp.replace( i, 1, "&lt;" );
            len += 3;
            i += 4;
        } else if ( tmp[(int)i] == '"' ) {
            tmp.replace( i, 1, "&quot;" );
            len += 5;
            i += 6;
        } else if ( tmp[(int)i] == '&' ) {
            tmp.replace( i, 1, "&amp;" );
            len += 4;
            i += 5;
        } else if ( tmp[(int)i] == '>'  ) {
            tmp.replace( i, 1, "&gt;" );
            len += 3;
            i += 4;
        } else {
            ++i;
        }
    }

    return tmp;
}

