 /***************************************************************************
//                            setAlarm.cpp  -  description
//                               -------------------
//       Created: Wed Mar 13 19:47:24 2002
//      copyright            : (C) 2002 by ljp
//      email                : ljp@llornkcor.com
// 
***************************************************************************
   *   This program is free software; you can redistribute it and/or modify  *
   *   it under the terms of the GNU General Public License as published by  *
   *   the Free Software Foundation; either version 2 of the License, or     *
   *   (at your option) any later version.                                   *
   ***************************************************************************/

#include "setAlarm.h"

#include <opie/ofileselector.h>
#include <opie/ofiledialog.h>

#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qstring.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>

Set_Alarm::Set_Alarm( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "Set_Alarm" );
    resize( 240, 101 ); 
    setMaximumSize( QSize( 240, 320 ) );
    move(0,45);
    setCaption( tr( "Set Alarm" ) );
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    Set_AlarmLayout = new QGridLayout( this ); 
    Set_AlarmLayout->setSpacing( 6 );
    Set_AlarmLayout->setMargin( 11 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr( "Hour" ) );

    Set_AlarmLayout->addWidget( TextLabel1, 0, 0 );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setText( tr( "Minute" ) );

    Set_AlarmLayout->addMultiCellWidget( TextLabel2, 0, 0, 1, 2 );

    Hour_Slider = new QSlider( this, "Hour_Slider" );
    Hour_Slider->setPageStep( 1);
    Hour_Slider->setOrientation( QSlider::Horizontal );
    connect(Hour_Slider, SIGNAL( valueChanged(int)),this,SLOT(slotChangeHour(int)));

    Set_AlarmLayout->addWidget( Hour_Slider, 2, 0 );

    Hour_LCDNumber = new QLCDNumber( this, "Hour_LCDNumber" );
    Hour_LCDNumber->setFrameShape( QLCDNumber::Box );
    Hour_LCDNumber->setFrameShadow( QLCDNumber::Plain );
    Hour_LCDNumber->setSegmentStyle( QLCDNumber::Flat );

    Set_AlarmLayout->addWidget( Hour_LCDNumber, 1, 0 );

    Minute_Slider = new QSlider( this, "Minute_Slider" );
    Minute_Slider->setMaxValue( 59);
    Minute_Slider->setPageStep( 1);
    Minute_Slider->setOrientation( QSlider::Horizontal );
    connect(Minute_Slider, SIGNAL(  valueChanged(int)),this,SLOT(slotChangeMinute(int)));
    
    Set_AlarmLayout->addMultiCellWidget( Minute_Slider, 2, 2, 1, 2 );

    Minute_LCDNumber = new QLCDNumber( this, "Minute_LCDNumber" );
    Minute_LCDNumber->setFrameShape( QLCDNumber::Box );
    Minute_LCDNumber->setFrameShadow( QLCDNumber::Plain );
    Minute_LCDNumber->setSegmentStyle( QLCDNumber::Flat );

    Set_AlarmLayout->addMultiCellWidget( Minute_LCDNumber, 1, 1, 1, 2 );

    Am_RadioButton = new QRadioButton( this, "Am_RadioButton" );
    Am_RadioButton->setText( tr( "AM" ) );
    Am_RadioButton->setChecked(TRUE);
    connect( Am_RadioButton, SIGNAL(toggled(bool)),this,SLOT( amButtonToggled(bool)));
    
    Set_AlarmLayout->addMultiCellWidget( Am_RadioButton, 0, 1, 3, 4 );


    Pm_RadioButton = new QRadioButton( this, "Pm_RadioButton" );
    Pm_RadioButton->setText( tr( "PM" ) );
    connect( Pm_RadioButton, SIGNAL(toggled(bool)),this,SLOT( pmButtonToggled(bool)));
    
    Set_AlarmLayout->addMultiCellWidget(Pm_RadioButton, 1, 2, 3, 4 );

    useMp3Check = new QCheckBox ( tr( "mp3 alarm" ), this );
    useMp3Check-> setFocusPolicy ( QWidget::NoFocus );
    Set_AlarmLayout->addMultiCellWidget( useMp3Check, 2, 3, 3, 4 );



    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setText( tr( "Snooze Delay\n(minutes)" ) );

    Set_AlarmLayout->addMultiCellWidget( TextLabel3, 3, 3, 0, 1 );

    Snooze_LCDNumber = new QLCDNumber( this, "Snooze_LCDNumber" );
    Snooze_LCDNumber->setFrameShape( QLCDNumber::Box );
    Snooze_LCDNumber->setFrameShadow( QLCDNumber::Plain );
    Snooze_LCDNumber->setSegmentStyle( QLCDNumber::Flat );

    Set_AlarmLayout->addMultiCellWidget( Snooze_LCDNumber, 3, 3, 1, 2 );

    SnoozeSlider = new QSlider( this, "SnoozeSlider" );
    SnoozeSlider->setMaxValue( 60 );
    SnoozeSlider->setOrientation( QSlider::Horizontal );
    connect(SnoozeSlider, SIGNAL(  valueChanged(int)),this,SLOT(slotChangeSnooze(int)));

    Set_AlarmLayout->addMultiCellWidget( SnoozeSlider, 4, 4, 1, 2 );

    Config config( "qpe" );
    config.setGroup("Time");

    bool ok;
    bool ampm = config.readBoolEntry( "AMPM", TRUE );
    QString alarmHour=config.readEntry("clockAlarmHour","8");
    int i_alarmHour = alarmHour.toInt(&ok,10);
    QString alarmMinute=config.readEntry("clockAlarmMinute","0");
    QString snoozeTime=config.readEntry("clockAlarmSnooze","0");
    if(ampm) {
        Hour_Slider->setMaxValue( 12);
        Hour_Slider->setMinValue( 1);
        
        if( i_alarmHour > 12) {
            i_alarmHour = i_alarmHour - 12;
            Pm_RadioButton->setChecked(TRUE);
        }
        else if ( i_alarmHour == 0 ) {
          i_alarmHour = 12; 
        } 
        Hour_Slider->setValue( i_alarmHour );
        Minute_Slider->setValue( alarmMinute.toInt(&ok,10) );
        SnoozeSlider->setValue( snoozeTime.toInt(&ok,10) );
    } else {
        Hour_Slider->setMaxValue( 23);
        Hour_Slider->setMinValue( 0);
        Hour_Slider->setValue( i_alarmHour);
        Minute_Slider->setValue( alarmMinute.toInt(&ok,10) );
        SnoozeSlider->setValue( snoozeTime.toInt(&ok,10) );
        Am_RadioButton->hide();
        Pm_RadioButton->hide();
    }
    if( config.readBoolEntry("mp3Alarm") )
        useMp3Check->setChecked(true);        

    // signals and slots connections
    connect(useMp3Check,SIGNAL(toggled(bool)),this,SLOT(slotChangemp3CkeckBox(bool)));
}

Set_Alarm::~Set_Alarm()
{

}

void Set_Alarm::slotChangeHour(int hour)
{
    Hour_LCDNumber->display(hour);
}

void Set_Alarm::slotChangeMinute(int minute)
{
    Minute_LCDNumber->display(minute);
}

void Set_Alarm::slotChangeSnooze(int minute)
{
    Snooze_LCDNumber->display(minute);
}

void Set_Alarm::amButtonToggled(bool b)
{
    if ( b)
        Pm_RadioButton->setChecked(FALSE);
}

void Set_Alarm::pmButtonToggled(bool b)
{
    if (b)
        Am_RadioButton->setChecked(FALSE);
}

void Set_Alarm::cleanUp()
{
}

void Set_Alarm::slotChangemp3CkeckBox(bool b) {
    Config config( "qpe" );
    config.setGroup("Time");
    if(b) {
        QMap<QString, QStringList> map;
        map.insert(tr("All"), QStringList() );
        QStringList text;
        text << "audio/*";
        map.insert(tr("Audio"), text );
        QString str = OFileDialog::getOpenFileName( 2,"/", QString::null, map);//,"", "*", this );
//        QString str = OFileDialog::getOpenFileName( 2,"/");//,"", "*", this );
        if(!str.isEmpty() ) {
            qDebug(str);
            config.writeEntry("mp3Alarm",1);
            config.writeEntry("mp3File",str);
        }
    } else {
        config.writeEntry("mp3Alarm",0);
        config.writeEntry("mp3File","");
    }
}
