/****************************************************************************
** $Id: qmousedriverinterface_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of Qt/Embedded Mouse Driver Interface
**
** Created : 20020220
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

#ifndef QMOUSEDRIVERINTERFACE_H
#define QMOUSEDRIVERINTERFACE_H

#ifndef QT_H
#include <private/qcom_p.h>
#endif // QT_H

#ifndef QT_NO_COMPONENT

// {4367CF5A-F7CE-407B-8BB6-DF19AEDA2EBB}
#ifndef IID_QMouseDriver
#define IID_QMouseDriver QUuid( 0x4367cf5a, 0xf7ce, 0x407b, 0x8b, 0xb6, 0xdf, 0x19, 0xae, 0xda, 0x2e, 0xbb)
#endif

class QWSMouseHandler;

struct Q_EXPORT QMouseDriverInterface : public QFeatureListInterface
{
    virtual QWSMouseHandler* create( const QString& driver, const QString &device ) = 0;
};

#endif // QT_NO_COMPONENT

#endif // QMOUSEDRIVERINTERFACE_H
