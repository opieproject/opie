/****************************************************************************
** $Id: qkbddriverinterface_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of Qt/Embedded Keyboard Driver Interface
**
** Created : 20020218
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QKBDDRIVERINTERFACE_H
#define QKBDDRIVERINTERFACE_H

#ifndef QT_H
#include <private/qcom_p.h>
#endif // QT_H

#ifndef QT_NO_COMPONENT

// {C7C838EA-FC3E-4905-92AD-F479E81F1D02}
#ifndef IID_QKbdDriver
#define IID_QKbdDriver QUuid( 0xc7c838ea, 0xfc3e, 0x4905, 0x92, 0xad, 0xf4, 0x79, 0xe8, 0x1f, 0x1d, 0x02)
#endif

class QWSKeyboardHandler;

struct Q_EXPORT QKbdDriverInterface : public QFeatureListInterface
{
    virtual QWSKeyboardHandler* create( const QString& driver, const QString& device ) = 0;
};

#endif // QT_NO_COMPONENT

#endif // QKBDDRIVERINTERFACE_H
