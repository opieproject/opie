/****************************************************************************
** $Id: qcomponentfactory_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of the QComponentFactory class
**
** Created : 990101
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
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

#ifndef QCOMPONENTFACTORY_H
#define QCOMPONENTFACTORY_H

#ifndef QT_H
#include "qcom_p.h"
#endif // QT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_NO_COMPONENT

class Q_EXPORT QComponentFactory
{
public:
    static QRESULT createInstance( const QString &cid, const QUuid &iid, QUnknownInterface** instance, QUnknownInterface *outer = 0 );
    static QRESULT registerServer( const QString &filename );
    static QRESULT unregisterServer( const QString &filename );

    static bool registerComponent( const QUuid &cid, const QString &filename, const QString &name = QString::null, 
				   int version = 0, const QString &description = QString::null );
    static bool unregisterComponent( const QUuid &cid );
};

#endif // QT_NO_COMPONENT

#endif // QCOMPONENTFACTORY_H
