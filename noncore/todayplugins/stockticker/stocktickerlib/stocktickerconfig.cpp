/*
 * stocktickeRconfig.cpp
 *
 * copyright   : (c) 2002 by ljp
 * email       : llornkcor@handhelds.org
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

#include "stocktickerconfig.h"
#include <opie/todayconfigwidget.h>

#include <qpe/config.h>

#include <qapplication.h>

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qpe/config.h>
#include <qstringlist.h>
#include <qmainwindow.h>

#include <stdlib.h>

StocktickerPluginConfig::StocktickerPluginConfig( QWidget *parent,  const char* name)
    : TodayConfigWidget(parent,  name ) {

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing(2);
    layout->setMargin( 2);

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setFocus();
//     QWhatsThis::add( LineEdit1, tr("Enter the stock symbols you want to be shown here."));
    
    layout->addMultiCellWidget(  LineEdit1, 0, 0, 0, 4);
    
    Config cfg( "stockticker");
    cfg.setGroup( "Symbols" );
    QString symbollist;
    symbollist = cfg.readEntry("Symbols", "");
    LineEdit1->setText(symbollist);
    
    QLabel *label;
    label = new QLabel(this);
    label->setText( tr("Enter stock symbols seperated\nby a space."));
    label->setMaximumHeight(60);
    layout->addMultiCellWidget( label, 1, 1, 0, 4);

    cfg.setGroup( "Fields" );

    timeCheck= new QCheckBox ( "Time",this );
    timeCheck->setChecked( cfg.readBoolEntry("timeCheck",1));
    layout->addMultiCellWidget(timeCheck, 2, 2, 0, 0 );
    QWhatsThis::add( timeCheck, tr("Toggles Time of current price field"));
    
    dateCheck= new QCheckBox ( "Date", this );
    dateCheck->setChecked( cfg.readBoolEntry("dateCheck",1));
    layout->addMultiCellWidget( dateCheck, 2, 2, 1, 1 );
    QWhatsThis::add(dateCheck, tr("Toggles date field"));
    
    symbolCheck= new QCheckBox ( "Symbol", this );
    symbolCheck->setChecked( cfg.readBoolEntry("symbolCheck",1));
    layout->addMultiCellWidget( symbolCheck, 2, 2, 2, 2 );
    QWhatsThis::add(symbolCheck, tr("Toggles Symbol field"));

    nameCheck= new QCheckBox ( "Name", this );
    nameCheck->setChecked( cfg.readBoolEntry("nameCheck",1));
    layout->addMultiCellWidget( nameCheck, 3, 3, 0, 0 );
    QWhatsThis::add(nameCheck, tr("Toggles Name of symbols owner field"));

    currentPriceCheck= new QCheckBox ( "Price", this );
    currentPriceCheck->setChecked( cfg.readBoolEntry("currentPriceCheck",1));
    layout->addMultiCellWidget( currentPriceCheck, 3, 3, 1, 1 );
    QWhatsThis::add(currentPriceCheck, tr("Toggles current Price field"));

    lastPriceCheck= new QCheckBox ( "Last Price", this );
    lastPriceCheck->setChecked( cfg.readBoolEntry("lastPriceCheck",1));
    layout->addMultiCellWidget(lastPriceCheck, 3, 3, 2, 2);
    QWhatsThis::add(lastPriceCheck, tr("Toggles last price field"));

    openPriceCheck= new QCheckBox ( "Open Price", this);
    openPriceCheck->setChecked( cfg.readBoolEntry("openPriceCheck",1));
    layout->addMultiCellWidget( openPriceCheck, 4, 4, 0, 0 );
    QWhatsThis::add(openPriceCheck, tr("Toggles opening price field"));

    minPriceCheck= new QCheckBox ( "Min Price", this );
    minPriceCheck->setChecked( cfg.readBoolEntry("minPriceCheck",1));
    layout->addMultiCellWidget( minPriceCheck, 4, 4, 1, 1);
    QWhatsThis::add(minPriceCheck, tr("Toggles minimum daily price field"));

    maxPriceCheck= new QCheckBox ( "Max Price", this);
    maxPriceCheck->setChecked( cfg.readBoolEntry("maxPriceCheck",1));
    layout->addMultiCellWidget( maxPriceCheck, 4, 4, 2, 2 );
    QWhatsThis::add(maxPriceCheck, tr("Toggles maximum daily price field"));

    variationCheck= new QCheckBox ( "Variation", this );
    variationCheck->setChecked( cfg.readBoolEntry("variationCheck",1));
    layout->addMultiCellWidget( variationCheck, 5, 5, 0, 0 );
    QWhatsThis::add(variationCheck, tr("Toggles daily variation of price field"));

    volumeCheck= new QCheckBox ( "Volume", this );
    volumeCheck->setChecked( cfg.readBoolEntry("volumeCheck",1));
    layout->addMultiCellWidget( volumeCheck , 5, 5, 1, 1);
    QWhatsThis::add(volumeCheck, tr("Toggles volume of trading field"));

    timerDelaySpin =  new QSpinBox( this, "timer spin" );
    QWhatsThis::add( timerDelaySpin , tr( "How often stocks prices should be looked up. In minutes" ) );
    timerDelaySpin->setMaxValue( 60);

    cfg.setGroup("Timer");
    timerDelaySpin->setValue( cfg.readNumEntry("Delay",0));
    layout->addMultiCellWidget( timerDelaySpin , 6, 6, 0, 0);
    
    QLabel *label2;
    label2 = new QLabel(this);
    label2->setText( tr("Minutes between lookups."));
    label2->setMaximumHeight(60);
    layout->addMultiCellWidget( label2, 6, 6, 1, 2);

    scrollSpeed = new QSpinBox( this, "Scrollspin" );
    QWhatsThis::add( timerDelaySpin , tr( "Speed of scrolling action, in milliseconds" ) );
    scrollSpeed->setMaxValue( 1000);
    scrollSpeed->setSteps(50,50);
    cfg.setGroup("Timer");
    scrollSpeed->setValue( cfg.readNumEntry("ScrollSpeed",50));
    layout->addMultiCellWidget( scrollSpeed , 7, 7, 0, 0);
    
    QLabel *label3;
    label3 = new QLabel(this);
    label3->setText( tr("Scroll Speed, in milliseconds"));
    label3->setMaximumHeight(60);
    layout->addMultiCellWidget( label3, 7, 7, 1, 2);

    scrollLength = new QSpinBox( this, "ScrollLength" );
    QWhatsThis::add( timerDelaySpin , tr( "Length of scrolling" ) );
    scrollLength->setMaxValue( 10);
//    scrollLength->setSteps(5,5);
    cfg.setGroup("Timer");
    scrollLength->setValue( cfg.readNumEntry("ScrollLength",1));
    layout->addMultiCellWidget( scrollLength , 8, 8, 0, 0);
    
    QLabel *label4;
    label4 = new QLabel(this);
    label4->setText( tr("Scroll Length"));
    label4->setMaximumHeight(60);
    layout->addMultiCellWidget( label4, 8, 8, 1, 2);
    
//      lookupButton = new QPushButton(this, "LookupButton");
//      lookupButton->setText(tr("Symbol Lookup"));
//      connect(lookupButton,SIGNAL(clicked()),SLOT( doLookup()));
//      layout->addMultiCellWidget( lookupButton , 9, 9, 0, 0);
    
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
    layout->addItem( spacer, 9, 0 );

}


void StocktickerPluginConfig::writeConfig() {
    Config cfg( "stockticker");
    cfg.setGroup( "Symbols" );
    QString outText = text().upper();
    outText.stripWhiteSpace();
    cfg.writeEntry("Symbols",  outText );
    cfg.setGroup( "Fields" );
    cfg.writeEntry("timeCheck",timeCheck->isChecked());
    cfg.writeEntry("dateCheck",dateCheck->isChecked());
    cfg.writeEntry("symbolCheck",symbolCheck->isChecked());
    cfg.writeEntry("nameCheck",nameCheck->isChecked());
    cfg.writeEntry("currentPriceCheck",currentPriceCheck->isChecked());
    cfg.writeEntry("lastPriceCheck",lastPriceCheck->isChecked());
    cfg.writeEntry("openPriceCheck",openPriceCheck->isChecked());
    cfg.writeEntry("minPriceCheck",minPriceCheck->isChecked());
    cfg.writeEntry("maxPriceCheck",maxPriceCheck->isChecked());
    cfg.writeEntry("variationCheck",variationCheck->isChecked());
    cfg.writeEntry("volumeCheck",volumeCheck->isChecked());

    cfg.setGroup("Timer");
    cfg.writeEntry("Delay",timerDelaySpin->value());
    cfg.writeEntry("ScrollLength",scrollLength->value());
    cfg.writeEntry("ScrollSpeed",scrollSpeed->value());
      
    cfg.write();
}

StocktickerPluginConfig::~StocktickerPluginConfig() {
}

QString StocktickerPluginConfig::text() const {
   return LineEdit1->text();
}

void StocktickerPluginConfig::doLookup() {

    system("stockticker");
}
