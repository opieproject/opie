/****************************************************************************
** $Id: qstyleinterface_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** ...
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
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

#ifndef QSTYLEINTERFACE_H
#define QSTYLEINTERFACE_H

#ifndef QT_H
#include <private/qcom_p.h>
#endif // QT_H

#ifndef QT_NO_STYLE
#ifndef QT_NO_COMPONENT

class QStyle;

// {FC1B6EBE-053C-49c1-A483-C377739AB9A5}
#ifndef IID_QStyleFactory
#define IID_QStyleFactory QUuid(0xfc1b6ebe, 0x53c, 0x49c1, 0xa4, 0x83, 0xc3, 0x77, 0x73, 0x9a, 0xb9, 0xa5)
#endif

struct Q_EXPORT QStyleFactoryInterface : public QFeatureListInterface
{
    virtual QStyle* create( const QString& style ) = 0;
};

#endif //QT_NO_COMPONENT
#endif //QT_NO_STYLE

#endif //QSTYLEINTERFACE_H
