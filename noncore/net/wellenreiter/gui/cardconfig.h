/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef CARDCONFIG_H
#define CARDCONFIG_H

#include <qstring.h>

#ifdef QWS
#include <opie/odevice.h>
using namespace Opie;
#endif

class CardConfig
{
  public:
  
    typedef enum { Prism, Orinoco, HostAP, Manual } Type;
  
  public:
      
    CardConfig( const QString& interface, Type type = Manual, int hopinterval = 100 );
    virtual ~CardConfig();
    
    const QString& interface() { return _interface; };
    int hopinterval() { return _hopinterval; };
    Type type() { return _type; };
    
    #ifdef QWS
    OSystem system() { return _system; };
    #endif
    
  private:
    
    QString _interface;
    Type _type;
    int _hopinterval;
    
    #ifdef QWS
    OSystem _system;
    #endif
    
};

#endif

