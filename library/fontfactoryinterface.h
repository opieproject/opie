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
#ifndef FONTFACTORYINTERFACE_H
#define FONTFACTORYINTERFACE_H

#include <qnamespace.h>
#include <qstring.h>
#include <qpe/qcom.h>

class QFontFactory;

#ifndef QT_NO_COMPONENT
// {7F194DD6-FAA3-498F-8F30-9C297A570DFA}
#ifndef IID_FontFactory
#define IID_FontFactory QUuid( 0x7f194dd6, 0xfaa3, 0x498f, 0x8f, 0x30, 0x9c, 0x29, 0x7a, 0x57, 0x0d, 0xfa)
#endif
#endif

struct FontFactoryInterface : public QUnknownInterface
{
    virtual QFontFactory *fontFactory() = 0;
};

#endif
