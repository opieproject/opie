/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 OPIE team <opie@handhelds.org?>
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

#include "predicttabwidget.h"

#include <qpe/config.h>
#include <qpe/timeconversion.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qtable.h>

#include <stdlib.h>

PredictTabWidget::PredictTabWidget( QWidget *parent )
	: QWidget( parent, 0x0, 0 )
{
/*
	QVBoxLayout *tmpvb = new QVBoxLayout( this );
	QScrollView *sv = new QScrollView( this );
	tmpvb->addWidget( sv, 0, 0 );
	sv->setResizePolicy( QScrollView::AutoOneFit );
	sv->setFrameStyle( QFrame::NoFrame );
	QWidget *container = new QWidget( sv->viewport() );
	sv->addChild( container );
*/

	QGridLayout *layout = new QGridLayout( this );
	layout->setMargin( 2 );
	layout->setSpacing( 4 );

	// Predicted time drift
	layout->addWidget( new QLabel( tr( "Predicted time drift" ), this ), 0, 0 );
	lblDrift = new QLabel( tr( "n/a" ), this );
	layout->addWidget( lblDrift, 0, 1 );

	// Estimated time difference
	layout->addWidget( new QLabel( tr( "Estimated shift" ), this ), 1, 0 );
	lblDiff = new QLabel( tr( "n/a" ), this );
	layout->addWidget( lblDiff, 1, 1 );

	// Predicted time
	layout->addWidget( new QLabel( tr( "Predicted time" ), this ), 2, 0 );
	lblPredicted = new QLabel( tr( "n/a" ), this );
	layout->addWidget( lblPredicted, 2, 1 );

	// Prediction table
	tblLookups = new QTable( 2, 3, this );
	QFont font(  tblLookups->font() );
	font.setPointSize( 7 );
	tblLookups->setFont( font );
	tblLookups->horizontalHeader()->setLabel( 0, tr( "Shift [s/h]" ) );
	tblLookups->horizontalHeader()->setLabel( 1, tr( "Last [h]" ) );
	tblLookups->horizontalHeader()->setLabel( 2, tr( "Offset [s]" ) );
	tblLookups->setColumnWidth( 0, 78 );
	tblLookups->setColumnWidth( 1, 50 );
	tblLookups->setColumnWidth( 2, 50 );
	tblLookups->setMinimumHeight( 50 );
	tblLookups->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum ) );
	layout->addMultiCellWidget( tblLookups, 3, 3, 0, 1 );

	// Predict time button
	QPushButton *pb = new QPushButton( tr( "Predict time" ), this );
	connect( pb, SIGNAL(clicked()), this, SLOT(slotPredictTime()) );
	layout->addWidget( pb, 4, 0 );

	// Set predicted time button
	pb = new QPushButton( tr( "Set predicted time" ), this );
	connect( pb, SIGNAL(clicked()), this, SLOT(slotSetPredictedTime()) );
	layout->addWidget( pb, 4, 1 );

	// Initialize values
	Config config( "ntp" );
	config.setGroup( "lookups" );
	int lookupCount = config.readNumEntry( "count", 0 );
	float last, shift, shiftPerSec;
	tblLookups->setNumRows( lookupCount );
	int cw = tblLookups->width() / 4;
	cw = 50;
	tblLookups->sortColumn( 0, FALSE, TRUE );
	_shiftPerSec = 0;
	QString grpname;
	for ( int i=0; i < lookupCount; i++ )
	{
		grpname = "lookup_";
		grpname.append( QString::number( i ) );
		config.setGroup( grpname );
		last = config.readEntry( "secsSinceLast", 0 ).toFloat();
		shift = QString( config.readEntry( "timeShift", 0 ) ).toFloat();
		shiftPerSec =  shift / last;
		_shiftPerSec += shiftPerSec;
		tblLookups->setText( i, 0, QString::number( shiftPerSec * 60 * 60 ) );
		tblLookups->setText( i, 2, QString::number( shift ) );
		tblLookups->setText( i, 1, QString::number( last / ( 60 * 60 ) ) );
	}
	_shiftPerSec /= lookupCount;
	QString drift = QString::number( _shiftPerSec * 60 * 60);
	drift.append( tr( " s/h" ) );
	lblDrift->setText( drift );

	Config lconfig( "locale" );
	lconfig.setGroup( "Location" );
	tz = lconfig.readEntry( "Timezone", "America/New_York" );
}

PredictTabWidget::~PredictTabWidget()
{
}

void PredictTabWidget::setShiftPerSec( int i )
{
	_shiftPerSec += i;
}

void PredictTabWidget::slotTZChanged( const QString &newtz )
{
	tz = newtz;
}

void PredictTabWidget::slotPredictTime()
{
	Config config( "ntp" );
	config.setGroup( "lookups" );
	int lastTime = config.readNumEntry( "time", 0 );
	config.writeEntry( "lastNtp", TRUE );
	setenv( "TZ", tz, 1 );
	int now = TimeConversion::toUTC( QDateTime::currentDateTime() );
	int corr = int( ( now - lastTime ) * _shiftPerSec );
	QString diff = QString::number( corr );
	diff.append( tr( " seconds" ) );
	lblDiff->setText( diff );
	predictedTime = QDateTime::currentDateTime().addSecs( corr );
	lblPredicted->setText( predictedTime.toString() );
}

void PredictTabWidget::slotSetPredictedTime()
{
	slotPredictTime();
	emit setTime( predictedTime );
}
