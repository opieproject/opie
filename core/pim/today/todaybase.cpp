/*
 * todaybase.cpp *
 * ---------------------
 *
 * begin       : Sun 10 17:20:00 CEST 2002
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : max.reiss@gmx.de
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

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
//#include <qscrollview.h>

#include <qpe/resource.h>

/* 
 *  Constructs a TodayBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
TodayBase::TodayBase( QWidget* parent,  const char* name, WFlags fl )
  : QWidget( parent, name, fl )
{
  // logo
  QPixmap logo = Resource::loadPixmap( "today/today_logo");
  // datebook
  QPixmap datebook = Resource::loadPixmap("DateBook");
  // todo
  QPixmap todo = Resource::loadPixmap( "TodoList" );
  // config icon
  QPixmap config = Resource::loadPixmap( "today/config" );
  // mail icon
  QPixmap mail = Resource::loadPixmap( "today/mail" );
  
  
QPalette pal2;
 QColorGroup cg;
 cg.setColor( QColorGroup::Text, white );
 cg.setBrush( QColorGroup::Background, QBrush( QColor( 238, 238, 230), logo ) );
 pal2.setActive( cg );


QPalette pal = this->palette();
QColor col = pal.color(QPalette::Active, QColorGroup::Background);
pal.setColor(QPalette::Active, QColorGroup::Button, col);
this->setPalette(pal);

  QVBoxLayout * layout = new QVBoxLayout(this);
  

  Frame4 = new QFrame( this, "Frame4" );
  Frame4->setFrameShape( QScrollView::StyledPanel );
  Frame4->setFrameShadow( QScrollView::Sunken );
  Frame4->setBackgroundOrigin( QScrollView::ParentOrigin );

  // hehe, qt is ...
  getridoffuckingstrippeldlinesbutton =  new QPushButton (Frame4, "asdfsad" );
  getridoffuckingstrippeldlinesbutton->setGeometry( QRect( -5, 10, 0, 0 ) );

  DatesButton = new QPushButton (Frame4, "DatesButton" );
  DatesButton->setGeometry( QRect( 2, 4, 36, 32 ) );
  DatesButton->setBackgroundOrigin( QPushButton::WidgetOrigin );
  DatesButton->setPalette( pal );
  DatesButton->setPixmap( datebook  );
  DatesButton->setFlat( TRUE );

  DatesField = new QLabel( Frame4, "DatesField" );
  DatesField->setGeometry( QRect( 40, 4, 203, 120 ) ); 
  DatesField->setText( tr( "No appointments today" ) );
  DatesField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

  // today logo
  Frame = new QLabel( this, "Frame" );
  Frame->setPalette( pal2 );
  Frame->setFrameShape( QFrame::StyledPanel );
  Frame->setFrameShadow( QFrame::Raised );
  Frame->setLineWidth( 0 );
  Frame->setMaximumHeight(50);
  Frame->setMinimumHeight(50);
  
  // date
  TextLabel1 = new QLabel( Frame, "TextLabel1" );
  TextLabel1->setGeometry( QRect( 10, 35, 168, 12 ) ); 
  QFont TextLabel1_font(  TextLabel1->font() );
  TextLabel1_font.setBold( TRUE );
  TextLabel1->setFont( TextLabel1_font ); 
  TextLabel1->setBackgroundOrigin( QLabel::ParentOrigin );
  TextLabel1->setTextFormat( QLabel::AutoText );
   
  // todo
  Frame15 = new QFrame( this, "Frame15" );
  Frame15->setFrameShape( QFrame::StyledPanel );
  Frame15->setFrameShadow( QFrame::Sunken );
  
  TodoButton = new QPushButton (Frame15, "TodoButton" );
  TodoButton->setGeometry( QRect( 2, 4, 36, 32 ) );
  //TodoButton->setBackgroundMode(NoBackground);
  TodoButton->setPalette( pal );
  //TodoButton->setBackgroundOrigin( QPushButton::WidgetOrigin );
  TodoButton->setPixmap( todo  );
  TodoButton->setFlat( TRUE );

  TodoField = new QLabel( Frame15, "TodoField" );
  TodoField->setGeometry( QRect( 40, 4, 196, 120 ) ); 
  TodoField->setFrameShadow( QLabel::Plain );
  TodoField->setText( tr( "No current todos" ) );
  TodoField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

  PushButton1 = new QPushButton (Frame15, "PushButton1" );
  PushButton1->setGeometry( QRect( 216, 68, 25, 21 ) ); 
  //PushButton1->setBackgroundOrigin( QPushButton::WidgetOrigin );
  PushButton1->setPixmap( config  );
  PushButton1->setPalette( pal );
  PushButton1->setAutoDefault( TRUE );
  PushButton1->setFlat( TRUE );

  // mail
  MailFrame = new QFrame( this ,"MailFrame" );
  MailFrame->setFrameShape( QScrollView::StyledPanel );
  MailFrame->setFrameShadow( QScrollView::Sunken );
  //MailFrame->setBackgroundOrigin( QScrollView::ParentOrigin );

  MailButton = new QPushButton (MailFrame, "MailButton" );
  MailButton->setGeometry( QRect( 2, 10, 36, 19 ) );
  //MailButton->setBackgroundOrigin( QPushButton::WidgetOrigin );
  MailButton->setPalette( pal );
  MailButton->setPixmap( mail  );
  MailButton->setFlat( TRUE );

  MailField = new QLabel( MailFrame, "DatesField" );
  MailField->setGeometry( QRect( 40, 10, 203, 120 ) ); 
  MailField->setText( tr( "Opiemail not installed" ) );
  MailField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );
  MailField->setMaximumHeight(15);
  MailField->setMinimumHeight(10);


  

  layout->addWidget(Frame);
  layout->addWidget(Frame4);
  layout->addWidget(MailFrame);
  layout->addWidget(Frame15);
  //layout->addWidget(split);

  layout->setStretchFactor(Frame4,4);
  layout->setStretchFactor(MailFrame,1);
  layout->setStretchFactor(Frame15,3);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TodayBase::~TodayBase()
{

}

