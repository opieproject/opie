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
#include <qlabel.h>
#include <qlist.h>
#include <qvbox.h>
#include "dataTable.h"
#include "psewidget.h"
#include "oxyframe.h"


/*Element::Element( const QString &name )
{
}*/


PSEWidget::PSEWidget() : QWidget()
{
    this->setCaption( "Periodic System" );

    QVBoxLayout *vlay = new QVBoxLayout( this );
    
    QGridLayout *grid = new QGridLayout( 18,10 );
    int h=0, v=0;
    for( int n = 0 ; n < 118 ; n++ )
    {
        position( n+1,h,v );
        QList<OxyFrame> PSEframe;
        PSEframe.append( new OxyFrame( this , QString::number( n ) ) );
        grid->addWidget( PSEframe.current() , v/40+1 , h/40 );
        PSEframe.current()->setMinimumHeight( 11 );
        connect( PSEframe.current(), SIGNAL( clicked() ), this, SLOT( slotShowElement() ));
    }
    
    OxydataWidget *oxyDW = new OxydataWidget(this);
    oxyDW->setElement( 1 );

    vlay->addLayout( grid );
    vlay->addWidget( oxyDW );
}

void PSEWidget::slotShowElement(){ qDebug( "ja genau" );};

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
