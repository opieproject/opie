/*
 * todaybase.cpp
 *
 * copyright   : (c) 2002, 2003 by Maximilian Reiﬂ
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "todaybase.h"

#include <qvbox.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <qpe/resource.h>


TodayBase::TodayBase( QWidget* parent,  const char* name, WFlags )
    : QWidget( parent, name, WStyle_ContextHelp ) {

  QPixmap logo = Resource::loadPixmap( "today/today_logo"); // logo
  QPixmap opiezilla = Resource::loadPixmap("today/opiezilla" ); //the opiezilla
  QPixmap config = Resource::loadPixmap( "SettingsIcon" );  // config icon

  layout = 0L;

  QPalette pal = this->palette();
  QColor col = pal.color( QPalette::Active, QColorGroup::Background );
  pal.setColor( QPalette::Active, QColorGroup::Button, col );
  pal.setColor( QPalette::Inactive, QColorGroup::Button, col );
  pal.setColor( QPalette::Normal, QColorGroup::Button, col );
  pal.setColor( QPalette::Disabled, QColorGroup::Button, col );
  this->setPalette( pal );

  // --- logo Section ---
  QPalette pal2;
  QColorGroup cg;
  cg.setColor( QColorGroup::Text, white );
  cg.setBrush( QColorGroup::Background, QBrush( QColor( 238, 238, 230 ), logo ) );
  pal2.setActive( cg );

// today logo
  Frame = new QLabel( this, "Frame" );
  Frame->setPalette( pal2 );
  Frame->setFrameShape( QFrame::StyledPanel );
  Frame->setFrameShadow( QFrame::Raised );
  Frame->setLineWidth( 0 );

  QHBoxLayout *frameLayout = new QHBoxLayout( Frame );
  QVBox *box1 = new QVBox( Frame );

  // Today text
  TodayLabel = new QLabel( box1, "TodayText" );
  QFont TodayLabel_font(  TodayLabel->font() );
  TodayLabel_font.setBold( TRUE );
  TodayLabel_font.setPointSize( 40 );
  TodayLabel->setFont( TodayLabel_font );
  TodayLabel->setBackgroundOrigin( QLabel::ParentOrigin );
  TodayLabel->setText( "<font color=#FFFFFF>" + tr("Today") +"</font>" );

  // date
  DateLabel = new QLabel( box1, "TextLabel1" );
  QFont DateLabel_font( DateLabel->font() );
  DateLabel_font.setBold( TRUE );
  DateLabel->setFont( DateLabel_font );
  DateLabel->setBackgroundOrigin( QLabel::ParentOrigin );
  DateLabel->setTextFormat( RichText );

  // Opiezilla
  Opiezilla = new QLabel( Frame, "OpieZilla" );
  Opiezilla->setPixmap( opiezilla );
  QWhatsThis::add( Opiezilla , tr( "Today by Maximilian Reiﬂ" ) );
  Opiezilla->setBackgroundOrigin( QLabel::ParentOrigin );


  // Ownerfield
  OwnerField = new OClickableLabel( this , "Owner" );
  OwnerField->setAlignment( int (QLabel::AlignTop | QLabel::AlignLeft ) );

  // config
  ConfigButton = new OClickableLabel ( Frame, "PushButton1" );
  ConfigButton->setPixmap( config );
  QWhatsThis::add( ConfigButton, tr( "Click here to get to the config dialog" ) );
  ConfigButton->setBackgroundOrigin( QLabel::ParentOrigin );

  frameLayout->addWidget( box1, 1 );
  frameLayout->addStretch( 1 );
  frameLayout->addWidget( ConfigButton,  0, AlignBottom );
  frameLayout->addWidget( Opiezilla );
}


/**
 *  D' tor
 */
TodayBase::~TodayBase() {
}

