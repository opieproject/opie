/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef TRANINFO_H
#define TRANINFO_H

#include <qdatetime.h>
#include <qlist.h>
#include <qstring.h>

class Config;

class TranInfo
{
	public:
		TranInfo( int = 0, const QString & = 0x0, const QDate & = QDate::currentDate(),
				  bool = TRUE, const QString & = 0x0, const QString & = 0x0,
				  float = 0.0, float = 0.0,
				  const QString & = 0x0, const QString & = 0x0 );
		TranInfo( Config, int );

		int            id()			const { return i; }
		const QString &desc()		const { return d; }
		const QDate   &date()		const { return td; }
		const QString &datestr();
		bool           withdrawal()	const { return w; }
		const QString &type()		const { return t; }
		const QString &category()	const { return c; }
		float          amount()		const { return a; }
		float          fee()		const { return f; }
		const QString &number()		const { return cn; }
		const QString &notes()		const { return n; }

		void setDesc( const QString &desc )		{ d = desc; }
		void setDate( const QDate &date )		{ td = date; }
		void setWithdrawal( bool withdrawal )	{ w = withdrawal; }
		void setType( const QString &type )		{ t = type; }
		void setCategory( const QString &cat )	{ c = cat; }
		void setAmount( float amount )			{ a = amount; }
		void setFee( float fee )				{ f = fee; }
		void setNumber( const QString &num )	{ cn = num; }
		void setNotes( const QString &notes )	{ n = notes; }

		void write( Config *, int );

	private:
		int     i;
		QString d;
		QDate   td;
		bool    w;
		QString t;
		QString c;
		float   a;
		float   f;
		QString cn;
		QString n;
};

typedef QList<TranInfo> TranInfoList;

#endif
