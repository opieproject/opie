/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#ifndef BINARYIMPL_H
#define BINARYIMPL_H

#include "binaryimpl.h"
#include <plugininterface.h>
#include <engine.h>

class BinaryInterface : public CalcInterface
{
public:
    BinaryInterface(){input = 0;};
    virtual ~BinaryInterface(){};

#ifndef QT_NO_COMPONENT
    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT
#endif

    QWidget *getPlugin( Engine *, QWidget *parent );

private:
    FormBinaryImpl *input;
    ulong ref;
};

#endif
