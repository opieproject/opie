/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/



#include "liquidset.h"
#include "liquid.h"

#include <qpe/qpeapplication.h>
#include <qpe/global.h>

#include <qslider.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include <qpe/config.h>

#include <opie/ocolorbutton.h>


static void changeButtonColor ( QWidget *btn, const QColor &col )
{
	QPalette pal = btn-> palette ( );

	pal. setColor ( QPalette::Normal, QColorGroup::Button, col );
	pal. setColor ( QPalette::Active, QColorGroup::Button, col );
	pal. setColor ( QPalette::Disabled, QColorGroup::Button, col );
	pal. setColor ( QPalette::Inactive, QColorGroup::Button, col );
	pal. setColor ( QPalette::Normal, QColorGroup::Background, col );
	pal. setColor ( QPalette::Active, QColorGroup::Background, col );
	pal. setColor ( QPalette::Disabled, QColorGroup::Background, col );
	pal. setColor ( QPalette::Inactive, QColorGroup::Background, col );

	btn-> setPalette ( pal );
}


LiquidSettings::LiquidSettings ( QWidget* parent, const char *name, WFlags fl )
		: QWidget ( parent, name, fl )
{
	setCaption ( tr( "Liquid Style" ) );
	
	Config config ( "qpe" );
    config. setGroup ( "Liquid-Style" );

	m_type       = config. readNumEntry ( "Type", TransStippleBg );
	QColor mcol  = QColor ( config. readEntry ( "Color",  QApplication::palette ( ). active ( ). button ( ). name ( )));
	QColor tcol  = QColor ( config. readEntry ( "TextColor", QApplication::palette ( ). active ( ). text ( ). name ( )));
	int opacity  = config. readNumEntry ( "Opacity", 10 );
	m_shadow     = config. readBoolEntry ( "ShadowText", true );
	m_deco       = config. readBoolEntry ( "WinDecoration", true );
	int contrast = config. readNumEntry ( "StippleContrast", 5 );
	m_flat       = config. readBoolEntry ( "FlatToolButtons", false );

	QVBoxLayout *vbox = new QVBoxLayout ( this );
	vbox-> setSpacing ( 3 );
	vbox-> setMargin ( 4 );

	QComboBox *cb = new QComboBox ( this );
	cb-> insertItem ( tr( "No translucency" ), None );
	cb-> insertItem ( tr( "Stippled, background color" ), StippledBg );
	cb-> insertItem ( tr( "Stippled, button color" ), StippledBtn );
	cb-> insertItem ( tr( "Translucent stippled, background color" ), TransStippleBg );
	cb-> insertItem ( tr( "Translucent stippled, button color" ), TransStippleBtn );
	cb-> insertItem ( tr( "Custom translucency" ), Custom );
	
	cb-> setCurrentItem ( m_type );
	vbox-> addWidget ( cb );
	
	
	QGridLayout *grid = new QGridLayout ( vbox );
	grid-> addColSpacing ( 0, 16 );
	grid-> addColSpacing ( 3, 8 );
	
	grid-> addWidget ( m_menulbl = new QLabel ( tr( "Menu color" ), this ), 0, 1 );
	grid-> addWidget ( m_textlbl = new QLabel ( tr( "Text color" ), this ), 0, 4 );
	grid-> addWidget ( m_opaclbl = new QLabel ( tr( "Opacity" ), this ), 1, 1 );

	m_menubtn = new OColorButton ( this );
	m_menubtn-> setColor ( mcol );
	grid-> addWidget ( m_menubtn, 0, 2 );

	m_textbtn = new OColorButton ( this );
	m_textbtn-> setColor ( tcol );
	grid-> addWidget ( m_textbtn, 0, 5 );

	m_opacsld = new QSlider ( Horizontal, this );
	m_opacsld-> setRange ( -20, 20 );
	m_opacsld-> setValue ( opacity );
	m_opacsld-> setTickmarks ( QSlider::Below );
	grid-> addMultiCellWidget ( m_opacsld, 1, 1, 2, 5 );
	
	vbox-> addSpacing ( 4 );
	
	QCheckBox *shadow = new QCheckBox ( tr( "Use shadowed menu text" ), this );
	shadow-> setChecked ( m_shadow );
	vbox-> addWidget ( shadow );
	
	vbox-> addSpacing ( 4 );
	
	QCheckBox *windeco = new QCheckBox ( tr( "Draw liquid window title bars" ), this );
	windeco-> setChecked ( m_deco );
	vbox-> addWidget ( windeco );

	vbox-> addSpacing ( 4 );
	
	QCheckBox *flattb = new QCheckBox ( tr( "Make toolbar buttons appear flat" ), this );
	flattb-> setChecked ( m_flat );
	vbox-> addWidget ( flattb );

	vbox-> addSpacing ( 4 );
	
	QHBoxLayout *hbox = new QHBoxLayout ( vbox );
	
	hbox-> addWidget ( new QLabel ( tr( "Stipple contrast" ), this )); 
	
	m_contsld = new QSlider ( Horizontal, this );
	m_contsld-> setRange ( 0, 10 );
	m_contsld-> setValue ( contrast );
	m_contsld-> setTickmarks ( QSlider::Below );
	hbox-> addWidget ( m_contsld, 10 );
	
	vbox-> addStretch ( 10 );

	changeType ( m_type );
	
	connect ( cb, SIGNAL( highlighted ( int ) ), this, SLOT( changeType ( int ) ) );
	connect ( shadow, SIGNAL( toggled ( bool ) ), this, SLOT( changeShadow ( bool ) ) );
	connect ( windeco, SIGNAL( toggled ( bool ) ), this, SLOT( changeDeco ( bool ) ) );
	connect ( flattb, SIGNAL( toggled ( bool ) ), this, SLOT( changeFlat ( bool ) ) );
}

void LiquidSettings::changeType ( int t )
{
	bool custom = ( t == Custom );
	
	m_menulbl-> setEnabled ( custom );	
	m_textlbl-> setEnabled ( custom );	
	m_opaclbl-> setEnabled ( custom );	
	m_menubtn-> setEnabled ( custom );	
	m_textbtn-> setEnabled ( custom );	
	m_opacsld-> setEnabled ( custom );	
	
	m_type = t;
}

void LiquidSettings::changeShadow ( bool b )
{
	m_shadow = b;
}

void LiquidSettings::changeDeco ( bool b )
{
	m_deco = b;
}

void LiquidSettings::changeFlat ( bool b )
{
	m_flat = b;
}


bool LiquidSettings::writeConfig ( )
{
	Config config ( "qpe" );
    config. setGroup ( "Liquid-Style" );

	config. writeEntry ( "Type", m_type  );
	config. writeEntry ( "Color",  m_menubtn-> color ( ). name ( ));
	config. writeEntry ( "TextColor", m_textbtn-> color ( ). name ( ));
	config. writeEntry ( "Opacity", m_opacsld-> value ( ));
	config. writeEntry ( "ShadowText", m_shadow );	
	config. writeEntry ( "WinDecoration", m_deco );
	config. writeEntry ( "StippleContrast", m_contsld-> value ( ));
	config. writeEntry ( "FlatToolButtons", m_flat );
	config. write ( );

	return true;
}

