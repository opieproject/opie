/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#include <qstring.h>

class QPixmap;
class QIconSet;

namespace Resource
{
    QPixmap loadPixmap( const QString& );
    QIconSet loadIconSet( const QString& );
}

#endif // RESOURCE_H
