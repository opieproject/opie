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

#ifndef _XML_ENCODE_ATTR_
#define _XML_ENCODE_ATTR_

#include <qstring.h>

QString encodeAttr( const QString& str );

#endif

