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

#include "traninfo.h"

#include <qpe/config.h>

QString tempstr;

TranInfo::TranInfo( int id, const QString &desc, const QDate &date, bool withdrawal,
					const QString &type, const QString &category, float amount,
					float fee, const QString &number, const QString &notes )
{
	i = id;
	d = desc;
	td = date;
	w = withdrawal;
	t = type;
	c = category;
	a = amount;
	f = fee;
	cn = number;
	n = notes;
}

TranInfo::TranInfo( Config config, int entry )
{
	config.setGroup( QString::number( entry ) );
	QString desc = config.readEntry( "Description", "Not Found" );
	if ( desc != "Not Found" )
	{
		// ID
		i = entry;

		// Description
		d = desc;

		// Transaction date
		int yr, mn, dy;
		QString datestr = config.readEntry( "Date", "" );
		int begin, end;
		begin = datestr.find( '/' );
		mn = datestr.left( begin ).toInt();
		end = datestr.find( '/', ++begin );
		dy = datestr.mid( begin, end - begin ).toInt();
		yr = datestr.right( datestr.length() - end - 1).toInt();
		td.setYMD( yr, mn, dy );

		// Deposit/withdrawal indicator ( withdrawal == TRUE )
		w = ( config.readEntry( "Payment", "false" ) == "true" );

		// Type
		QString type = config.readEntry( "Type", "0" );
		if ( w )
		{   // Withdrawal types
			if( type == "0" )
				t = "Debit Charge";
			else if( type == "1" )
				t = "Written Check";
			else if( type == "2" )
				t = "Transfer";
			else if( type == "3" )
				t = "Credit Card";
		}
		else
		{
			if( type == "0" )
				t = "Written Check";
			else if( type == "1" )
				t = "Automatic Payment";
			else if( type == "2" )
				t = "Transfer";
			else if( type == "3" )
				t = "Cash";
		}

		// Category
		c = config.readEntry( "Category", "" );

		// Transaction amount
		QString stramount = config.readEntry( "Amount", "0.00" );
		bool ok;
		a = stramount.toFloat( &ok );

		// Transaction fee
		stramount = config.readEntry( "TransactionFee", "0.00" );
		f = stramount.toFloat( &ok );

		// Transaction number
		cn = config.readEntry( "CheckNumber", "" );

		// Notes
		n = config.readEntry( "Comments", "" );
	}
}

const QString &TranInfo::datestr()
{
	tempstr = QString::number( td.year() );
	tempstr.append( '/' );
	int tempfield = td.month();
	if ( tempfield < 10 ) tempstr.append( '0' );
	tempstr.append( QString::number( tempfield ) );
	tempstr.append( '/' );
	tempfield = td.day();
	if ( tempfield < 10 ) tempstr.append( '0' );
	tempstr.append( QString::number( tempfield ) );

	return( tempstr );
}

void TranInfo::write( Config *config, int entry )
{
	config->setGroup( QString::number( entry ) );

	config->writeEntry( "Description", d );

	tempstr = QString::number( td.month() );
	tempstr.append( '/' );
	tempstr.append( QString::number( td.day() ) );
	tempstr.append( '/' );
	tempstr.append(  QString::number( td.year() ) );
	config->writeEntry( "Date", tempstr );

	w ? tempstr = "true"
	  : tempstr = "false";
	config->writeEntry( "Payment", tempstr );

	if ( t == "Debit Charge" || t == "Written Check" )
		tempstr = "0";
	else if ( t == "Written Check" || t == "Automatic Payment" )
		tempstr = "1";
	else if ( t == "Transfer" )
		tempstr = "2";
	else if ( t == "Credit Card" || t == "Cash" )
		tempstr = "3";
	config->writeEntry( "Type", tempstr );

	config->writeEntry( "Category", c );

	tempstr.setNum( a, 'f', 2 );
	config->writeEntry( "Amount", tempstr );

	tempstr.setNum( f, 'f', 2 );
	config->writeEntry( "TransactionFee", tempstr );

	config->writeEntry( "CheckNumber", cn );

	config->writeEntry( "Comments", n );
}

int TranInfoList::compareItems( QCollection::Item item1, QCollection::Item item2 )
{
	QDate d1 = ((TranInfo *)item1)->date();
	QDate d2 = ((TranInfo *)item2)->date();
	int r = -1;

	if ( d1 < d2 )
		r = -1;
	else if ( d1 == d2 )
		r = 0;
	else if ( d1 > d2 )
		r = 1;
	return( r );
}
