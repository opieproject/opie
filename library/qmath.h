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
#ifndef QMATH_H
#define QMATH_H

#ifdef __cplusplus
extern "C"
{
#endif

double qSqrt( double value );
double qSin( double a );
double qCos( double a );
double qATan2( double y, double x );
double qATan( double a );
double qASin( double a );
double qTan( double a );
double qFloor( double a );
double qFabs( double a );

#ifdef __cplusplus
}
#endif

#endif
