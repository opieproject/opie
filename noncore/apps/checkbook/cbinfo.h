/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
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

#ifndef CBINFO_H
#define CBINFO_H

#include <qlist.h>
#include <qstring.h>

class Config;
class TranInfo;
class TranInfoList;

class CBInfo
{
	public:
		CBInfo();
		CBInfo( const QString &, const QString & );

		const QString &name()				const { return n; }
		const QString &filename()			const { return fn; }
		const QString &password()			const { return pw; }
		const QString &type()				const { return t; }
		const QString &bank()				const { return bn; }
		const QString &account()			const { return a; }
		const QString &pin()				const { return p; }
		const QString &notes()				const { return nt; }
		float           startingBalance()	const { return sb; }
		float           balance();

		void setName( const QString &name )			{ n = name; }
		void setFilename( const QString &filename )	{ fn = filename; }
		void setPassword( const QString &password )	{ pw = password; }
		void setType( const QString &type )			{ t = type; }
		void setBank( const QString &bank )			{ bn = bank; }
		void setAccount( const QString &account )		{ a = account; }
		void setPin( const QString &pin )				{ p = pin; }
		void setNotes( const QString &notes )			{ nt = notes; }
		void setStartingBalance( float startbal )		{ sb = startbal; }

		void     write();

		TranInfoList *transactions()		const { return tl; }
		TranInfo     *findTransaction( const QString &, const QString &, const QString & );
		void           addTransaction( TranInfo * );
		void           removeTransaction( TranInfo * );

	private:
		QString n;
		QString fn;
		QString pw;
		QString t;
		QString bn;
		QString a;
		QString p;
		QString nt;
		float   sb;
		float   b;

		TranInfoList *tl;

		void loadTransactions();
		void calcBalance();
};

class CBInfoList : public QList<CBInfo>
{
	protected:
		int compareItems( QCollection::Item, QCollection::Item );
};

#endif
