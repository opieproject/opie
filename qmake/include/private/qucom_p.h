/****************************************************************************
** $Id: qucom_p.h,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Definition of the QUcom interfaces
**
** Created : 990101
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
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

#ifndef QUCOM_H
#define QUCOM_H

#ifndef QT_H
#include <qstring.h>
#include "quuid.h"
#endif // QT_H

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

#ifdef check
#undef check
#endif

struct QUObject;
struct QUInterfaceDescription;
struct QUnknownInterface;
struct QDispatchInterface;


struct Q_EXPORT QUBuffer
{
    virtual long read( char *data, ulong maxlen ) = 0;
    virtual long write( const char *data, ulong len ) = 0;
};


// A type for a QUObject
struct Q_EXPORT QUType
{
    virtual const QUuid *uuid() const = 0;
    virtual const char *desc() const = 0;


    virtual bool canConvertFrom( QUObject *, QUType * ) = 0;
    // virtual private, only called by canConvertFrom
    virtual bool canConvertTo( QUObject *, QUType * ) = 0;


    virtual bool convertFrom( QUObject *, QUType * ) = 0;
    // virtual private, only called by convertFrom
    virtual bool convertTo( QUObject *, QUType * ) = 0;

    virtual void clear( QUObject * ) = 0;

    virtual int serializeTo( QUObject *, QUBuffer * ) = 0;
    virtual int serializeFrom( QUObject *, QUBuffer * ) = 0;

    static bool isEqual( const QUType *t1, const QUType *t2 );
    static bool check( QUObject* o, QUType* t );
};


// {DE56510E-4E9F-4b76-A3C2-D1E2EF42F1AC}
extern Q_EXPORT const QUuid TID_QUType_Null;
struct Q_EXPORT QUType_Null : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * );
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_Null static_QUType_Null;


// The magic QUObject
struct Q_EXPORT QUObject
{
public: // scary MSVC bug makes this necessary
    QUObject() : type( &static_QUType_Null ) {}
    ~QUObject() { type->clear( this ); }

    QUType *type;

    // the unavoidable union
    union
    {
	bool b;
	
	char c;
	short s;
	int i;
	long l;
	
	unsigned char uc;
	unsigned short us;
	unsigned int ui;
	unsigned long ul;
	
	float f;
	double d;
	
	char byte[16];
	
	struct {
	   char* data;
	   unsigned long size;
	} bytearray;
	
	void* ptr;
	
	struct {
	    void *ptr;
	    bool owner;
	} voidstar;
	
	struct {
	    char *ptr;
	    bool owner;
	} charstar;

	struct {
	    char *ptr;
	    bool owner;
	} utf8;
	
	struct {
	    char *ptr;
	    bool owner;
	} local8bit;
	
	QUnknownInterface* iface;
	QDispatchInterface* idisp;
	
    } payload;

};


// A parameter description describes one method parameters. A
// parameter has a name, a type and a flag describing whether it's an
// in parameter, an out parameter, or both ways
struct Q_EXPORT QUParameter
{
    const char* name;
    QUType *type;
    const void* typeExtra; //Usually 0, UEnum* for QUType_enum, const char* for QUType_ptr, int* for QUType_varptr
    enum { In = 1, Out = 2, InOut = In | Out };
    int inOut;
};

// A method description describes one method. A method has a name and
// an array of parameters.
struct Q_EXPORT QUMethod
{
    const char* name;
    int count;
    const QUParameter* parameters;
};

// A Property description. Not used yet in the example.
struct Q_EXPORT QUProperty
{
    const char* name;
    QUType* type;
    const void* typeExtra; //type dependend. Usually 0, but UEnum for QUTypeenum or const char* for QUTypeptr

    int set; // -1 undefined
    int get; // -1 undefined

    int designable; // -1 FALSE, -2 TRUE, else method
    int stored; // -1 FALSE, -2 TRUE, else method
};

// An interface description describes one interface, that is all its
// methods and properties.
struct Q_EXPORT QUInterfaceDescription
{
    int methodCount;
    const QUMethod* methods;
    int propertyCount;
    const QUProperty* properties;
};


// A component description describe one component, that is its name,
// vendor, release, info, its component uuid and all its interface
// uuids.
struct Q_EXPORT QUComponentDescription
{
    const char* name;
    const char* vendor;
    const char* release;
    const char* info;
    QUuid cid;
    int count;
    const QUuid* interfaces;
};


// A component server description describe one component server, that
// is its name, vendor, release, info and the descriptions of all
// components it can instantiate.
struct Q_EXPORT QUComponentServerDescription
{
    const char* name;
    const char* vendor;
    const char* release;
    const char* info;
    int count;
    const QUComponentDescription* components;
};



struct Q_EXPORT QUEnumItem 				// - a name/value pair
{
    const char *key;
    int value;
};

struct Q_EXPORT QUEnum
{			
    const char *name;			// - enumerator name
    unsigned int count;			// - number of values
    const QUEnumItem *items;		// - the name/value pairs
    bool set;				// whether enum has to be treated as a set
};

inline bool QUType::isEqual( const QUType *t1, const QUType *t2 ) {
    return t1 == t2 || t1->uuid() == t2->uuid() ||
	*(t1->uuid()) == *(t2->uuid());
}

inline bool QUType::check( QUObject* o, QUType* t ) {
    return isEqual( o->type, t ) || t->convertFrom( o, o->type );
}



// {7EE17B08-5419-47e2-9776-8EEA112DCAEC}
extern Q_EXPORT const QUuid TID_QUType_enum;
struct Q_EXPORT QUType_enum : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, int );
    int &get( QUObject * o ) { return o->payload.i; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_enum static_QUType_enum;


// {8AC26448-5AB4-49eb-968C-8F30AB13D732}
extern Q_EXPORT const QUuid TID_QUType_ptr;
struct Q_EXPORT QUType_ptr : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, const void* );
    void* &get( QUObject * o ) { return o->payload.ptr; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_ptr static_QUType_ptr;

// {97A2594D-6496-4402-A11E-55AEF2D4D25C}
extern Q_EXPORT const QUuid TID_QUType_iface;
struct Q_EXPORT QUType_iface : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, QUnknownInterface* );
    QUnknownInterface* &get( QUObject *o ){ return o->payload.iface; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_iface static_QUType_iface;

// {2F358164-E28F-4bf4-9FA9-4E0CDCABA50B}
extern Q_EXPORT const QUuid TID_QUType_idisp;
struct Q_EXPORT QUType_idisp : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, QDispatchInterface* );
    QDispatchInterface* &get( QUObject *o ){ return o->payload.idisp; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_idisp static_QUType_idisp;

// {CA42115D-13D0-456c-82B5-FC10187F313E}
extern Q_EXPORT const QUuid TID_QUType_bool;
struct Q_EXPORT QUType_bool : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, bool );
    bool &get( QUObject *o ) { return o->payload.b; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_bool static_QUType_bool;

// {53C1F3BE-73C3-4c7d-9E05-CCF09EB676B5}
extern Q_EXPORT const QUuid TID_QUType_int;
struct Q_EXPORT QUType_int : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, int );
    int &get( QUObject *o ) { return o->payload.i; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_int static_QUType_int;

// {5938712A-C496-11D5-8CB2-00C0F03BC0F3}
extern Q_EXPORT const QUuid TID_QUType_uint;
struct Q_EXPORT QUType_uint : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, uint );
    uint &get( QUObject *o ) { return o->payload.ui; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_uint static_QUType_uint;

// {2D0974E5-0BA6-4ec2-8837-C198972CB48C}
extern Q_EXPORT const QUuid TID_QUType_double;
struct Q_EXPORT QUType_double : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, double );
    double &get( QUObject *o ) { return o->payload.d; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_double static_QUType_double;

// {544C5175-6993-4486-B04D-CEC4D21BF4B9 }
extern Q_EXPORT const QUuid TID_QUType_float;
struct Q_EXPORT QUType_float : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, float );
    float &get( QUObject *o ) { return o->payload.f; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_float static_QUType_float;

// {EFCDD1D4-77A3-4b8e-8D46-DC14B8D393E9}
extern Q_EXPORT const QUuid TID_QUType_charstar;
struct Q_EXPORT QUType_charstar : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, const char*, bool take = FALSE );
    char* get( QUObject *o ){ return o->payload.charstar.ptr; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * );
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );

};
extern Q_EXPORT QUType_charstar static_QUType_charstar;

// {44C2A547-01E7-4e56-8559-35AF9D2F42B7}
extern const QUuid TID_QUType_QString;

struct Q_EXPORT QUType_QString : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, const QString & );
    QString &get( QUObject * o ) { return *(QString*)o->payload.ptr; }

    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * );
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );

};
extern Q_EXPORT QUType_QString static_QUType_QString;


#endif // QUCOM_H
