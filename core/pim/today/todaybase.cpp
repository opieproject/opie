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
#include <qscrollview.h>
#include <qvbox.h>
#include <qapplication.h>

#include <qpe/resource.h>

/* 
 *  Constructs a TodayBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
TodayBase::TodayBase( QWidget* parent,  const char* name, WFlags fl )
  : QWidget( parent, name, fl )
{
  QPixmap logo = Resource::loadPixmap( "today/today_logo"); // logo
  QPixmap datebook = Resource::loadPixmap("DateBook"); // datebook
  QPixmap todo = Resource::loadPixmap( "TodoList" );   // todo
  QPixmap config = Resource::loadPixmap( "today/config" );  // config icon
  QPixmap mail = Resource::loadPixmap( "today/mail" ); // mail icon
  
  QPalette pal = this->palette();
  QColor col = pal.color(QPalette::Active, QColorGroup::Background);
  pal.setColor(QPalette::Active, QColorGroup::Button, col);
  this->setPalette(pal);


  QWidget *d = QApplication::desktop();
  int w=d->width();                 
  int h=d->height();                
  resize( w , h ); 
  
  // hehe, qt is ...
  getridoffuckingstrippeldlinesbutton =  new QPushButton (this, "asdfsad" );
  getridoffuckingstrippeldlinesbutton->setGeometry( QRect( -5, 10, 0, 0 ) );

  QVBoxLayout * layout = new QVBoxLayout(this);

  // --- logo Section ---
  QPalette pal2;
  QColorGroup cg;
  cg.setColor( QColorGroup::Text, white );
  cg.setBrush( QColorGroup::Background, QBrush( QColor( 238, 238, 230), logo ) );
  pal2.setActive( cg );
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
  TextLabel1->setTextFormat( RichText );
  
  OwnerField = new QLabel(this , "Owner" );
  OwnerField->setGeometry(QRect(0,0, this->width(), 12 ));
  OwnerField->setAlignment(int (QLabel::AlignTop | QLabel::AlignLeft ) );
  OwnerField->setMaximumHeight(12);
    
  // --- dates section ---
  Frame4 = new QFrame( this, "Frame4" );
  Frame4->setPalette( pal );
  Frame4->setFrameShape( QScrollView::StyledPanel );
  Frame4->setFrameShadow( QScrollView::Sunken );
  Frame4->setBackgroundOrigin( QScrollView::ParentOrigin );
  Frame4->setFrameStyle( QFrame::NoFrame );
  Frame4->setGeometry (QRect( 0, 8, this->width() , this->height()) ); 
  
  sv1 = new QScrollView( Frame4 );
  sv1->setResizePolicy(QScrollView::AutoOneFit);
  sv1->setHScrollBarMode( QScrollView::AlwaysOff );
  // need to find a better way!!!
  sv1->setGeometry (QRect( 40, 2, Frame4->width()-40 , (Frame4->height()/3)+20  ) ); 
  sv1->setFrameShape(QFrame::NoFrame);
 
  DatesButton = new QPushButton (Frame4, "DatesButton" );
  DatesButton->setGeometry( QRect( 2, 4, 36, 32 ) );
  DatesButton->setBackgroundOrigin( QPushButton::WidgetOrigin );
  DatesButton->setPalette( pal );
  DatesButton->setPixmap( datebook  );
  DatesButton->setFlat( TRUE );
  
  // --- mail section ---)
  MailFrame = new QFrame( this ,"MailFrame" );
  MailFrame->setBackgroundOrigin( QScrollView::ParentOrigin );
  MailFrame->setGeometry (QRect( 0, 0, this->width() , 15) ); 
  MailFrame->setFrameStyle( QFrame::NoFrame );
  
  QFrame* Line1 = new QFrame( MailFrame);
  Line1->setGeometry( QRect( -5, 0, MailFrame->width()+5, 5 ) );
  Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );

  MailButton = new QPushButton (MailFrame, "MailButton" );
  MailButton->setGeometry( QRect( 2, 3, 36, 19 ) );
  MailButton->setPalette( pal );
  MailButton->setPixmap( mail  );
  MailButton->setFlat( TRUE );

  MailField = new QLabel( MailFrame, "MailField" );
  MailField->setGeometry( QRect( 40, 4, MailFrame->width(), 12) ); 
  MailField->setText( tr( "Opiemail not installed" ) );
  MailField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );
  MailField->setMaximumHeight(40);
  MailField->setMinimumHeight(15);

  // --- todo section --
  Frame15 = new QFrame( this, "Frame15" );
  Frame15->setFrameStyle( QFrame::NoFrame );
  Frame15->setGeometry (QRect( 40, 3, this->width() , this->height()) ); 

  QFrame* Line2 = new QFrame( Frame15);
  Line2->setGeometry( QRect( -5, 0, MailFrame->width()+5, 5 ) );
  Line2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

  TodoButton = new QPushButton (Frame15, "TodoButton" );
  TodoButton->setGeometry( QRect( 2, 4, 36, 32 ) );
  TodoButton->setPalette( pal );
  TodoButton->setPixmap( todo  );
  TodoButton->setFlat( TRUE );

  QScrollView* sv2 = new QScrollView( Frame15 );
  sv2->setResizePolicy(QScrollView::AutoOneFit);
  sv2->setHScrollBarMode( QScrollView::AlwaysOff );
  sv2->setGeometry (QRect( 40, 3, Frame15->width()-40 , (Frame15->height()/3) ) ); 
  sv2->setFrameShape(QFrame::NoFrame);

  TodoField = new QLabel( sv2->viewport(), "TodoField" );
  sv2->addChild(TodoField);
  TodoField->setFrameShadow( QLabel::Plain );
  //TodoField->setText( tr( "No current todos" ) );
  TodoField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

  PushButton1 = new QPushButton (Frame15, "PushButton1" );
  PushButton1->setGeometry( QRect( 2, 68, 25, 21 ) ); 
  PushButton1->setPixmap( config  );
  PushButton1->setPalette( pal );
  PushButton1->setAutoDefault( TRUE );
  PushButton1->setFlat( TRUE );

  // -- layout --
  layout->addWidget(Frame);
  layout->addWidget(OwnerField);
  layout->addWidget(Frame4);
  layout->addWidget(MailFrame);
  layout->addWidget(Frame15);

  layout->setStretchFactor(Frame4,5);
  layout->setStretchFactor(MailFrame,1);
  layout->setStretchFactor(Frame15,4);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TodayBase::~TodayBase()
{
}

