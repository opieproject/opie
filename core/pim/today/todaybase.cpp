/*
                             This file is part of the Opie Project

                             Copyright (C) Maximilian Reiss <harlekin@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "todaybase.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>

#include <qvbox.h>
#include <qwhatsthis.h>

using namespace Opie::Ui;
TodayBase::TodayBase( QWidget* parent,  const char* name, WFlags )
    : QWidget( parent, name, WStyle_ContextHelp ) {

  QPixmap logo = Opie::Core::OResource::loadPixmap( "today/today_logo" ); // logo
  QImage  opiezillaimage = QImage( Opie::Core::OResource::loadImage( "logo/opielogo" ) );
  opiezillaimage = opiezillaimage.smoothScale( 45, 45 );
  QPixmap opiezilla; //the opiezilla
  opiezilla.convertFromImage( opiezillaimage );
  QPixmap config = Opie::Core::OResource::loadPixmap( "SettingsIcon", Opie::Core::OResource::SmallIcon );

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
  QWhatsThis::add( Opiezilla , tr( "Today by Maximilian Rei�" ) );
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

