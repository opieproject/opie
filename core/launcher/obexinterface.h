/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef OBEXINTERFACE_H
#define OBEXINTERFACE_H

#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
// ### regenerate!!!!!!
// {6CA35D0B-C637-4865-A667-7D4CD8A70407}
# ifndef IID_ObexInterface
#  define IID_ObexInterface QUuid( 0x6ca35d0b, 0xc637, 0x4865, 0xa6, 0x67, 0x7d, 0x4c, 0xd8, 0xa7, 0x04, 0x07)
# endif
#endif

class QObject;

struct ObexInterface : public QUnknownInterface
{
};

#endif
