/****************************************************************************
** $Id: qcom_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** ...
**
** Copyright (C) 2001-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QCOM_H
#define QCOM_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "qstringlist.h"
#include "quuid.h"
#endif // QT_H

#ifndef QT_NO_COMPONENT

class QObject;
struct QUInterfaceDescription;
struct QUObject;

#define QRESULT		unsigned long
#define QS_OK		(QRESULT)0x00000000
#define QS_FALSE	(QRESULT)0x00000001

#define QE_NOTIMPL      (QRESULT)0x80000001
#define QE_OUTOFMEMORY  (QRESULT)0x80000002
#define QE_INVALIDARG	(QRESULT)0x80000003
#define QE_NOINTERFACE	(QRESULT)0x80000004
#define QE_NOCOMPONENT	(QRESULT)0x80000005


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

// {FBAC965E-A441-413F-935E-CDF582573FAB}
#ifndef IID_QDispatch
#define IID_QDispatch QUuid( 0xfbac965e, 0xa441, 0x413f, 0x93, 0x5e, 0xcd, 0xf5, 0x82, 0x57, 0x3f, 0xab)
#endif

// the dispatch interface that inherits the unknown interface.. It is
// used to explore interfaces during runtime and to do dynamic calls.
struct Q_EXPORT QDispatchInterface : public QUnknownInterface
{
    // returns the interface description of this dispatch interface.
    virtual const QUInterfaceDescription* interfaceDescription() const = 0;

    // returns the event description of this dispatch interface.
    virtual const QUInterfaceDescription* eventsDescription() const = 0;

    // invokes method id with parameters V*. Returns some sort of
    // exception code.
    virtual QRESULT invoke( int id, QUObject* o ) = 0;

    // installs listener as event listener
    virtual void installListener( QDispatchInterface* listener ) = 0;

    // remove listener as event listener
    virtual void removeListener( QDispatchInterface* listener ) = 0;
};

template <class T>
class QInterfacePtr
{
public:
    QInterfacePtr():iface(0){}

    QInterfacePtr( T* i) {
	if ( (iface = i) )
	    iface->addRef();
    }

    QInterfacePtr(const QInterfacePtr<T> &p) {
	if ( (iface = p.iface) )
	    iface->addRef();
    }

    ~QInterfacePtr() {
	if ( iface )
	    iface->release();
    }

    QInterfacePtr<T> &operator=(const QInterfacePtr<T> &p) {
	if ( iface != p.iface ) {
	    if ( iface )
		iface->release();
	    if ( (iface = p.iface) )
		iface->addRef();
	}
	return *this;
    }

    QInterfacePtr<T> &operator=(T* i) {
	if (iface != i ) {
	    if ( iface )
		iface->release();
	    if ( (iface = i) )
		iface->addRef();
	}
	return *this;
    }

    bool operator==( const QInterfacePtr<T> &p ) const { return iface == p.iface; }

    bool operator!= ( const QInterfacePtr<T>& p ) const {  return !( *this == p ); }

    bool isNull() const { return !iface; }

    T* operator->() const { return iface; }

    T& operator*() const { return *iface; }

    operator T*() const { return iface; }

    QUnknownInterface** operator &() const {
	if( iface )
	    iface->release();
	return (QUnknownInterface**)&iface;
    }

    T** operator &() {
	if ( iface )
	    iface->release();
	return &iface;
    }

private:
    T* iface;
};

// {10A1501B-4C5F-4914-95DD-C400486CF900}
#ifndef IID_QObject
#define IID_QObject QUuid( 0x10a1501b, 0x4c5f, 0x4914, 0x95, 0xdd, 0xc4, 0x00, 0x48, 0x6c, 0xf9, 0x00)
#endif

struct Q_EXPORT QObjectInterface
{
    virtual QObject*   qObject() = 0;
};

// {5F3968A5-F451-45b1-96FB-061AD98F926E}
#ifndef IID_QComponentInformation
#define IID_QComponentInformation QUuid(0x5f3968a5, 0xf451, 0x45b1, 0x96, 0xfb, 0x6, 0x1a, 0xd9, 0x8f, 0x92, 0x6e)
#endif

struct Q_EXPORT QComponentInformationInterface : public QUnknownInterface
{
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString author() const = 0;
    virtual QString version() const = 0;
};

// {6CAA771B-17BB-4988-9E78-BA5CDDAAC31E}
#ifndef IID_QComponentFactory
#define IID_QComponentFactory QUuid( 0x6caa771b, 0x17bb, 0x4988, 0x9e, 0x78, 0xba, 0x5c, 0xdd, 0xaa, 0xc3, 0x1e)
#endif

struct Q_EXPORT QComponentFactoryInterface : public QUnknownInterface
{
    virtual QRESULT createInstance( const QUuid &cid, const QUuid &iid, QUnknownInterface** instance, QUnknownInterface *outer ) = 0;
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

// {3F8FDC44-3015-4f3e-B6D6-E4AAAABDEAAD}
#ifndef IID_QFeatureList
#define IID_QFeatureList QUuid(0x3f8fdc44, 0x3015, 0x4f3e, 0xb6, 0xd6, 0xe4, 0xaa, 0xaa, 0xbd, 0xea, 0xad)
#endif

struct Q_EXPORT QFeatureListInterface : public QUnknownInterface
{
    virtual QStringList	featureList() const = 0;
};

// {B5FEB5DE-E0CD-4E37-B0EB-8A812499A0C1}
#ifndef IID_QComponentRegistration
#define IID_QComponentRegistration QUuid( 0xb5feb5de, 0xe0cd, 0x4e37, 0xb0, 0xeb, 0x8a, 0x81, 0x24, 0x99, 0xa0, 0xc1)
#endif

struct Q_EXPORT QComponentRegistrationInterface : public QUnknownInterface
{
    virtual bool    registerComponents( const QString &filepath ) const = 0;
    virtual bool    unregisterComponents() const = 0;
};

// internal class that wraps an initialized ulong
struct Q_EXPORT QtULong
{
    QtULong() : ref( 0 ) { }
    operator unsigned long () const { return ref; }
    unsigned long& operator++() { return ++ref; }
    unsigned long operator++( int ) { return ref++; }
    unsigned long& operator--() { return --ref; }
    unsigned long operator--( int ) { return ref--; }

    unsigned long ref;
};
// default implementation of ref counting. A variable "ulong ref" has to be a member


#define Q_REFCOUNT \
private:	   \
    QtULong qtrefcount;   \
public:		   \
    ulong addRef() {return qtrefcount++;} \
    ulong release() {if(!--qtrefcount){delete this;return 0;}return qtrefcount;}

#ifndef Q_EXPORT_COMPONENT
#if defined(QT_THREAD_SUPPORT)
#define QT_THREADED_BUILD 1
#define Q_UCM_FLAGS_STRING "11"
#else
#define QT_THREADED_BUILD 0
#define Q_UCM_FLAGS_STRING "01"
#endif

#ifndef Q_EXTERN_C
#ifdef __cplusplus
#define Q_EXTERN_C    extern "C"
#else
#define Q_EXTERN_C    extern
#endif
#endif

// this is duplicated at Q_PLUGIN_VERIFICATION_DATA in qgplugin.h
// NOTE: if you change pattern, you MUST change the pattern in
// qcomlibrary.cpp as well.  changing the pattern will break all
// backwards compatibility as well (no old plugins will be loaded).
#ifndef Q_UCM_VERIFICATION_DATA
#  define Q_UCM_VERIFICATION_DATA \
	static const char *qt_ucm_verification_data =			\
            "pattern=""QT_UCM_VERIFICATION_DATA""\n"			\
            "version="QT_VERSION_STR"\n"				\
            "flags="Q_UCM_FLAGS_STRING"\n"				\
	    "buildkey="QT_BUILD_KEY"\0";
#endif // Q_UCM_VERIFICATION_DATA

// This macro expands to the default implementation of ucm_instantiate.
#ifndef Q_CREATE_INSTANCE
#    define Q_CREATE_INSTANCE( IMPLEMENTATION )		\
	IMPLEMENTATION *i = new IMPLEMENTATION;		\
	QUnknownInterface* iface = 0; 			\
	i->queryInterface( IID_QUnknown, &iface );	\
	return iface;
#endif // Q_CREATE_INSTANCE

#    ifdef Q_WS_WIN
#	ifdef Q_CC_BOR
#	    define Q_EXPORT_COMPONENT() \
	        Q_UCM_VERIFICATION_DATA \
		Q_EXTERN_C __declspec(dllexport) \
                const char * __stdcall qt_ucm_query_verification_data() \
                { return qt_ucm_verification_data; } \
		Q_EXTERN_C __declspec(dllexport) QUnknownInterface* \
                __stdcall ucm_instantiate()
#	else
#	    define Q_EXPORT_COMPONENT() \
	        Q_UCM_VERIFICATION_DATA \
		Q_EXTERN_C __declspec(dllexport) \
                const char *qt_ucm_query_verification_data() \
                { return qt_ucm_verification_data; } \
		Q_EXTERN_C __declspec(dllexport) QUnknownInterface* ucm_instantiate()
#	endif
#    else
#	define Q_EXPORT_COMPONENT() \
	    Q_UCM_VERIFICATION_DATA \
	    Q_EXTERN_C \
            const char *qt_ucm_query_verification_data() \
            { return qt_ucm_verification_data; } \
	    Q_EXTERN_C QUnknownInterface* ucm_instantiate()
#    endif
#    define Q_EXPORT_INTERFACE() Q_EXPORT_COMPONENT()
#endif

#endif //QT_NO_COMPONENT

#endif //QCOM_H
