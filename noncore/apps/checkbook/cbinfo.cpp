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

// --- CBInfo -----------------------------------------------------------------
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
    _sLastTab="";
    _first=-1;
    _last=-1;

	tl = new TranInfoList();
}


// --- CBInfo -----------------------------------------------------------------
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
    _sLastTab = config.readEntry("LastTab", "");
    _first=config.readNumEntry("First", -1);
    _sSortOrder = config.readEntry( "SortOrder", QWidget::tr("Date") );

	bool ok;
	sb = config.readEntry( "Balance", "0.0" ).toFloat( &ok );

	loadTransactions();
}

// --- balance ----------------------------------------------------------------
float CBInfo::balance()
{
	calcBalance();
	return b;
}

// --- write ------------------------------------------------------------------
void CBInfo::write()
{
	QFile f( fn );
	if ( f.exists() )
		f.remove();

	Config *config = new Config(fn, Config::File);


    // fix transaction numbers
    _first=-1;
    TranInfo *prev=NULL;
    for ( TranInfo *tran = tl->first(); tran; tran = tl->next() ) {
        if( _first<0 ) _first=tran->id();
        if( prev ) prev->setNext( tran->id() );
        tran->setNext(-1);
        prev=tran;
    }

	// Save transactions
    for ( TranInfo *tran = tl->first(); tran; tran = tl->next() ) {
        tran->write(config);
    }

    // Save info
    if( _first<0 && _last>=0 ) _first=_last;
	config->setGroup( "Account" );
	config->writeEntryCrypt( "Password", pw );
	config->writeEntry( "Type", t );
	config->writeEntry( "Bank", bn );
	config->writeEntryCrypt( "Number", a );
	config->writeEntryCrypt( "PINNumber", p );
	config->writeEntry( "Notes", nt );
    config->writeEntry( "LastTab", _sLastTab );
	QString balstr;
	balstr.setNum( sb, 'f', 2 );
	config->writeEntry( "Balance", balstr );
    config->writeEntry( "First", _first );
    config->writeEntry( "SortOrder", _sSortOrder );

    config->write();
	delete config;
}


// --- findTransaction --------------------------------------------------------
TranInfo *CBInfo::findTransaction( const QString &sId )
{
    bool bOk;
    int id=sId.toInt( &bOk );
    if( !bOk )
        return(false);
    TranInfo *traninfo;
	for(traninfo=tl->first(); traninfo; traninfo=tl->next()) {
        if( traninfo->id() == id )
            break;
    }
    return(traninfo);
}

void CBInfo::addTransaction( TranInfo *tran )
{
    tl->append( tran );
	calcBalance();
}

void CBInfo::removeTransaction( TranInfo *tran )
{
    tl->removeRef( tran );
    delete tran;
	calcBalance();
}


// --- loadTransactions -------------------------------------------------------
// Reads the transactions. Either the old way 1-n or as linked list.
void CBInfo::loadTransactions()
{
	TranInfo *tran;
	QString trandesc = "";

	tl = new TranInfoList();

	Config config( fn, Config::File );
    int i=_first;
    bool bOld=false;
    if( i==-1 ) {
        i=1;
        bOld=true;
    }
    while( i>=0 ) {
        _last=i;
        tran=new TranInfo(config, i);
        trandesc = tran->desc();
        if( trandesc==QString::null ) {
            delete tran;
            break;
        }
        tl->append(tran);
        i= bOld ? i+1 : tran->getNext();
    }

    calcBalance();
}


// --- calcBalance ------------------------------------------------------------
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
