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
#ifndef TEXTCODECINTERFACE_H
#define TEXTCODECINTERFACE_H

#include <qstringlist.h>
#include <qpe/qcom.h>

class QTextCodec;

#ifndef QT_NO_COMPONENT
#ifndef IID_QtopiaTextCodec
#define IID_QtopiaTextCodec QUuid( 0x3ee02ba3, 0x57dc, 0x9b1e, 0x40, 0xf4, 0xda, 0xdf, 0x21, 0x89, 0xb6, 0xb4)
#endif
#endif

struct TextCodecInterface : public QUnknownInterface
{
public:
    virtual QStringList names() const = 0;
    virtual QTextCodec *createForName( const QString &name ) = 0;
    
    virtual QValueList<int> mibEnums() const = 0;
    virtual QTextCodec *createForMib( int mib ) = 0;
};

#endif
