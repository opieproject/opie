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
 * $Id: vmemoimpl.h,v 1.1 2002-02-14 01:39:21 llornkcor Exp $
 */

#ifndef __VMEMOIMPLAPPLETIMPL_H__
#define __VMEMOIMPLAPPLETIMPL_H__

#include <qpe/taskbarappletinterface.h>

class VMemo;

class VMemoAppletImpl : public TaskbarAppletInterface
{
public:
    VMemoAppletImpl();
    virtual ~VMemoAppletImpl();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

private:
    VMemo *vmemo;
    ulong ref;
};

#endif
