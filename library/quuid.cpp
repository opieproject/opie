/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#include "quuid.h"

/*!
  \class QUuid quuid.h
  \brief The QUuid class defines a Universally Unique Identifier (UUID).

  This class is temporarily copied from Qt 3.0.
*/

/*!
  \fn QUuid::QUuid()

  Creates the null UUID {00000000-0000-0000-0000-000000000000}.
*/

/*!
  \fn QUuid::QUuid( uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8 )

  Creates an UUID with the value specified by the parameters, \a l, \a
  w1, \a w2, \a b1, \a b2, \a b3, \a b4, \a b5, \a b6, \a b7, \a b8.

  Example:
  \code
  // {67C8770B-44F1-410A-AB9A-F9B5446F13EE}
  QUuid IID_MyInterface( 0x67c8770b, 0x44f1, 0x410a, 0xab, 0x9a, 0xf9, 0xb5, 0x44, 0x6f, 0x13, 0xee )
  \endcode
*/

/*!
  \fn QUuid::QUuid( const QUuid &orig )

  Creates a copy of the QUuid \a orig.
*/
#ifndef QT_NO_QUUID_STRING
/*!
  Creates a QUuid object from the string \a text. Right now, the function
  can only convert the format {12345678-1234-1234-1234-123456789ABC} and
  will create the null UUID when the conversion fails.
*/
QUuid::QUuid( const QString &text )
{
    bool ok;
    QString temp = text.upper();

    data1 = temp.mid( 1, 8 ).toULong( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }

    data2 = temp.mid( 10, 4 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    data3 = temp.mid( 15, 4 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    data4[0] = temp.mid( 20, 2 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    data4[1] = temp.mid( 22, 2 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    for ( int i = 2; i<8; i++ ) {
	data4[i] = temp.mid( 25 + (i-2)*2, 2 ).toUShort( &ok, 16 );
	if ( !ok ) {
	    *this = QUuid();
	    return;
	}
    }
}
#endif
/*!
  \fn QUuid QUuid::operator=(const QUuid &uuid )

  Assigns the value of \a uuid to this QUuid object.
*/

/*!
  \fn bool QUuid::operator==(const QUuid &other) const

  Returns TRUE if this QUuid and the \a other QUuid are identical, otherwise returns FALSE.
*/

/*!
  \fn bool QUuid::operator!=(const QUuid &other) const

  Returns TRUE if this QUuid and the \a other QUuid are different, otherwise returns FALSE.
*/
#ifndef QT_NO_QUUID_STRING
/*!
  QString QUuid::toString() const

  Returns a string in {12345678-1234-1234-1234-123456789ABC} format.
*/
QString QUuid::toString() const
{
    QString result;

    result = "{" + QString::number( data1, 16 ).rightJustify( 8, '0' ) + "-";
    result += QString::number( (int)data2, 16 ).rightJustify( 4, '0' ) + "-";
    result += QString::number( (int)data3, 16 ).rightJustify( 4, '0' ) + "-";
    result += QString::number( (int)data4[0], 16 ).rightJustify( 2, '0' );
    result += QString::number( (int)data4[1], 16 ).rightJustify( 2, '0' ) + "-";
    for ( int i = 2; i < 8; i++ )
	result += QString::number( (int)data4[i], 16 ).rightJustify( 2, '0' );

    return result + "}";
}
#endif
/*!
  Returns TRUE if this is the null UUID {00000000-0000-0000-0000-000000000000}, otherwise returns FALSE.
*/
bool QUuid::isNull() const
{
    return data4[0] == 0 && data4[1] == 0 && data4[2] == 0 && data4[3] == 0 &&
	   data4[4] == 0 && data4[5] == 0 && data4[6] == 0 && data4[7] == 0 &&
	   data1 == 0 && data2 == 0 && data3 == 0;
}
