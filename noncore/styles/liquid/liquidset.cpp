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
#include <qtoolbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpalette.h>

#include <qpe/config.h>

#include <opie/colorpopupmenu.h>


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
	m_menucol    = QColor ( config. readEntry ( "Color",  QApplication::palette ( ). active ( ). button ( ). name ( )));
	m_textcol    = QColor ( config. readEntry ( "TextColor", QApplication::palette ( ). active ( ). text ( ). name ( )));
	int opacity  = config. readNumEntry ( "Opacity", 10 );
	m_shadow     = config. readBoolEntry ( "ShadowText", true );
	m_deco       = config. readBoolEntry ( "WinDecoration", true );
	int contrast = config. readNumEntry ( "StippleContrast", 5 );

	QVBoxLayout *vbox = new QVBoxLayout ( this );
	vbox-> setSpacing ( 3 );
	vbox-> setMargin ( 6 );

	QButtonGroup *btngrp = new QButtonGroup ( this );
	btngrp-> hide ( );

	QRadioButton *rad;
	
	rad = new QRadioButton ( tr( "No translucency" ), this );
	btngrp-> insert ( rad, None );
	vbox-> addWidget ( rad );
	
	rad = new QRadioButton ( tr( "Stippled, background color" ), this );
	btngrp-> insert ( rad, StippledBg );
	vbox-> addWidget ( rad );
	
	rad = new QRadioButton ( tr( "Stippled, button color" ), this );
	btngrp-> insert ( rad, StippledBtn );
	vbox-> addWidget ( rad );
	
	rad = new QRadioButton ( tr( "Translucent stippled, background color" ), this );
	btngrp-> insert ( rad, TransStippleBg );
	vbox-> addWidget ( rad );
	
	rad = new QRadioButton ( tr( "Translucent stippled, button color" ), this );
	btngrp-> insert ( rad, TransStippleBtn );
	vbox-> addWidget ( rad );
	
	rad = new QRadioButton ( tr( "Custom translucency" ), this );
	btngrp-> insert ( rad, Custom );
	vbox-> addWidget ( rad );
	
	btngrp-> setExclusive ( true );
	btngrp-> setButton ( m_type );
	
	QGridLayout *grid = new QGridLayout ( vbox );
	grid-> addColSpacing ( 0, 16 );
	grid-> addColSpacing ( 3, 8 );
	
	grid-> addWidget ( m_menulbl = new QLabel ( tr( "Menu color" ), this ), 0, 1 );
	grid-> addWidget ( m_textlbl = new QLabel ( tr( "Text color" ), this ), 0, 4 );
	grid-> addWidget ( m_opaclbl = new QLabel ( tr( "Opacity" ), this ), 1, 1 );

	m_menubtn = new QToolButton ( this );
	grid-> addWidget ( m_menubtn, 0, 2 );

	QPopupMenu *popup;

	popup = new ColorPopupMenu ( m_menucol, 0 );
	m_menubtn-> setPopup ( popup );
	m_menubtn-> setPopupDelay ( 0 );
	connect ( popup, SIGNAL( colorSelected ( const QColor & )), this, SLOT( changeMenuColor ( const QColor & )));
	changeMenuColor ( m_menucol );

	m_textbtn = new QToolButton ( this );
	grid-> addWidget ( m_textbtn, 0, 5 );

	popup = new ColorPopupMenu ( m_textcol, 0 );
	m_textbtn-> setPopup ( popup );
	m_textbtn-> setPopupDelay ( 0 );
	connect ( popup, SIGNAL( colorSelected ( const QColor & )), this, SLOT( changeTextColor ( const QColor & )));
	changeTextColor ( m_textcol );

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
	
	QHBoxLayout *hbox = new QHBoxLayout ( vbox );
	
	hbox-> addWidget ( new QLabel ( tr( "Stipple contrast" ), this )); 
	
	m_contsld = new QSlider ( Horizontal, this );
	m_contsld-> setRange ( 0, 10 );
	m_contsld-> setValue ( contrast );
	m_contsld-> setTickmarks ( QSlider::Below );
	hbox-> addWidget ( m_contsld, 10 );
	
	vbox-> addStretch ( 10 );

	changeType ( m_type );
	
	connect ( btngrp, SIGNAL( clicked ( int ) ), this, SLOT( changeType ( int ) ) );
	connect ( shadow, SIGNAL( toggled ( bool ) ), this, SLOT( changeShadow ( bool ) ) );
	connect ( windeco, SIGNAL( toggled ( bool ) ), this, SLOT( changeDeco ( bool ) ) );
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

void LiquidSettings::changeMenuColor ( const QColor &col )
{
	changeButtonColor ( m_menubtn, col );
	m_menucol = col;
}

void LiquidSettings::changeTextColor ( const QColor &col )
{
	changeButtonColor ( m_textbtn, col );
	m_textcol = col;
}

void LiquidSettings::changeShadow ( bool b )
{
	m_shadow = b;
}

void LiquidSettings::changeDeco ( bool b )
{
	m_deco = b;
}


bool LiquidSettings::writeConfig ( )
{
	Config config ( "qpe" );
    config. setGroup ( "Liquid-Style" );

	config. writeEntry ( "Type", m_type  );
	config. writeEntry ( "Color",  m_menucol. name ( ));
	config. writeEntry ( "TextColor", m_textcol. name ( ));
	config. writeEntry ( "Opacity", m_opacsld-> value ( ));
	config. writeEntry ( "ShadowText", m_shadow );	
	config. writeEntry ( "WinDecoration", m_deco );
	config. writeEntry ( "StippleContrast", m_contsld-> value ( ));
	config. write ( );

	return true;
}

