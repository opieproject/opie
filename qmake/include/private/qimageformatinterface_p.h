/****************************************************************************
** $Id: qimageformatinterface_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of ???
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QIMAGEFORMATINTERFACE_H
#define QIMAGEFORMATINTERFACE_H

#ifndef QT_H
#include <private/qcom_p.h>
#endif // QT_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_NO_COMPONENT

// {04903F05-54B1-4726-A849-FB5CB097CA87} 
#ifndef IID_QImageFormat
#define IID_QImageFormat QUuid( 0x04903f05, 0x54b1, 0x4726, 0xa8, 0x49, 0xfb, 0x5c, 0xb0, 0x97, 0xca, 0x87 )
#endif

class QImage;

struct Q_EXPORT QImageFormatInterface : public QFeatureListInterface
{
    virtual QRESULT loadImage( const QString &format, const QString &filename, QImage * ) = 0;
    virtual QRESULT saveImage( const QString &format, const QString &filename, const QImage & ) = 0;

    virtual QRESULT installIOHandler( const QString & ) = 0;
};

#endif // QT_NO_COMPONENT

#endif // QIMAGEFORMATINTERFACE_H
