/*
 * Sun clock.  X11 version by John Mackin.
 *
 * This program was derived from, and is still in part identical with, the
 * Suntools Sun clock program whose author's comment appears immediately
 * below.  Please preserve both notices.
 *
 * The X11R3/4 version of this program was written by John Mackin, at the
 * Basser Department of Computer Science, University of Sydney, Sydney,
 * New South Wales, Australia; <john@cs.su.oz.AU>.  This program, like
 * the one it was derived from, is in the public domain: `Love is the
 * law, love under will.'
 */

/*

	Sun clock

	Designed and implemented by John Walker in November of 1988.

	Version for the Sun Workstation.

    The algorithm used to calculate the position of the Sun is given in
    Chapter 18 of:

    "Astronomical  Formulae for Calculators" by Jean Meeus, Third Edition,
    Richmond: Willmann-Bell, 1985.  This book can be obtained from:

       Willmann-Bell
       P.O. Box 35025
       Richmond, VA  23235
       USA
       Phone: (804) 320-7016

    This program was written by:

       John Walker
       Autodesk, Inc.
       2320 Marinship Way
       Sausalito, CA  94965
       USA
       Fax:   (415) 389-9418
       Voice: (415) 332-2344 Ext. 2829
       Usenet: {sun,well,uunet}!acad!kelvin
	   or: kelvin@acad.uu.net

    modified for interactive maps by

	Stephen Martin
	Fujitsu Systems Business of Canada
	smartin@fujitsu.ca

    This  program is in the public domain: "Do what thou wilt shall be the
    whole of the law".  I'd appreciate  receiving  any  bug  fixes  and/or
    enhancements,  which  I'll  incorporate  in  future  versions  of  the
    program.  Please leave the original attribution information intact	so
    that credit and blame may be properly apportioned.

    Revision history:

	1.0  12/21/89  Initial version.
	      8/24/89  Finally got around to submitting.

	1.1   8/31/94  Version with interactive map.
	1.2  10/12/94  Fixes for HP and Solaris, new icon bitmap
	1.3  11/01/94  Timezone now shown in icon
	1.4  03/29/98  Fixed city drawing, added icon animation

*/

#include "sun.h"

#include <qpe/qmath.h>

/*  PROJILLUM  --  Project illuminated area on the map.  */

void
projillum(wtab, xdots, ydots, dec)
short *wtab;
int xdots, ydots;
double dec;
{
	int i, ftf = 1, ilon, ilat, lilon = 0, lilat = 0, xt;
	double m, x, y, z, th, lon, lat, s, c;

	/* Clear unoccupied cells in width table */

	for (i = 0; i < ydots; i++)
		wtab[i] = -1;

	/* Build transformation for declination */

	s = qSin(-dtr(dec));
	c = qCos(-dtr(dec));

	/* Increment over a semicircle of illumination */

	for (th = -(PI / 2); th <= PI / 2 + 0.001;
	    th += PI / TERMINC) {

		/* Transform the point through the declination rotation. */

		x = -s * qSin(th);
		y = qCos(th);
		z = c * qSin(th);

		/* Transform the resulting co-ordinate through the
		   map projection to obtain screen co-ordinates. */

		lon = (y == 0 && x == 0) ? 0.0 : rtd(qATan2(y, x));
		lat = rtd(qASin(z));

		ilat = ydots - (lat + 90) * (ydots / 180.0);
		ilon = lon * (xdots / 360.0);

		if (ftf) {

			/* First time.  Just save start co-ordinate. */

			lilon = ilon;
			lilat = ilat;
			ftf = 0;
		} else {

			/* Trace out the line and set the width table. */

			if (lilat == ilat) {
				wtab[(ydots - 1) - ilat] = ilon == 0 ? 1 : ilon;
			} else {
				m = ((double) (ilon - lilon)) / (ilat - lilat);
				for (i = lilat; i != ilat; i += sgn(ilat - lilat)) {
					xt = lilon + qFloor((m * (i - lilat)) + 0.5);
					wtab[(ydots - 1) - i] = xt == 0 ? 1 : xt;
				}
			}
			lilon = ilon;
			lilat = ilat;
		}
	}

	/* Now tweak the widths to generate full illumination for
	   the correct pole. */

	if (dec < 0.0) {
		ilat = ydots - 1;
		lilat = -1;
	} else {
		ilat = 0;
		lilat = 1;
	}

	for (i = ilat; i != ydots / 2; i += lilat) {
		if (wtab[i] != -1) {
			while (1) {
				wtab[i] = xdots / 2;
				if (i == ilat)
					break;
				i -= lilat;
			}
			break;
		}
	}
}

/*
 * Sun clock - astronomical routines.
 */

/*  JDATE  --  Convert internal GMT date and time to Julian day
	       and fraction.  */

long
jdate(t)
struct tm *t;
{
	long c, m, y;

	y = t->tm_year + 1900;
	m = t->tm_mon + 1;
	if (m > 2)
	   m = m - 3;
	else {
	   m = m + 9;
	   y--;
	}
	c = y / 100L;		   /* Compute century */
	y -= 100L * c;
	return t->tm_mday + (c * 146097L) / 4 + (y * 1461L) / 4 +
	    (m * 153L + 2) / 5 + 1721119L;
}

/* JTIME --    Convert internal GMT  date  and	time  to  astronomical
	       Julian  time  (i.e.   Julian  date  plus  day fraction,
	       expressed as a double).	*/

double
jtime(t)
struct tm *t;
{
	return (jdate(t) - 0.5) + 
	   (((long) t->tm_sec) +
	     60L * (t->tm_min + 60L * t->tm_hour)) / 86400.0;
}

/*  KEPLER  --	Solve the equation of Kepler.  */

double
kepler(m, ecc)
double m, ecc;
{
	double e, delta;
#define EPSILON 1E-6

	e = m = dtr(m);
	do {
	   delta = e - ecc * qSin(e) - m;
	   e -= delta / (1 - ecc * qCos(e));
	} while (qFabs(delta) > EPSILON);
	return e;
}

/*  SUNPOS  --	Calculate position of the Sun.	JD is the Julian  date
		of  the  instant for which the position is desired and
		APPARENT should be nonzero if  the  apparent  position
		(corrected  for  nutation  and aberration) is desired.
                The Sun's co-ordinates are returned  in  RA  and  DEC,
		both  specified  in degrees (divide RA by 15 to obtain
		hours).  The radius vector to the Sun in  astronomical
                units  is returned in RV and the Sun's longitude (true
		or apparent, as desired) is  returned  as  degrees  in
		SLONG.	*/


void
sunpos(jd, apparent, ra, dec, rv, slong)
double jd;
int apparent;
double *ra, *dec, *rv, *slong;
{
	double t, t2, t3, l, m, e, ea, v, theta, omega,
	       eps;

	/* Time, in Julian centuries of 36525 ephemeris days,
	   measured from the epoch 1900 January 0.5 ET. */

	t = (jd - 2415020.0) / 36525.0;
	t2 = t * t;
	t3 = t2 * t;

	/* Geometric mean longitude of the Sun, referred to the
	   mean equinox of the date. */

	l = fixangle(279.69668 + 36000.76892 * t + 0.0003025 * t2);

        /* Sun's mean anomaly. */

	m = fixangle(358.47583 + 35999.04975*t - 0.000150*t2 - 0.0000033*t3);

        /* Eccentricity of the Earth's orbit. */

	e = 0.01675104 - 0.0000418 * t - 0.000000126 * t2;

	/* Eccentric anomaly. */

	ea = kepler(m, e);

	/* True anomaly */

	v = fixangle(2 * rtd(qATan(qSqrt((1 + e) / (1 - e))  * qTan(ea / 2))));

        /* Sun's true longitude. */

	theta = l + v - m;

	/* Obliquity of the ecliptic. */

	eps = 23.452294 - 0.0130125 * t - 0.00000164 * t2 + 0.000000503 * t3;

        /* Corrections for Sun's apparent longitude, if desired. */

	if (apparent) {
	   omega = fixangle(259.18 - 1934.142 * t);
	   theta = theta - 0.00569 - 0.00479 * qSin(dtr(omega));
	   eps += 0.00256 * qCos(dtr(omega));
	}

        /* Return Sun's longitude and radius vector */

	*slong = theta;
	*rv = (1.0000002 * (1 - e * e)) / (1 + e * qCos(dtr(v)));

	/* Determine solar co-ordinates. */

	*ra =
	fixangle(rtd(qATan2(qCos(dtr(eps)) * qSin(dtr(theta)), qCos(dtr(theta)))));
	*dec = rtd(qASin(sin(dtr(eps)) * qSin(dtr(theta))));
}

/*  GMST  --  Calculate Greenwich Mean Siderial Time for a given
	      instant expressed as a Julian date and fraction.	*/

double
gmst(jd)
double jd;
{
	double t, theta0;


	/* Time, in Julian centuries of 36525 ephemeris days,
	   measured from the epoch 1900 January 0.5 ET. */

	t = ((qFloor(jd + 0.5) - 0.5) - 2415020.0) / 36525.0;

	theta0 = 6.6460656 + 2400.051262 * t + 0.00002581 * t * t;

	t = (jd + 0.5) - (qFloor(jd + 0.5));

	theta0 += (t * 24.0) * 1.002737908;

	theta0 = (theta0 - 24.0 * (qFloor(theta0 / 24.0)));

	return theta0;
}
