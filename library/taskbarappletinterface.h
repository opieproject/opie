/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#ifndef TASKBARAPPLETINTERFACE_H
#define TASKBARAPPLETINTERFACE_H

#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
// {6CA34D0B-C637-4865-A667-7D4CD8A70407}
# ifndef IID_TaskbarApplet
#  define IID_TaskbarApplet QUuid( 0x6ca34d0b, 0xc637, 0x4865, 0xa6, 0x67, 0x7d, 0x4c, 0xd8, 0xa7, 0x04, 0x07)
# endif
#endif

class QWidget;

struct TaskbarAppletInterface : public QUnknownInterface
{
    virtual QWidget *applet( QWidget *parent ) = 0;
    virtual int position() const = 0;
};

#endif
