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
#include <stdio.h>

#include <qfile.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qtextstream.h>

#include <qpe/config.h>
#include <qpe/resource.h>

#include "weatherpluginwidget.h"

WeatherPluginWidget::WeatherPluginWidget( QWidget *parent,  const char* name )
	: QWidget( parent,  name )
{
	Config config( "todayweatherplugin");
	config.setGroup( "Config" );

	location = config.readEntry( "Location", "" );
	useMetric = config.readBoolEntry( "Metric", TRUE );
	frequency = config.readNumEntry( "Frequency", 5 );

	localFile = "/tmp/";
	localFile.append( location );
	localFile.append( ".TXT" );

	remoteFile = "http://weather.noaa.gov/pub/data/observations/metar/stations/";
	remoteFile.append( location );
	remoteFile.append( ".TXT" );

	QHBoxLayout *layout = new QHBoxLayout( this );
	layout->setAutoAdd( TRUE );
	layout->setSpacing( 2 );

	weatherIcon = new QLabel( this );
	weatherIcon->setMaximumWidth( 32 );
	QImage logo1 = Resource::loadImage( "todayweatherplugin/wait" );
	QPixmap pic;
	pic.convertFromImage( logo1 );
	weatherIcon->setPixmap( pic );

	weatherLabel = new QLabel( tr( "Retreiving current weather information." ), this );
	weatherLabel->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred ) );

	startTimer(1000);
	//retreiveData();
}

WeatherPluginWidget::~WeatherPluginWidget()
{
	QFile file( localFile );
	if ( file.exists() )
	{
		file.remove();
	}
}

void WeatherPluginWidget::timerEvent( QTimerEvent *e )
{
	killTimer( e->timerId() );
	retreiveData();
}



void WeatherPluginWidget::retreiveData()
{
	startTimer( frequency * 60000 );

	QFile file( localFile );
	if ( file.exists() )
	{
		file.remove();
	}

	QString command = "wget -q ";
	command.append( remoteFile );
	command.append( " -O " );
	command.append( localFile );
	FILE *get = popen( command.latin1(), "r" );
	if ( get )
	{
		pclose( get );
		displayWeather();
	}
	else
	{
		weatherLabel->setText( tr( "Current weather data not available.\nTry looking out the window." ) );
	}
}

void WeatherPluginWidget::displayWeather()
{
	weatherData = QString::null;

	QFile file( localFile );
	if ( file.open( IO_ReadOnly ) )
	{
		QTextStream data( &file );
		while ( !data.eof() )
		{
			weatherData.append( data.readLine() );
		}
		file.close();
		weatherData = weatherData.simplifyWhiteSpace();

		QString tmpstr;

		tmpstr.append( tr( "Temp: " ) );
		getTemp( weatherData );
		tmpstr.append( dataStr );

		tmpstr.append( tr( "  Wind: " ) );
		getWind( weatherData );
		tmpstr.append( dataStr );

		tmpstr.append( tr( "\nPres: " ) );
		getPressure( weatherData );
		tmpstr.append( dataStr );

		weatherLabel->setText( tmpstr );

		tmpstr = "todayweatherplugin/";
		getIcon( weatherData );
		tmpstr.append( dataStr );
		QImage logo1 = Resource::loadImage( tmpstr );
		QPixmap pic;
		pic.convertFromImage( logo1 );
		weatherIcon->setPixmap( pic );
	}
	else
	{
		weatherLabel->setText( tr( "Current weather data not available.\nTry looking out the window." ) );
	}
}

void WeatherPluginWidget::getTemp( const QString &data )
{
	int value;
	bool ok;

	int pos = data.find( QRegExp( "M?[0-9]+/M?[0-9]+" ), 20 );
	if ( pos > -1 )
	{
		if ( data.at( pos ) == 'M' )
		{
			value = -1 * data.mid( pos + 1, 2 ).toInt( &ok );
		}
		else
		{
			value = data.mid( pos, 2 ).toInt( &ok );
		}
		if ( useMetric )
		{
			dataStr = QString::number( value );
			dataStr.append( 'C' );
		}
		else
		{
			dataStr = QString::number( ( value * 9 / 5 ) + 32 );
			dataStr.append( 'F' );
		}
	}
	else
	{
		dataStr = tr( "n/a" );
	}
}

void WeatherPluginWidget::getWind( const QString &data )
{
	int value;
	bool ok;

	int pos = data.find( QRegExp( "[0-9]*G*[0-9]*KT" ), 20 );
	if ( pos > -1 )
	{
		if ( data.mid( pos, 3 ) != "VRB" )
		{
			value = data.mid( pos, 3 ).toInt( &ok );
			if ( ( value >= 0 && value < 23 ) || ( value >= 239 && value <= 360 ) )
				dataStr = tr("E " );
			else if ( value >= 23 && value < 69 )
				dataStr = tr( "NE " );
			else if ( value >= 69 && value < 113 )
				dataStr = tr( "N " );
			else if ( value >= 113 && value < 157 )
				dataStr = tr( "NW " );
			else if ( value >= 157 && value < 203 )
				dataStr = tr( "W " );
			else if ( value >= 203 && value < 248 )
				dataStr = tr( "SW " );
			else if ( value >= 248 && value < 294 )
				dataStr = tr( "S " );
			else if ( value >= 294 && value < 238 )
				dataStr = tr( "SE " );
		}
		if ( data.mid( pos + 5, 1) == "G" ||
			 data.mid( pos + 5, 1) == "K" )
		{
			value = data.mid( pos + 3, 2 ).toInt( &ok );
		}
		else
		{
			value = data.mid( pos + 3, 3 ).toInt( &ok );
		}
		if ( useMetric )
		{
			value = value * 3.6 / 1.94;
			dataStr.append( QString::number( value ) );
			dataStr.append( tr( " KPH" ) );
		}
		else
		{
			value = value * 2.24 / 1.94;
			dataStr.append( QString::number( value ) );
			dataStr.append( tr( " MPH" ) );
		}
	}
	else
	{
		dataStr = tr( "n/a" );
	}
}

void WeatherPluginWidget::getPressure( const QString &data )
{
	float value;
	bool ok;

	int pos = data.find( QRegExp( "[AQ][0-9]+" ), 20 );
	if ( pos > -1 )
	{
		value = data.mid( pos + 1, 4 ).toFloat( &ok );
		if ( useMetric )
		{
			if ( data.mid( pos, 1 ) == "A" )
				value *= 33.8639 / 100;
			dataStr = QString::number( value, 'f', 2 );
			dataStr.append( tr( " hPa" ) );
		}
		else
		{
			if ( data.mid( pos, 1 ) == "Q" )
				value /= 33.8639;
			else
				value /= 100;
			dataStr = QString::number( value, 'f', 2 );
			dataStr.append( tr( " Hg" ) );
		}
	}
	else
	{
		dataStr = tr( "n/a" );
	}
}

void WeatherPluginWidget::getIcon(const QString &data )
{
	dataStr = "psunny";
	if ( data.find( "CLR ", 20  ) > -1 ||
		 data.find( "SKC ", 20  ) > -1 ||
		 data.find( "CAVOK ", 20  ) > -1 )
	{
		dataStr = "sunny";
	}
	else if ( data.find( "SH ", 20 ) > -1 ||
			  data.find( "DZ ", 20 ) > -1 ||
			  data.find( "RA ", 20 ) > -1 ||
			  data.find( "UP ", 20 ) > -1 ||
			  data.find( "BR ", 20 ) > -1 )
	{
		dataStr = "shower";
	}
	else if ( data.find( "TS ", 20 ) > -1 )
	{
		dataStr = "tstorm";
	}
	else if ( data.find( "SN ", 20 ) > -1 ||
			  data.find( "SG ", 20 ) > -1 )
	{
		dataStr = "snow";
	}
	else if ( data.find( "FZ ", 20 ) > -1 ||
			  data.find( "GR ", 20 ) > -1 ||
			  data.find( "GS ", 20 ) > -1 ||
			  data.find( "PE ", 20 ) > -1 ||
			  data.find( "IC ", 20 ) > -1 )
	{
		dataStr = "sleet";
	}
}
