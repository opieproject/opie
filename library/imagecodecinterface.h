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
#ifndef IMAGECODECINTERFACE_H
#define IMAGECODECINTERFACE_H

#include <qstringlist.h>
#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
#ifndef IID_QtopiaImageCodec
#define IID_QtopiaImageCodec QUuid(0x09bf6906, 0x1549, 0xbb4a, 0x18, 0xba, 0xb9, 0xe7, 0x0a, 0x6e, 0x4d, 0x1e)
#endif
#endif

struct ImageCodecInterface : public QUnknownInterface
{
public:
    virtual QStringList keys() const = 0;
    virtual bool installIOHandler( const QString &format ) = 0;
};

#endif
