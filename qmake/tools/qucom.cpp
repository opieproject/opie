/****************************************************************************
** $Id: qucom.cpp,v 1.1 2002-11-01 00:10:45 kergoth Exp $
**
** Implementation of the QUcom classes
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

#include "qucom_p.h"

// Standard types

// {DE56510E-4E9F-4b76-A3C2-D1E2EF42F1AC}
const QUuid TID_QUType_Null( 0xde56510e, 0x4e9f, 0x4b76, 0xa3, 0xc2, 0xd1, 0xe2, 0xef, 0x42, 0xf1, 0xac );
const QUuid *QUType_Null::uuid() const { return &TID_QUType_Null; }
const char *QUType_Null::desc() const { return "null"; }
bool QUType_Null::canConvertFrom( QUObject *, QUType * ) { return FALSE; }
bool QUType_Null::canConvertTo( QUObject *, QUType * ) { return FALSE; }
bool QUType_Null::convertFrom( QUObject *, QUType * ) { return FALSE; }
bool QUType_Null::convertTo( QUObject *, QUType * ) { return FALSE; }
void QUType_Null::clear( QUObject *) {};
int QUType_Null::serializeTo( QUObject *, QUBuffer * ) { return 0; }
int QUType_Null::serializeFrom( QUObject *, QUBuffer * ) { return 0; };
QUType_Null static_QUType_Null;


// {7EE17B08-5419-47e2-9776-8EEA112DCAEC}
const QUuid TID_QUType_enum( 0x7ee17b08, 0x5419, 0x47e2, 0x97, 0x76, 0x8e, 0xea, 0x11, 0x2d, 0xca, 0xec );
QUType_enum static_QUType_enum;
const QUuid *QUType_enum::uuid() const { return &TID_QUType_enum; }
const char *QUType_enum::desc() const { return "enum"; }
void QUType_enum::set( QUObject *o, int v )
{
    o->payload.i = v;
    o->type = this;
}

bool QUType_enum::canConvertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_int ) ) // ## todo unsigned int?
	return TRUE;

    return t->canConvertTo( o, this );
}

bool QUType_enum::canConvertTo( QUObject * /*o*/, QUType *t )
{
    return isEqual( t, &static_QUType_int );
}

bool QUType_enum::convertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_int ) ) // ## todo unsigned int?
	;
    else
	return t->convertTo( o, this );

    o->type = this;
    return TRUE;
}

bool QUType_enum::convertTo( QUObject *o, QUType *t )
{
    if ( isEqual( t,  &static_QUType_int ) ) {
	o->type = &static_QUType_int;
	return TRUE;
    }
    return FALSE;
}

int QUType_enum::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_enum::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {8AC26448-5AB4-49eb-968C-8F30AB13D732}
const QUuid TID_QUType_ptr( 0x8ac26448, 0x5ab4, 0x49eb, 0x96, 0x8c, 0x8f, 0x30, 0xab, 0x13, 0xd7, 0x32 );
QUType_ptr static_QUType_ptr;
const QUuid *QUType_ptr::uuid() const  { return &TID_QUType_ptr; }
const char *QUType_ptr::desc() const { return "ptr"; }

void QUType_ptr::set( QUObject *o, const void* v )
{
    o->payload.ptr = (void*) v;
    o->type = this;
}

bool QUType_ptr::canConvertFrom( QUObject *o, QUType *t )
{
    return t->canConvertTo( o, this );
}

bool QUType_ptr::canConvertTo( QUObject *, QUType * )
{
    return FALSE;
}

bool QUType_ptr::convertFrom( QUObject *o, QUType *t )
{
    return t->convertTo( o, this );
}

bool QUType_ptr::convertTo( QUObject *, QUType * )
{
    return FALSE;
}

int QUType_ptr::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_ptr::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {97A2594D-6496-4402-A11E-55AEF2D4D25C}
const QUuid TID_QUType_iface( 0x97a2594d, 0x6496, 0x4402, 0xa1, 0x1e, 0x55, 0xae, 0xf2, 0xd4, 0xd2, 0x5c );
QUType_iface static_QUType_iface;
const QUuid *QUType_iface::uuid() const  { return &TID_QUType_iface; }
const char *QUType_iface::desc() const { return "UnknownInterface"; }

void QUType_iface::set( QUObject *o, QUnknownInterface* iface )
{
    o->payload.iface = iface;
    o->type = this;
}

bool QUType_iface::canConvertFrom( QUObject *o, QUType *t )
{
    return t->canConvertTo( o, this );
}

bool QUType_iface::canConvertTo( QUObject *, QUType * )
{
    return FALSE;
}

bool QUType_iface::convertFrom( QUObject *o, QUType *t )
{
    return t->convertTo( o, this );
}

bool QUType_iface::convertTo( QUObject *, QUType * )
{
    return FALSE;
}

int QUType_iface::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_iface::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {2F358164-E28F-4bf4-9FA9-4E0CDCABA50B}
const QUuid TID_QUType_idisp( 0x2f358164, 0xe28f, 0x4bf4, 0x9f, 0xa9, 0x4e, 0xc, 0xdc, 0xab, 0xa5, 0xb );
QUType_idisp static_QUType_idisp;
const QUuid *QUType_idisp::uuid() const  { return &TID_QUType_idisp; }
const char *QUType_idisp::desc() const { return "DispatchInterface"; }

void QUType_idisp::set( QUObject *o, QDispatchInterface* idisp )
{
    o->payload.idisp = idisp;
    o->type = this;
}

bool QUType_idisp::canConvertFrom( QUObject *o, QUType *t )
{
    return t->canConvertTo( o, this );
}

bool QUType_idisp::canConvertTo( QUObject * /*o*/, QUType *t )
{
    return isEqual( t, &static_QUType_iface );
}

bool QUType_idisp::convertFrom( QUObject *o, QUType *t )
{
    return t->convertTo( o, this );
}

bool QUType_idisp::convertTo( QUObject *o, QUType *t )
{
#ifndef QT_NO_COMPONENT
    if ( isEqual( t, &static_QUType_iface ) ) {
	o->payload.iface = (QUnknownInterface*)o->payload.idisp;
	o->type = &static_QUType_iface;
	return TRUE;
    }
#endif
    return FALSE;
}

int QUType_idisp::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_idisp::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {CA42115D-13D0-456c-82B5-FC10187F313E}
const QUuid TID_QUType_bool( 0xca42115d, 0x13d0, 0x456c, 0x82, 0xb5, 0xfc, 0x10, 0x18, 0x7f, 0x31, 0x3e );
QUType_bool static_QUType_bool;
const QUuid *QUType_bool::uuid() const  { return &TID_QUType_bool; }
const char *QUType_bool::desc() const { return "bool"; }

void QUType_bool::set( QUObject *o, bool v )
{
    o->payload.b = v;
    o->type = this;
}

bool QUType_bool::canConvertFrom( QUObject *o, QUType *t )
{
    return t->canConvertTo( o, this );
}

bool QUType_bool::canConvertTo( QUObject *, QUType * )
{
    return FALSE;
}

bool QUType_bool::convertFrom( QUObject *o, QUType *t )
{
    return t->convertTo( o, this );
}

bool QUType_bool::convertTo( QUObject *, QUType * )
{
    return FALSE;
}

int QUType_bool::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_bool::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {53C1F3BE-73C3-4c7d-9E05-CCF09EB676B5}
const QUuid TID_QUType_int( 0x53c1f3be, 0x73c3, 0x4c7d, 0x9e, 0x5, 0xcc, 0xf0, 0x9e, 0xb6, 0x76, 0xb5 );
QUType_int static_QUType_int;
const QUuid *QUType_int::uuid() const  { return &TID_QUType_int; }
const char *QUType_int::desc() const { return "int"; }

void QUType_int::set( QUObject *o, int v )
{
    o->payload.i = v;
    o->type = this;
}

bool QUType_int::canConvertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_double ) ||
	 isEqual( t, &static_QUType_float ) )
	return TRUE;

    return t->canConvertTo( o, this );
}

bool QUType_int::canConvertTo( QUObject * /*o*/, QUType *t )
{
    return isEqual( t,  &static_QUType_double ) ||
	isEqual( t, &static_QUType_float );
}

bool QUType_int::convertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_double ) )
	o->payload.i = (long)o->payload.d;
    else if ( isEqual( t, &static_QUType_float ) )
	o->payload.i = (long)o->payload.f;
    else
	return t->convertTo( o, this );

    o->type = this;
    return TRUE;
}

bool QUType_int::convertTo( QUObject *o, QUType *t )
{
    if ( isEqual( t,  &static_QUType_double ) ) {
	o->payload.d = (double)o->payload.i;
	o->type = &static_QUType_double;
    } else if ( isEqual( t,  &static_QUType_float ) ) {	
	o->payload.f = (float)o->payload.i;
	o->type = &static_QUType_float;
    } else
	return FALSE;
    return TRUE;
}

int QUType_int::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_int::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {5938712A-C496-11D5-8CB2-00C0F03BC0F3}
const QUuid TID_QUType_uint( 0x5938712a, 0xc496, 0x11d5, 0x8c, 0xb2, 0x00, 0xc0, 0xf0, 0x3b, 0xc0, 0xf3);
QUType_uint static_QUType_uint;
const QUuid *QUType_uint::uuid() const  { return &TID_QUType_uint; }
const char *QUType_uint::desc() const { return "uint"; }

void QUType_uint::set( QUObject *o, uint v )
{
    o->payload.ui = v;
    o->type = this;
}

bool QUType_uint::canConvertFrom( QUObject *o, QUType *t )
{
    return t->canConvertTo( o, this );
}

bool QUType_uint::canConvertTo( QUObject * /*o*/, QUType * /*t*/ )
{
    return FALSE;
}

bool QUType_uint::convertFrom( QUObject *o, QUType *t )
{
    return t->convertTo( o, this );
}

bool QUType_uint::convertTo( QUObject * /*o*/, QUType * /*t*/ )
{
    return FALSE;
}

int QUType_uint::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_uint::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {2D0974E5-0BA6-4ec2-8837-C198972CB48C}
const QUuid TID_QUType_double( 0x2d0974e5, 0xba6, 0x4ec2, 0x88, 0x37, 0xc1, 0x98, 0x97, 0x2c, 0xb4, 0x8c );
QUType_double static_QUType_double;
const QUuid *QUType_double::uuid() const { return &TID_QUType_double; }
const char *QUType_double::desc() const {return "double"; }

void QUType_double::set( QUObject *o, double v )
{
    o->payload.d = v;
    o->type = this;
}

bool QUType_double::canConvertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_int ) ||
	 isEqual( t, &static_QUType_float) )
	return TRUE;

    return t->canConvertTo( o, this );
}

bool QUType_double::canConvertTo( QUObject * /*o*/, QUType *t )
{
    return isEqual( t,  &static_QUType_int ) ||
	isEqual( t,  &static_QUType_float );
}

bool QUType_double::convertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_int ) )
	o->payload.d = (double)o->payload.i;
    else if ( isEqual( t, &static_QUType_float ) )
	o->payload.d = (double)o->payload.f;
    else
	return t->convertTo( o, this );

    o->type = this;
    return TRUE;
}

bool QUType_double::convertTo( QUObject *o, QUType *t )
{
    if ( isEqual( t,  &static_QUType_int ) ) {
	o->payload.i = (int) o->payload.d;
	o->type = &static_QUType_int;
    } else if ( isEqual( t,  &static_QUType_double ) ) {
	o->payload.d = (double) o->payload.f;
	o->type = &static_QUType_double;
    } else
	return FALSE;
    return TRUE;
}

int QUType_double::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_double::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}


// {544C5175-6993-4486-B04D-CEC4D21BF4B9 }
const QUuid TID_QUType_float( 0x544c5175, 0x6993, 0x4486, 0xb0, 0x4d, 0xce, 0xc4, 0xd2, 0x1b, 0xf4, 0xb9 );
QUType_float static_QUType_float;
const QUuid *QUType_float::uuid() const { return &TID_QUType_float; }
const char *QUType_float::desc() const {return "float"; }

void QUType_float::set( QUObject *o, float v )
{
    o->payload.f = v;
    o->type = this;
}

bool QUType_float::canConvertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_int ) ||
	 isEqual( t, &static_QUType_double ) )
	return TRUE;

    return t->canConvertTo( o, this );
}

bool QUType_float::canConvertTo( QUObject * /*o*/, QUType *t )
{
    return isEqual( t,  &static_QUType_int ) ||
	isEqual( t,  &static_QUType_double );
}

bool QUType_float::convertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_int ) )
	o->payload.f = (float)o->payload.i;
    else if ( isEqual( t, &static_QUType_double ) )
	o->payload.f = (float)o->payload.d;
    else
	return t->convertTo( o, this );

    o->type = this;
    return TRUE;
}

bool QUType_float::convertTo( QUObject *o, QUType *t )
{
    if ( isEqual( t,  &static_QUType_int ) ) {
	o->payload.i = (int) o->payload.f;
	o->type = &static_QUType_int;
    } else if ( isEqual( t,  &static_QUType_double ) ) {
	o->payload.d = (double) o->payload.f;
	o->type = &static_QUType_double;
    } else
	return FALSE;
    return TRUE;
}

int QUType_float::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_float::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

// {EFCDD1D4-77A3-4b8e-8D46-DC14B8D393E9}
const QUuid TID_QUType_charstar( 0xefcdd1d4, 0x77a3, 0x4b8e, 0x8d, 0x46, 0xdc, 0x14, 0xb8, 0xd3, 0x93, 0xe9 );
QUType_charstar static_QUType_charstar;
const QUuid *QUType_charstar::uuid() const { return &TID_QUType_charstar; }
const char *QUType_charstar::desc() const { return "char*"; }

void QUType_charstar::set( QUObject *o, const char* v, bool take )
{
    if ( take ) {
	if ( v ) {
	    o->payload.charstar.ptr = new char[ strlen(v) + 1 ];
	    strcpy( o->payload.charstar.ptr, v );
	} else {
	    o->payload.charstar.ptr = 0;
	}
	o->payload.charstar.owner = TRUE;
    } else {
	o->payload.charstar.ptr = (char*) v;
	o->payload.charstar.owner = FALSE;
    }
    o->type = this;
}

bool QUType_charstar::canConvertFrom( QUObject *o, QUType *t )
{
    return t->canConvertTo( o, this );
}

bool QUType_charstar::canConvertTo( QUObject *, QUType * )
{
    return FALSE;
}

bool QUType_charstar::convertFrom( QUObject *o, QUType *t )
{
    return t->convertTo( o, this );
}

bool QUType_charstar::convertTo( QUObject *, QUType * )
{
    return FALSE;
}

void QUType_charstar::clear( QUObject *o )
{
    if ( o->payload.charstar.owner )
	delete [] o->payload.charstar.ptr;
    o->payload.charstar.ptr = 0;
}

int QUType_charstar::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_charstar::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}


// Qt specific types

// {44C2A547-01E7-4e56-8559-35AF9D2F42B7}
const QUuid TID_QUType_QString( 0x44c2a547, 0x1e7, 0x4e56, 0x85, 0x59, 0x35, 0xaf, 0x9d, 0x2f, 0x42, 0xb7 );
QUType_QString static_QUType_QString;
const QUuid *QUType_QString::uuid() const { return &TID_QUType_QString; }
const char *QUType_QString::desc() const { return "QString"; }

void QUType_QString::set( QUObject *o, const QString& v )
{
    o->payload.ptr = new QString( v );
    o->type = this;
}

bool QUType_QString::canConvertFrom( QUObject *o, QUType *t )
{
    if ( isEqual( t, &static_QUType_charstar ) ||
	 isEqual( t, &static_QUType_double ) ||
	 isEqual( t, &static_QUType_float ) ||
	 isEqual( t, &static_QUType_int ) )
	return TRUE;

    return t->canConvertTo( o, this );
}

bool QUType_QString::canConvertTo( QUObject * /*o*/, QUType *t )
{
    return isEqual( t, &static_QUType_charstar ) ||
	isEqual( t,  &static_QUType_int ) ||
	isEqual( t,  &static_QUType_double ) ||
	isEqual( t,  &static_QUType_float );
}

bool QUType_QString::convertFrom( QUObject *o, QUType *t )
{
    QString *str = 0;
    if ( isEqual( t, &static_QUType_charstar ) )
	str = new QString( o->payload.charstar.ptr );
    else if ( isEqual( t, &static_QUType_double ) )
	str = new QString( QString::number( o->payload.d ) );
    else if ( isEqual( t, &static_QUType_float ) )
	str = new QString( QString::number( o->payload.f ) );
    else if ( isEqual( t, &static_QUType_int ) )
	str = new QString( QString::number( o->payload.i ) );
    else
	return t->convertTo( o, this );

    o->type->clear( o );
    o->payload.ptr = str;
    o->type = this;
    return TRUE;
}

bool QUType_QString::convertTo( QUObject *o, QUType *t )
{
    QString *str = (QString *)o->payload.ptr;
    if ( isEqual( t, &static_QUType_charstar ) ) {
	o->payload.charstar.ptr = qstrdup( str->local8Bit().data() );
	o->payload.charstar.owner = TRUE;
	o->type = &static_QUType_charstar;
    } else if ( isEqual( t,  &static_QUType_int ) ) {
	o->payload.l = str->toLong();
	o->type = &static_QUType_int;
    } else if ( isEqual( t,  &static_QUType_double ) ) {
	o->payload.d = str->toDouble();
	o->type = &static_QUType_double;
    } else if ( isEqual( t,  &static_QUType_float ) ) {
	o->payload.d = str->toFloat();
	o->type = &static_QUType_float;
    } else {
        return FALSE;
    }
    delete str;
    return TRUE;
}

int QUType_QString::serializeTo( QUObject *, QUBuffer * )
{
    return 0;
}

int QUType_QString::serializeFrom( QUObject *, QUBuffer * )
{
    return 0;
}

void QUType_QString::clear( QUObject *o )
{
    delete (QString*)o->payload.ptr;
    o->payload.ptr = 0;
}
