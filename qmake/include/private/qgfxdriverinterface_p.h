/****************************************************************************
** $Id: qgfxdriverinterface_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of Qt/Embedded Graphics Driver Interface
**
** Created : 20020211
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

#ifndef QGFXDRIVERINTERFACE_H
#define QGFXDRIVERINTERFACE_H

#ifndef QT_H
#include <private/qcom_p.h>
#endif // QT_H

#ifndef QT_NO_COMPONENT

// {449EC6C6-DF3E-43E3-9E57-354A3D05AB34}
#ifndef IID_QGfxDriver
#define IID_QGfxDriver QUuid( 0x449ec6c6, 0xdf3e, 0x43e3, 0x9e, 0x57, 0x35, 0x4a, 0x3d, 0x05, 0xab, 0x34)
#endif

class QScreen;

struct Q_EXPORT QGfxDriverInterface : public QFeatureListInterface
{
    virtual QScreen* create( const QString& driver, int displayId ) = 0;
};

#endif // QT_NO_COMPONENT

#endif // QGFXDRIVERINTERFACE_H
