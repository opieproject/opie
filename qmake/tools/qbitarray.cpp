/****************************************************************************
** $Id: qbitarray.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QBitArray class
**
** Created : 940118
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
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

#include "qbitarray.h"
#include "qdatastream.h"

#define SHBLOCK	 ((bitarr_data*)(sharedBlock()))


/*!
    \class QBitVal qbitarray.h
    \reentrant
    \brief The QBitVal class is an internal class, used with QBitArray.

    \ingroup collection

    The QBitVal is required by the indexing [] operator on bit arrays.
    It is not for use in any other context.
*/

/*!
    \fn QBitVal::QBitVal (QBitArray* a, uint i)

    Constructs a reference to element \a i in the QBitArray \a a.
    This is what QBitArray::operator[] constructs its return value
    with.
*/

/*!
    \fn QBitVal::operator int()

    Returns the value referenced by the QBitVal.
*/

/*!
    \fn QBitVal& QBitVal::operator= (const QBitVal& v)

    Sets the value referenced by the QBitVal to that referenced by
    QBitVal \a v.
*/

/*!
    \overload QBitVal& QBitVal::operator= (bool v)

    Sets the value referenced by the QBitVal to \a v.
*/


/*!
    \class QBitArray qbitarray.h
    \reentrant
    \brief The QBitArray class provides an array of bits.

    \ingroup collection
    \ingroup tools
    \ingroup shared

    Because QBitArray is a QMemArray, it uses explicit \link
    shclass.html sharing\endlink with a reference count.

    A QBitArray is a special byte array that can access individual
    bits and perform bit-operations (AND, OR, XOR and NOT) on entire
    arrays or bits.

    Bits can be manipulated by the setBit() and clearBit() functions,
    but it is also possible to use the indexing [] operator to test
    and set individual bits. The [] operator is a little slower than
    setBit() and clearBit() because some tricks are required to
    implement single-bit assignments.

    Example:
    \code
    QBitArray a(3);
    a.setBit( 0 );
    a.clearBit( 1 );
    a.setBit( 2 );     // a = [1 0 1]

    QBitArray b(3);
    b[0] = 1;
    b[1] = 1;
    b[2] = 0;          // b = [1 1 0]

    QBitArray c;
    c = ~a & b;        // c = [0 1 0]
    \endcode

    When a QBitArray is constructed the bits are uninitialized. Use
    fill() to set all the bits to 0 or 1. The array can be resized
    with resize() and copied with copy(). Bits can be set with
    setBit() and cleared with clearBit(). Bits can be toggled with
    toggleBit(). A bit's value can be obtained with testBit() and with
    at().

    QBitArray supports the \& (AND), | (OR), ^ (XOR) and ~ (NOT)
    operators.
*/

/*! \class QBitArray::bitarr_data
  \brief The QBitArray::bitarr_data class is internal.
  \internal
*/


/*!
    Constructs an empty bit array.
*/

QBitArray::QBitArray() : QByteArray( 0, 0 )
{
    bitarr_data *x = new bitarr_data;
    Q_CHECK_PTR( x );
    x->nbits = 0;
    setSharedBlock( x );
}

/*!
    Constructs a bit array of \a size bits. The bits are uninitialized.

    \sa fill()
*/

QBitArray::QBitArray( uint size ) : QByteArray( 0, 0 )
{
    bitarr_data *x = new bitarr_data;
    Q_CHECK_PTR( x );
    x->nbits = 0;
    setSharedBlock( x );
    resize( size );
}

/*!
    \fn QBitArray::QBitArray( const QBitArray &a )

    Constructs a shallow copy of \a a.
*/

/*!
    \fn QBitArray &QBitArray::operator=( const QBitArray &a )

    Assigns a shallow copy of \a a to this bit array and returns a
    reference to this array.
*/


/*!
    Pad last byte with 0-bits.
*/
void QBitArray::pad0()
{
    uint sz = size();
    if ( sz && sz%8 )
	*(data()+sz/8) &= (1 << (sz%8)) - 1;
}


/*!
    \fn uint QBitArray::size() const

    Returns the bit array's size (number of bits).

    \sa resize()
*/

/*!
    Resizes the bit array to \a size bits and returns TRUE if the bit
    array could be resized; otherwise returns FALSE.

    If the array is expanded, the new bits are set to 0.

    \sa size()
*/

bool QBitArray::resize( uint size )
{
    uint s = this->size();
    if ( !QByteArray::resize( (size+7)/8 ) )
	return FALSE;				// cannot resize
    SHBLOCK->nbits = size;
    if ( size != 0 ) {				// not null array
	int ds = (int)(size+7)/8 - (int)(s+7)/8;// number of bytes difference
	if ( ds > 0 )				// expanding array
	    memset( data() + (s+7)/8, 0, ds );	//   reset new data
    }
    return TRUE;
}


/*!
    Fills the bit array with \a v (1's if \a v is TRUE, or 0's if \a v
    is FALSE).

    fill() resizes the bit array to \a size bits if \a size is
    nonnegative.

    Returns FALSE if a nonnegative \e size was specified and the bit
    array could not be resized; otherwise returns TRUE.

    \sa resize()
*/

bool QBitArray::fill( bool v, int size )
{
    if ( size >= 0 ) {				// resize first
	if ( !resize( size ) )
	    return FALSE;			// cannot resize
    } else {
	size = this->size();
    }
    if ( size > 0 )
	memset( data(), v ? 0xff : 0, (size + 7) / 8 );
    if ( v )
	pad0();
    return TRUE;
}


/*!
    Detaches from shared bit array data and makes sure that this bit
    array is the only one referring to the data.

    If multiple bit arrays share common data, this bit array
    dereferences the data and gets a copy of the data. Nothing happens
    if there is only a single reference.

    \sa copy()
*/

void QBitArray::detach()
{
    int nbits = SHBLOCK->nbits;
    this->duplicate( *this );
    SHBLOCK->nbits = nbits;
}

/*!
    Returns a deep copy of the bit array.

    \sa detach()
*/

QBitArray QBitArray::copy() const
{
    QBitArray tmp;
    tmp.duplicate( *this );
    ((bitarr_data*)(tmp.sharedBlock()))->nbits = SHBLOCK->nbits;
    return tmp;
}


/*!
    Returns TRUE if the bit at position \a index is set, i.e. is 1;
    otherwise returns FALSE.

    \sa setBit(), clearBit()
*/

bool QBitArray::testBit( uint index ) const
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	qWarning( "QBitArray::testBit: Index %d out of range", index );
	return FALSE;
    }
#endif
    return (*(data()+(index>>3)) & (1 << (index & 7))) != 0;
}

/*!
    \overload

    Sets the bit at position \a index to 1.

    \sa clearBit() toggleBit()
*/

void QBitArray::setBit( uint index )
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	qWarning( "QBitArray::setBit: Index %d out of range", index );
	return;
    }
#endif
    *(data()+(index>>3)) |= (1 << (index & 7));
}

/*!
    \fn void QBitArray::setBit( uint index, bool value )

    Sets the bit at position \a index to \a value.

    Equivalent to:
    \code
    if ( value )
	setBit( index );
    else
	clearBit( index );
    \endcode

    \sa clearBit() toggleBit()
*/

/*!
    Clears the bit at position \a index, i.e. sets it to 0.

    \sa setBit(), toggleBit()
*/

void QBitArray::clearBit( uint index )
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	qWarning( "QBitArray::clearBit: Index %d out of range", index );
	return;
    }
#endif
    *(data()+(index>>3)) &= ~(1 << (index & 7));
}

/*!
    Toggles the bit at position \a index.

    If the previous value was 0, the new value will be 1. If the
    previous value was 1, the new value will be 0.

    \sa setBit(), clearBit()
*/

bool QBitArray::toggleBit( uint index )
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	qWarning( "QBitArray::toggleBit: Index %d out of range", index );
	return FALSE;
    }
#endif
    register uchar *p = (uchar *)data() + (index>>3);
    uchar b = (1 << (index & 7));		// bit position
    uchar c = *p & b;				// read bit
    *p ^= b;					// toggle bit
    return c;
}


/*!
    \fn bool QBitArray::at( uint index ) const

    Returns the value (0 or 1) of the bit at position \a index.

    \sa operator[]()
*/

/*!
    \fn QBitVal QBitArray::operator[]( int index )

    Implements the [] operator for bit arrays.

    The returned QBitVal is a context object. It makes it possible to
    get and set a single bit value by its \a index position.

    Example:
    \code
    QBitArray a( 3 );
    a[0] = 0;
    a[1] = 1;
    a[2] = a[0] ^ a[1];
    \endcode

    The functions testBit(), setBit() and clearBit() are faster.

    \sa at()
*/

/*!
    \overload bool QBitArray::operator[]( int index ) const

    Implements the [] operator for constant bit arrays.
*/


/*!
    Performs the AND operation between all bits in this bit array and
    \a a. Returns a reference to this bit array.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.
    \code
    QBitArray a( 3 ), b( 2 );
    a[0] = 1;  a[1] = 0;  a[2] = 1;     // a = [1 0 1]
    b[0] = 1;  b[1] = 0;                // b = [1 0]
    a &= b;                             // a = [1 0 0]
    \endcode

    \sa operator|=(), operator^=(), operator~()
*/

QBitArray &QBitArray::operator&=( const QBitArray &a )
{
    resize( QMAX(size(), a.size()) );
    register uchar *a1 = (uchar *)data();
    register uchar *a2 = (uchar *)a.data();
    int n = QMIN( QByteArray::size(), a.QByteArray::size() );
    int p = QMAX( QByteArray::size(), a.QByteArray::size() ) - n;
    while ( n-- > 0 )
	*a1++ &= *a2++;
    while ( p-- > 0 )
	*a1++ = 0;
    return *this;
}

/*!
    Performs the OR operation between all bits in this bit array and
    \a a. Returns a reference to this bit array.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.
    \code
    QBitArray a( 3 ), b( 2 );
    a[0] = 1;  a[1] = 0;  a[2] = 1;     // a = [1 0 1]
    b[0] = 1;  b[1] = 0;                // b = [1 0]
    a |= b;                             // a = [1 0 1]
    \endcode

    \sa operator&=(), operator^=(), operator~()
*/

QBitArray &QBitArray::operator|=( const QBitArray &a )
{
    resize( QMAX(size(), a.size()) );
    register uchar *a1 = (uchar *)data();
    register uchar *a2 = (uchar *)a.data();
    int n = QMIN( QByteArray::size(), a.QByteArray::size() );
    while ( n-- > 0 )
	*a1++ |= *a2++;
    return *this;
}

/*!
    Performs the XOR operation between all bits in this bit array and
    \a a. Returns a reference to this bit array.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.
    \code
    QBitArray a( 3 ), b( 2 );
    a[0] = 1;  a[1] = 0;  a[2] = 1;     // a = [1 0 1]
    b[0] = 1;  b[1] = 0;                // b = [1 0]
    a ^= b;                             // a = [0 0 1]
    \endcode

    \sa operator&=(), operator|=(), operator~()
*/

QBitArray &QBitArray::operator^=( const QBitArray &a )
{
    resize( QMAX(size(), a.size()) );
    register uchar *a1 = (uchar *)data();
    register uchar *a2 = (uchar *)a.data();
    int n = QMIN( QByteArray::size(), a.QByteArray::size() );
    while ( n-- > 0 )
	*a1++ ^= *a2++;
    return *this;
}

/*!
    Returns a bit array that contains the inverted bits of this bit array.

    Example:
    \code
    QBitArray a( 3 ), b;
    a[0] = 1;  a[1] = 0; a[2] = 1;	// a = [1 0 1]
    b = ~a;				// b = [0 1 0]
    \endcode
*/

QBitArray QBitArray::operator~() const
{
    QBitArray a( size() );
    register uchar *a1 = (uchar *)data();
    register uchar *a2 = (uchar *)a.data();
    int n = QByteArray::size();
    while ( n-- )
	*a2++ = ~*a1++;
    a.pad0();
    return a;
}


/*!
    \relates QBitArray

    Returns the AND result between the bit arrays \a a1 and \a a2.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.

    \sa QBitArray::operator&=()
*/

QBitArray operator&( const QBitArray &a1, const QBitArray &a2 )
{
    QBitArray tmp = a1.copy();
    tmp &= a2;
    return tmp;
}

/*!
    \relates QBitArray

    Returns the OR result between the bit arrays \a a1 and \a a2.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.

    \sa QBitArray::operator|=()
*/

QBitArray operator|( const QBitArray &a1, const QBitArray &a2 )
{
    QBitArray tmp = a1.copy();
    tmp |= a2;
    return tmp;
}

/*!
    \relates QBitArray

    Returns the XOR result between the bit arrays \a a1 and \a a2.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.

    \sa QBitArray::operator^()
*/

QBitArray operator^( const QBitArray &a1, const QBitArray &a2 )
{
    QBitArray tmp = a1.copy();
    tmp ^= a2;
    return tmp;
}


/* \enum QGArray::array_data

  \warning This will be renamed in the next major release of Qt.  Until
  then it is undocumented and we recommend against its use.

  \internal

  ### 3.0 rename ###
  ### 3.0 move it to QGArray? ###
*/


/*!
    \fn QBitArray::array_data * QBitArray::newData()

    \internal

    Returns data specific to QBitArray that extends what QGArray provides.
    QPtrCollection mechanism for allowing extra/different data.
*/


/*!
    \fn void  QBitArray::deleteData ( array_data * d )

    \internal

    Deletes data specific to QBitArray that extended what QGArray provided.

    QPtrCollection mechanism for allowing extra/different data.
*/


/*****************************************************************************
  QBitArray stream functions
 *****************************************************************************/

/*!
    \relates QBitArray

    Writes bit array \a a to stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/
#ifndef QT_NO_DATASTREAM
QDataStream &operator<<( QDataStream &s, const QBitArray &a )
{
    Q_UINT32 len = a.size();
    s << len;					// write size of array
    if ( len > 0 )				// write data
	s.writeRawBytes( a.data(), a.QByteArray::size() );
    return s;
}

/*!
    \relates QBitArray

    Reads a bit array into \a a from stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/

QDataStream &operator>>( QDataStream &s, QBitArray &a )
{
    Q_UINT32 len;
    s >> len;					// read size of array
    if ( !a.resize( (uint)len ) ) {		// resize array
#if defined(QT_CHECK_NULL)
	qWarning( "QDataStream: Not enough memory to read QBitArray" );
#endif
	len = 0;
    }
    if ( len > 0 )				// read data
	s.readRawBytes( a.data(), a.QByteArray::size() );
    return s;
}

#endif // QT_NO_DATASTREAM
