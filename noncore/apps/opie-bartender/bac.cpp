/****************************************************************************
**
** Created: Sun Jul 21 19:00:14 2002
**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#include "bac.h"

#include <qcombobox.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
BacDialog::BacDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
                : QDialog( parent, name, modal, fl )
{
        if ( !name )
                setName( "BacDialog" );
        setCaption( tr( "Blood Alcohol Estimator" ) );

        Layout7 = new QVBoxLayout( this);
        Layout7->setSpacing( 6 );
        Layout7->setMargin( 0 );

        Layout1 = new QHBoxLayout;
        Layout1->setSpacing( 6 );
        Layout1->setMargin( 0 );

        NumberSpinBox = new QSpinBox( this, "NumberSpinBox" );
        Layout1->addWidget( NumberSpinBox );

        TextLabel1 = new QLabel(this, "TextLabel1" );
        TextLabel1->setText( tr( "# Drinks Consumed" ) );
        Layout1->addWidget( TextLabel1 );
        Layout7->addLayout( Layout1 );

        Layout2 = new QHBoxLayout;
        Layout2->setSpacing( 6 );
        Layout2->setMargin( 0 );

        WeightSpinBox = new QSpinBox( this, "WeightSpinBox" );
        Layout2->addWidget( WeightSpinBox );
        WeightSpinBox->setMaxValue(500);

        TextLabel2 = new QLabel( this, "TextLabel2" );
        TextLabel2->setText( tr( "Weight" ) );
        Layout2->addWidget( TextLabel2 );
        Layout7->addLayout( Layout2 );

        weightUnitsCombo =new QComboBox( FALSE, this, "unitComboBox" );
        weightUnitsCombo->insertItem( tr( "Kilos" ) );
        weightUnitsCombo->insertItem( tr( "Pounds" ) );
        Layout2->addWidget( weightUnitsCombo );

        weightUnitsLabel = new QLabel(this, "unitsTextLabel" );
        weightUnitsLabel->setText( tr( "Units" ) );
        Layout2->addWidget( weightUnitsLabel );

        Layout3 = new QHBoxLayout;
        Layout3->setSpacing( 6 );
        Layout3->setMargin( 0 );

        TimeSpinBox = new QSpinBox( this, "TimeSpinBox" );
        Layout3->addWidget( TimeSpinBox );
        TimeSpinBox->setMaxValue(24);

        TextLabel3 = new QLabel( this, "TextLabel3" );
        TextLabel3->setText( tr( "Period of Time (hours)" ) );
        Layout3->addWidget( TextLabel3 );
        Layout7->addLayout( Layout3 );

        Layout4 = new QHBoxLayout;
        Layout4->setSpacing( 6 );
        Layout4->setMargin( 0 );

        GenderComboBox = new QComboBox( FALSE, this, "GenderComboBox" );
        GenderComboBox->insertItem( tr( "Male" ) );
        GenderComboBox->insertItem( tr( "Female" ) );
        GenderComboBox->insertItem( tr( "Unknown" ) );
        Layout4->addWidget( GenderComboBox );

        TextLabel4 = new QLabel( this, "TextLabel4" );
        TextLabel4->setText( tr( "Gender" ) );
        Layout4->addWidget( TextLabel4 );
        Layout7->addLayout( Layout4 );

        Layout6 = new QHBoxLayout;
        Layout6->setSpacing( 6 );
        Layout6->setMargin( 0 );

        TypeDrinkComboBox = new QComboBox( FALSE,this, "TypeDrinkComboBox" );
        TypeDrinkComboBox->insertItem( tr( "Beer" ) );
        TypeDrinkComboBox->insertItem( tr( "Wine" ) );
        TypeDrinkComboBox->insertItem( tr( "Shot" ) );
        Layout6->addWidget( TypeDrinkComboBox );

        TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
        TextLabel1_2->setText( tr( "Type of drink" ) );
        Layout6->addWidget( TextLabel1_2 );
        Layout7->addLayout( Layout6 );

        PushButton1 = new QPushButton( this, "PushButton1" );
        PushButton1->setText( tr( "Calculate" ) );
        Layout7->addWidget( PushButton1 );
        connect(PushButton1,SIGNAL( clicked()), this, SLOT( calculate()));

        LCDNumber1 = new QLCDNumber( this, "LCDNumber1" );
        LCDNumber1->setMaximumHeight( 50);
        LCDNumber1->setNumDigits(6);
        LCDNumber1->setSmallDecimalPoint(TRUE);
        LCDNumber1->setFrameStyle(QFrame::Box);
        LCDNumber1->setLineWidth(2);
        LCDNumber1->setSegmentStyle( QLCDNumber::Filled);
        QPalette palette = LCDNumber1->palette();
        palette.setColor(QPalette::Normal, QColorGroup::Foreground, Qt::red);
        palette.setColor(QPalette::Normal, QColorGroup::Light, Qt::black);
        palette.setColor(QPalette::Normal, QColorGroup::Dark, Qt::darkGreen);
        LCDNumber1->setPalette(palette);

        Layout7->addWidget( LCDNumber1 );
	NumberSpinBox->setFocus();
}

BacDialog::~BacDialog()
{}

void BacDialog::calculate()
{

        float weight,genderDiff, bac, typeDrink=0, drinkOz=0, bodyWater, milliliters, oz, gram, gramsMil, alc, metab, bac1;
        float weightunitDifference, massunitdiff;
        QString estBac;

        if( GenderComboBox->currentItem()==0)
                genderDiff = .58;
        else
                genderDiff = .49;

        switch(TypeDrinkComboBox->currentItem()) {
        case 0: { //beer
                        typeDrink = .045;
                        drinkOz = 12;
                }
                break;
        case 1: {
                        typeDrink = .2;
                        drinkOz = 4;
                }
                break;
        case 2: {
                        typeDrink = .5;
                        drinkOz = 1.5;
                }
                break;
        };

	if( weightUnitsCombo->currentItem() == 0 ) {
                weight = WeightSpinBox->value(); // kilos
        } else {
                weight = WeightSpinBox->value() / 2.2046; // convert to kilograms
        }
                bodyWater = weight * genderDiff;
                milliliters = bodyWater * 1000;
                oz = 23.36/milliliters;
                gram = oz * .806;
                gramsMil = gram * 100;
                alc = drinkOz*NumberSpinBox->value() * typeDrink;
                metab = TimeSpinBox->value() * .012;
                bac1 = gramsMil * alc;
                bac = bac1 - metab;

        //       weightDrink= (nDrinks * .79) / (weight * genderDiff*1000);
        // (ounces * percent * 0.075 / weight) - (hours * 0.015);
        //       bac = (((weightDrink * .806) * 100) * .54);// - (time * .012);  // assuming beer of 4.5%
        estBac.sprintf("%f",bac);
        LCDNumber1->display(bac);
        //       BACTextLabel->setText(estBac );
}
