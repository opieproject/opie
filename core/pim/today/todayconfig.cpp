/*
 * todayconfig.cpp
 *
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

#include "todayconfig.h"

#include <qcheckbox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
//#include <qwhatsthis.h>

todayconfig::todayconfig( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
  if ( !name )
      setName( "todayconfig" );
  resize( 175, 232 );
  setCaption( tr( "Today config" ) );

  TabWidget3 = new QTabWidget( this, "TabWidget3" );
  TabWidget3->setGeometry( QRect( 0, 0, 220, 320 ) );
  TabWidget3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, TabWidget3->sizePolicy().hasHeightForWidth() ) );
  TabWidget3->setAutoMask( FALSE );
  TabWidget3->setTabShape( QTabWidget::Rounded );

  tab = new QWidget( TabWidget3, "tab" );

  Frame8 = new QFrame( tab, "Frame8" );
  Frame8->setGeometry( QRect( -5, 0, 200, 300 ) );
  Frame8->setFrameShape( QFrame::StyledPanel );
  Frame8->setFrameShadow( QFrame::Raised );

  TextLabel4 = new QLabel( Frame8, "TextLabel4" );
  TextLabel4->setGeometry( QRect( 20, 65, 100, 60 ) );
  TextLabel4->setText( tr( "Should the \n"
			   "location \n"
			   "be shown?" ) );

  TextLabel5 = new QLabel( Frame8, "TextLabel5" );
  TextLabel5->setGeometry( QRect( 20, 160, 120, 40 ) );
  TextLabel5->setText( tr( "Should the notes \n"
			   "be shown?" ) );

  CheckBox2 = new QCheckBox( Frame8, "CheckBox2" );
  CheckBox2->setGeometry( QRect( 158, 170, 27, 21 ) );
  //CheckBox2->setText( tr( "" ) );

  CheckBox1 = new QCheckBox( Frame8, "CheckBox1" );
  CheckBox1->setGeometry( QRect( 158, 65, 27, 50 ) );
  //CheckBox1->setText( tr( "" ) );

  CheckBox3 = new QCheckBox (Frame8, "CheckBox3" );
  CheckBox3->setGeometry( QRect( 158, 125, 27, 21 ) );

  TextLabel6 = new QLabel( Frame8, "All Day");
  TextLabel6->setGeometry( QRect( 20, 120, 100, 30 ) );
  TextLabel6->setText( tr( "Show only later\n"
			   "appointments") );

  SpinBox1 = new QSpinBox( Frame8, "SpinBox1" );
  SpinBox1->setGeometry( QRect( 115, 20, 58, 25 ) );
  SpinBox1->setMaxValue( 10 );
  SpinBox1->setValue( 5 );

  TextLabel3 = new QLabel( Frame8, "TextLabel3" );
  TextLabel3->setGeometry( QRect( 20, 10, 90, 60 ) );
  TextLabel3->setText( tr( "How many \n"
			   "appointment\n"
			   "should\n"
			   "be shown?" ) );
  TabWidget3->insertTab( tab, tr( "Calendar" ) );

  tab_2 = new QWidget( TabWidget3, "tab_2" );

  Frame9 = new QFrame( tab_2, "Frame9" );
  Frame9->setGeometry( QRect( -5, 0, 230, 310 ) );
  Frame9->setFrameShape( QFrame::StyledPanel );
  Frame9->setFrameShadow( QFrame::Raised );

  TextLabel6 = new QLabel( Frame9, "TextLabel6" );
  TextLabel6->setGeometry( QRect( 20, 10, 100, 60 ) );
  TextLabel6->setText( tr( "How many\n"
			   "tasks should \n"
			   "be shown?" ) );

  SpinBox2 = new QSpinBox( Frame9, "SpinBox2" );
  SpinBox2->setGeometry( QRect( 115, 20, 58, 25 ) );
  SpinBox2->setMaxValue( 20 );
  SpinBox2->setValue( 5 );
  TabWidget3->insertTab( tab_2, tr( "Tasks" ) );

  tab_3 = new QWidget( TabWidget3, "tab_3" );

  Frame14 = new QFrame( tab_3, "Frame14" );
  Frame14->setGeometry( QRect( -5, 0, 200, 220 ) );
  Frame14->setFrameShape( QFrame::StyledPanel );
  Frame14->setFrameShadow( QFrame::Raised );

  TextLabel1 = new QLabel( Frame14, "TextLabel1" );
  TextLabel1->setGeometry( QRect( 20, 20, 100, 30 ) );
  TextLabel1->setText( tr( "Clip after how\n"
			   "many letters" ) );

  SpinBox7 = new QSpinBox( Frame14, "SpinBox7" );
  SpinBox7->setGeometry( QRect( 115, 20, 58, 25 ) );
  SpinBox7->setMaxValue( 80 );

  TextLabel2 = new QLabel( Frame14, "AutoStart" );
  TextLabel2->setGeometry( QRect( 20, 60, 100, 45 ) );
  TextLabel2->setText( tr( "Should today be\n"
			   "autostarted on\n"
			   "resume?"
			   " (Opie only)" ) );

  CheckBoxAuto = new QCheckBox (Frame14, "CheckBoxAuto" );
  CheckBoxAuto->setGeometry( QRect( 158, 60, 27, 21 ) );

  TimeLabel = new QLabel( Frame14, "TimeLabel" );
  TimeLabel->setGeometry( QRect ( 20, 120, 120, 45 ) );
  TimeLabel->setText( tr( "Activate the  \n"
                          "autostart after how\n"
                          "many minutes?" ) );
  SpinBoxTime = new QSpinBox( Frame14, "TimeSpinner");
  SpinBoxTime->setGeometry( QRect( 115,  120,  58, 25 ) );

  TabWidget3->insertTab( tab_3, tr( "Misc" ) );

}

todayconfig::~todayconfig() {
}

