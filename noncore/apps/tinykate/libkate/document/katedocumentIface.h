/***************************************************************************
                          katedocumentIface.h  -  description
                             -------------------
    copyright            : (C) 2001 by The Kate Team
                           (C) 2002 by Joseph Wenninger
    email                : kwrite-devel@kde.org
                           jowenn@kde.org

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _KATEDOCUMENT_IFACE_H_
#define _KATEDOCUMENT_IFACE_H_

#include <dcopobject.h>

class KateDocumentDCOPIface : virtual public DCOPObject
{
  K_DCOP

  k_dcop:
    virtual void open (const QString &name=0)=0;
};
#endif

