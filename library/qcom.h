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

#ifndef QCOM_H
#define QCOM_H
 
#include <qstringlist.h>

#ifndef QT_NO_COMPONENT

#include <qpe/quuid.h>

#define QRESULT         unsigned long
#define QS_OK           (QRESULT)0x00000000
#define QS_FALSE        (QRESULT)0x00000001

#define QE_NOTIMPL      (QRESULT)0x80000001
#define QE_OUTOFMEMORY  (QRESULT)0x80000002
#define QE_INVALIDARG   (QRESULT)0x80000003
#define QE_NOINTERFACE  (QRESULT)0x80000004
#define QE_NOCOMPONENT  (QRESULT)0x80000005

// {1D8518CD-E8F5-4366-99E8-879FD7E482DE}
#ifndef IID_QUnknown
#define IID_QUnknown QUuid(0x1d8518cd, 0xe8f5, 0x4366, 0x99, 0xe8, 0x87, 0x9f, 0xd7, 0xe4, 0x82, 0xde)
#endif

struct Q_EXPORT QUnknownInterface
{
    virtual QRESULT queryInterface( const QUuid&, QUnknownInterface** ) = 0;
    virtual ulong   addRef() = 0;
    virtual ulong   release() = 0;
};

// {D16111D4-E1E7-4C47-8599-24483DAE2E07}
#ifndef IID_QLibrary
#define IID_QLibrary QUuid( 0xd16111d4, 0xe1e7, 0x4c47, 0x85, 0x99, 0x24, 0x48, 0x3d, 0xae, 0x2e, 0x07)
#endif
 
struct Q_EXPORT QLibraryInterface : public QUnknownInterface
{
    virtual bool    init() = 0;
    virtual void    cleanup() = 0;
    virtual bool    canUnload() const = 0;
};

#define Q_CREATE_INSTANCE( IMPLEMENTATION )         \
	IMPLEMENTATION *i = new IMPLEMENTATION; \
	QUnknownInterface* iface = 0;                   \
	i->queryInterface( IID_QUnknown, &iface );      \
	return iface;

#define Q_EXPORT_INTERFACE() \
	extern "C" QUnknownInterface* ucm_instantiate()

#define Q_REFCOUNT  ulong addRef() {return ref++;}ulong release() {if(!--ref){delete this;return 0;}return ref;}

#else // QT_NO_COMPONENT

struct Q_EXPORT QUnknownInterface
{
};

#endif // QT_NO_COMPONENT

#endif // QCOM_H
