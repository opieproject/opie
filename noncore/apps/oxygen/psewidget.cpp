/***************************************************************************
   application:             : Oxygen

   begin                    : September 2002
   copyright                : ( C ) 2002 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qpe/config.h>
#include <qlayout.h>
#include <qpe/qpeapplication.h>
#include <qlist.h>
#include "dataTable.h"
#include "psewidget.h"
#include "oxyframe.h"


PSEWidget::PSEWidget() : QWidget()
{
    this->setCaption( tr( "Periodic System" ) );
	
	lastElement=1;

    QVBoxLayout *vlay = new QVBoxLayout( this );
    
    QGridLayout *grid = new QGridLayout( 18,10 );
    int h=0, v=0;
    
    Config configobj( QPEApplication::qpeDir() +"share/oxygen/oxygendata", Config::File );
    for( int n = 0 ; n < 109 ; n++ )
    {
        configobj.setGroup( QString::number( n+1 ));
        
        position( n+1,h,v );
        PSEframe.append( new OxyFrame( this , QString::number(n), configobj.readEntry( "Symbol" ) ) );
        grid->addWidget( PSEframe.current() , v/40+1 , h/40 );
        PSEframe.current()->setMinimumHeight( 11 );
        PSEframe.current()->setPalette(  QPalette(  PSEColor( configobj.readEntry( "Block" )  ) ) );
        connect( PSEframe.current(), SIGNAL( num(QString) ), this, SLOT( slotShowElement(QString) ));
        connect( PSEframe.current(), SIGNAL( num(QString) ), this, SLOT( inverseColor(QString) ));
    }
    
    oxyDW = new OxydataWidget(this);
    oxyDW->setElement( 0 );
    oxyDW->setLayout();

    vlay->addLayout( grid );
    vlay->addWidget( oxyDW );
}

QColor PSEWidget::PSEColor( QString block )
{
    QColor c;
    if ( block == "s" ) c.setRgb( 213 , 233 , 231 );
    else if ( block == "d" ) c.setRgb( 200,230,160 );
    else if ( block == "p" ) c.setRgb( 238,146,138 );
    else if ( block == "f" ) c.setRgb( 190 , 190 , 190 );
    return  c;
};

void PSEWidget::inverseColor( QString number)
{
    Config configobj( QPEApplication::qpeDir() +"share/oxygen/oxygendata", Config::File );
    configobj.setGroup(  number );
	QString block = configobj.readEntry( "Block" );
    QColor c, d;
	c = PSEColor( block );
	d = c.dark();
	
	PSEframe.at( number.toUInt() )->setPalette(  QPalette( d ) );

	configobj.setGroup( QString::number( lastElement+1 ) );
	block = configobj.readEntry( "Block" );
	c = PSEColor( block );
	PSEframe.at( lastElement )->setPalette(  QPalette( c ) );

	lastElement=number.toInt();
}

void PSEWidget::slotShowElement(QString number)
{ 
    oxyDW->setElement( number.toInt() );
};

void PSEWidget::position(int n, int& h, int& v)
{
    //Hydrogen
    if (n == 1)
    {
	h=0; v=0;
    }


    //Helium
    if (n == 2)
    {
	h=680; v=0;
    }


    //Lithium
    if (n == 3)
    {
	h=0; v=40;
    }


    //Beryllium
    if (n == 4)
    {
	h=40; v=40;
    }


    //Boron-->Neon or Aluminum --> Argon
    if ((n >= 5 && n <= 10) || (n >= 13 && n <= 18))
	for (int i = 1; i <= (6-(10-n)); i++)
	{
	    h=((i*40)+440);
	    v = 40;
	    if (n >= 13)
	    {
	    	v=80;
    		h=(h-320);
	    }
	}


    //Sodium
    if (n == 11)
    {
	h=0; v=80;
    }


    //Magnesium
    if (n == 12)
    {
	h=40; v=80;
    }


    //Potassium --> Uuo without La and Ac
    if ((n >= 19 && n <= 56) || (n >= 72 && n <= 88) || n >= 104)
	for (int i = 1; i <= 18; i++)
	{
	    int f = n;
	    if (n > 18)
		    f = n-18;
	    if (n > 36)
		    f = n-36;
	    if (n > 54)
		    f = n-54;
	    if (n > 71)
		    f = n-68;
	    if (n > 86)
		    f = n-86;
	    if (n > 103)
		    f = n-100;

	    h=((f*40)-40);

	    v = 120;
	    if (n >= 37)
		    v=160;
	    if (n >= 55)
		    v=200;
	    if (n >= 87)
		    v=240;
	}


    //Lanthanum --> Lutetium and Actinum --> Lawrencium
    if ((n >= 57 && n <= 71) || (n >= 89 && n <= 103))
	for (int i = 1; i <= 14; i++)
	{
	    int f = n;
	    if (n > 56)
		    f = n-55;
	    if (n > 88)
		    f = n-87;
	    h=(f*40);
	    v = 290;
	    if (n >= 89)
		    v=330;
	}
    v += 10;
}
