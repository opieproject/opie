/**************************************************************************************94x78**
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 *********************************************************************************************/

/*
 * $Id: vmemoimpl.cpp,v 1.2 2002-06-23 03:12:26 llornkcor Exp $
 */

#include "vmemo.h"
#include "vmemoimpl.h"


VMemoAppletImpl::VMemoAppletImpl()
        : vmemo(0), ref(0)
{
}

VMemoAppletImpl::~VMemoAppletImpl()
{
    delete vmemo;
}

QWidget *VMemoAppletImpl::applet( QWidget *parent )
{
    if ( !vmemo )
        vmemo = new VMemo( parent );
    return vmemo;
}

int VMemoAppletImpl::position() const
{
    return 6;
}

QRESULT VMemoAppletImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
        *iface = this;
    else if ( uuid == IID_TaskbarApplet )
        *iface = this;
  
    if ( *iface )
        (*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( VMemoAppletImpl )
        }
