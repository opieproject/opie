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

#include <math.h>
#include <float.h>
#include "qmath.h"

#ifdef QT_QWS_CASSIOPEIA

double qFabs( double a )
{
    if ( a < 0.0 )
	return -a;
    return a;
}

double qSqrt( double value )
{
    const double tol = 0.000005; // relative error tolerance
    double old_app, new_app;
    if (value == 0.0)
	return 0.0;
    old_app = value; // take value as first approximation
    new_app = (old_app + value/old_app)/2;
    while (qFabs((new_app-old_app)/new_app) > tol)
    {
	old_app = new_app;
	new_app = (old_app + value/old_app)/2;
    }

    return new_app;
}

const double Q_PI   = 3.14159265358979323846;   // pi
const double Q_2PI  = 6.28318530717958647693;   // 2*pi
const double Q_PI2  = 1.57079632679489661923;   // pi/2

static double qsincos( double a, int calcCos )
{
    int sign;
    double a2;
    double a3;
    double a5;
    double a7;
    double a9;
    double a11;

    if ( calcCos )                              // calculate cosine
	a -= Q_PI2;
    if ( a >= Q_2PI || a <= -Q_2PI ) {          // fix range: -2*pi < a < 2*pi
	int m = (int)(a/Q_2PI);
	a -= Q_2PI*m;
    }
    if ( a < 0.0 )                              // 0 <= a < 2*pi
	a += Q_2PI;
    sign = a > Q_PI ? -1 : 1;
    if ( a >= Q_PI )
	a = Q_2PI - a;
    if ( a >= Q_PI2 )
	a = Q_PI - a;
    if ( calcCos )
	sign = -sign;
    a2  = a*a;                           // here: 0 <= a < pi/4
    a3  = a2*a;                          // make taylor sin sum
    a5  = a3*a2;
    a7  = a5*a2;
    a9  = a7*a2;
    a11 = a9*a2;
    return (a-a3/6+a5/120-a7/5040+a9/362880-a11/39916800)*sign;
}

double qSin( double a ) { return qsincos(a,0); }
double qCos( double a ) { return qsincos(a,1); }

//atan2 returns values from -PI to PI, so we have to do the same
double qATan2( double y, double x )
{
    double r;
    if ( x != 0.0 ) {
	double a = qFabs(y/x);
	if ( a <= 1 )
	    r = a/(1+ 0.28*a*a);
	else
	    r = Q_PI2 - a/(a*a + 0.28);
    } else {
	r = Q_PI2;
    }

    if ( y >= 0.0 ) {
	if ( x >= 0.0 )
	    return r;
	else
	    return Q_PI - r;
    } else {
	if ( x >= 0.0 )
	    return 0.0 - r;
	else
	    return -Q_PI + r;
    }
}

double qATan( double a )
{
    return qATan2( a, 1.0 );
}

double qASin( double a )
{
    return qATan2( a, qSqrt(1-a*a) );
}

double qTan( double a )
{
    double ca = qCos(a);
    if ( ca != 0.0 )
	return qSin( a ) / ca;

    return MAXDOUBLE;
}

double qFloor( double a )
{
    long i = (long) a;
    return (double) i;
}

#else

double qSqrt( double value ) { return sqrt( value ); }
double qSin( double a ) { return sin(a); }
double qCos( double a ) { return cos(a); }
double qATan2( double y, double x ) { return atan2(y,x); }
double qATan( double a ) { return atan(a); }
double qASin( double a ) { return asin(a); }
double qTan( double a ) { return tan(a); }
double qFloor( double a ) { return floor(a); }
double qFabs( double a ) { return fabs(a); }

#endif

