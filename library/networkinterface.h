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
#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include <qnamespace.h>
#include <qstring.h>
#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
// {09794290-69d5-4c42-8a53-230cf2e53c50}
# ifndef IID_Network
#  define IID_Network QUuid( 0x09794290, 0x69d5, 0x4c42, 0x8a, 0x53, 0x23, 0x0c, 0xf2, 0xe5, 0x3c, 0x50)
# endif
#endif

class QWidget;
class Config;

struct NetworkInterface : public QUnknownInterface
{
    virtual bool doProperties( QWidget *parent, Config& ) = 0;
    virtual bool create( Config& ) = 0;
    virtual bool remove( Config& ) = 0;
    virtual QString device( Config& ) const;
    virtual QString cardType( Config& ) const;
    virtual bool isActive( Config& ) const;
    virtual bool isAvailable( Config& ) const;
    virtual bool start( Config& ) = 0;
    virtual bool start( Config&, const QString& password );
    virtual bool stop( Config& ) = 0;
    virtual bool needPassword( Config& ) const;
    virtual QWidget* addStateWidget( QWidget* parent, Config& ) const;
};

#endif

