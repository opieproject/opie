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

#include "cbinfo.h"
#include "traninfo.h"

#include <qpe/config.h>

#include <qfile.h>

CBInfo::CBInfo()
{
	n = "";
	fn = "";
	pw = QString::null;
	t = "";
	bn = "";
	a = "";
	p = "";
	nt = "";
	sb = 0.0;

	tl = new TranInfoList();
}

CBInfo::CBInfo( const QString &name, const QString &filename )
{
	Config config( filename, Config::File );
	config.setGroup( "Account" );

	n = name;
	fn = filename;
	pw = config.readEntryCrypt( "Password", QString::null );

	t = config.readEntry( "Type" );
	bn = config.readEntry( "Bank", "" );
	a = config.readEntryCrypt( "Number", "" );
	p = config.readEntryCrypt( "PINNumber", "" );
	nt = config.readEntry( "Notes", "" );

	bool ok;
	sb = config.readEntry( "Balance", "0.0" ).toFloat( &ok );

	loadTransactions();
}

float CBInfo::balance()
{
	calcBalance();
	return b;
}

void CBInfo::write()
{
	QFile f( fn );
	if ( f.exists() )
	{
		f.remove();
	}

	Config *config = new Config(fn, Config::File);

	// Save info
	config->setGroup( "Account" );
	config->writeEntryCrypt( "Password", pw );
	config->writeEntry( "Type", t );
	config->writeEntry( "Bank", bn );
	config->writeEntryCrypt( "Number", a );
	config->writeEntryCrypt( "PINNumber", p );
	config->writeEntry( "Notes", nt );
	QString balstr;
	balstr.setNum( sb, 'f', 2 );
	config->writeEntry( "Balance", balstr );

	// Save transactions
	int i = 1;
	for ( TranInfo *tran = tl->first(); tran; tran = tl->next() )
	{
		tran->write( config, i );
		i++;
	}
	config->write();

	delete config;
}

TranInfo *CBInfo::findTransaction( const QString &checknum, const QString &date,
									  const QString &desc )
{
	TranInfo *traninfo = tl->first();
	while ( traninfo )
	{
		if ( traninfo->number() == checknum && traninfo->datestr() == date &&
			 traninfo->desc() == desc )
			break;
		traninfo = tl->next();
	}
	return( traninfo );
}

void CBInfo::addTransaction( TranInfo *tran )
{
	tl->inSort( tran );
	calcBalance();
}

void CBInfo::removeTransaction( TranInfo *tran )
{
	tl->remove( tran );
	delete tran;
	calcBalance();
}

void CBInfo::loadTransactions()
{
	TranInfo *tran;
	QString trandesc = "";

	tl = new TranInfoList();

	Config config( fn, Config::File );

	for ( int i = 1; trandesc != QString::null; i++ )
	{
		tran = new TranInfo( config, i );
		trandesc = tran->desc();
		if ( trandesc != QString::null )
		{
			tl->inSort( tran );
		}
		else
		{
			delete tran;
		}
	}

	calcBalance();
}

void CBInfo::calcBalance()
{
	float amount;

	b = sb;

	for ( TranInfo *tran = tl->first(); tran; tran = tl->next() )
	{
		b -= tran->fee();
		amount = tran->amount();
		if ( tran->withdrawal() )
		{
			amount *= -1;
		}
		b += amount;
	}
}

int CBInfoList::compareItems( QCollection::Item item1, QCollection::Item item2 )
{
	QString n1 = ((CBInfo *)item1)->name();
	QString n2 = ((CBInfo *)item2)->name();
	int r = -1;

	if ( n1 < n2 )
		r = -1;
	else if ( n1 == n2 )
		r = 0;
	else if ( n1 > n2 )
		r = 1;
	return( r );
}
