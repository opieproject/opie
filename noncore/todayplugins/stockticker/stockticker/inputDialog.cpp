#include "inputDialog.h"

#include <qapplication.h>

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


InputDialog::InputDialog( )
 : QMainWindow( 0x0, 0x0, WStyle_ContextHelp ) {
    setCaption( tr("Enter Stock Symbols"));

    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing(6);
    layout->setMargin( 2);

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setFocus();
    QWhatsThis::add( LineEdit1, tr("Enter the stock symbols you want to be shown here."));
    
    layout->addMultiCellWidget(  LineEdit1, 0, 0, 0, 3);
    
    Config cfg( "stockticker");
    cfg.setGroup( "Symbols" );
    QString symbollist;
    symbollist = cfg.readEntry("Symbols", "");
    LineEdit1->setText(symbollist);
    
    QLabel *label;
    label = new QLabel(this);
    label->setText( tr("Enter stock symbols seperated\nby a space."));
    label->setMaximumHeight(60);
    layout->addMultiCellWidget( label, 1, 1, 0, 3);

    cfg.setGroup( "Fields" );

    timeCheck= new QCheckBox ( "Time",this );
    timeCheck->setChecked( cfg.readBoolEntry("timeCheck",1));
    layout->addMultiCellWidget(timeCheck, 2, 2, 0, 0 );
    QWhatsThis::add( timeCheck, tr("Toggles Time Field"));
    
    dateCheck= new QCheckBox ( "Date", this );
    dateCheck->setChecked( cfg.readBoolEntry("dateCheck",1));
    layout->addMultiCellWidget( dateCheck, 2, 2, 1, 1 );
    QWhatsThis::add(dateCheck, tr("Toggles date field"));
    
    symbolCheck= new QCheckBox ( "Symbol", this );
    symbolCheck->setChecked( cfg.readBoolEntry("symbolCheck",1));
    layout->addMultiCellWidget( symbolCheck, 3, 3, 0, 0 );
    QWhatsThis::add(symbolCheck, tr("Toggles Symbol field"));

    nameCheck= new QCheckBox ( "Name", this );
    nameCheck->setChecked( cfg.readBoolEntry("nameCheck",1));
    layout->addMultiCellWidget( nameCheck, 3, 3, 1, 1 );
    QWhatsThis::add(nameCheck, tr("Toggles Name field"));

    currentPriceCheck= new QCheckBox ( "Current Price", this );
    currentPriceCheck->setChecked( cfg.readBoolEntry("currentPriceCheck",1));
    layout->addMultiCellWidget( currentPriceCheck, 4, 4, 0, 0 );
    QWhatsThis::add(currentPriceCheck, tr("Toggles current Price field"));
  

    lastPriceCheck= new QCheckBox ( "Last Price", this );
    lastPriceCheck->setChecked( cfg.readBoolEntry("lastPriceCheck",1));
    layout->addMultiCellWidget(lastPriceCheck, 4, 4, 1, 1);
    QWhatsThis::add(lastPriceCheck, tr("Toggles last price field"));

    openPriceCheck= new QCheckBox ( "Open Price", this);
    openPriceCheck->setChecked( cfg.readBoolEntry("openPriceCheck",1));
    layout->addMultiCellWidget( openPriceCheck, 5, 5, 0, 0 );
    QWhatsThis::add(openPriceCheck, tr("Toggles opening price field"));

    minPriceCheck= new QCheckBox ( "Min Price", this );
    minPriceCheck->setChecked( cfg.readBoolEntry("minPriceCheck",1));
    layout->addMultiCellWidget( minPriceCheck, 5, 5, 1, 1);
    QWhatsThis::add(minPriceCheck, tr("Toggles minamum price field"));

    maxPriceCheck= new QCheckBox ( "Max Price", this);
    maxPriceCheck->setChecked( cfg.readBoolEntry("maxPriceCheck",1));
    layout->addMultiCellWidget( maxPriceCheck, 6, 6, 0, 0 );
    QWhatsThis::add(maxPriceCheck, tr("Toggles maximum price field"));

    variationCheck= new QCheckBox ( "Variation", this );
    variationCheck->setChecked( cfg.readBoolEntry("variationCheck",1));
    layout->addMultiCellWidget( variationCheck, 6, 6, 1, 1 );
    QWhatsThis::add(variationCheck, tr("Toggles daily variation field"));

    volumeCheck= new QCheckBox ( "Volume", this );
    volumeCheck->setChecked( cfg.readBoolEntry("volumeCheck",1));
    layout->addMultiCellWidget( volumeCheck , 7, 7, 0, 0);
    QWhatsThis::add(volumeCheck, tr("Toggles volume field"));

 
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
    layout->addItem( spacer, 8, 0 );

}

QString InputDialog::text() const {
   return LineEdit1->text();
}

InputDialog::~InputDialog() {
}

void InputDialog::cleanUp(){
    Config cfg( "stockticker");
    cfg.setGroup( "Symbols" );
    QString outText = text().upper();
    outText.stripWhiteSpace();
    cfg.writeEntry("Symbols",  outText );
    qDebug( "<<<<<<<<<<<>>>>>>>>>>>>"+text());

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
          
      
    cfg.write();
}


