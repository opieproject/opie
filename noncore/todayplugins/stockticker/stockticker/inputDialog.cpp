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
#include "helpwindow.h"

#include <opie/oprocess.h>

#include <stdlib.h>
// #include <sys/stat.h>
// #include <unistd.h>

InputDialog::InputDialog( )
 : QMainWindow( 0x0, 0x0, WStyle_ContextHelp ) {
    setCaption( tr("Symbol Lookup"));

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing(6);
    layout->setMargin( 2);

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setFocus();
    
    layout->addMultiCellWidget(  LineEdit1, 0, 0, 0, 3);
    
    QLabel *label;
    label = new QLabel(this);
    label->setText( tr("Enter something to lookup / search."));
    label->setMaximumHeight(60);
    layout->addMultiCellWidget( label, 1, 1, 0, 3);

    connect(LineEdit1,SIGNAL(returnPressed()),this,SLOT(doLookup()));
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
    layout->addItem( spacer, 8, 0 );

}

InputDialog::~InputDialog() {
}

void InputDialog::doLookup() {
// http://finance.yahoo.com/l?m=&s=siemens&t=

    QString url = "\"http://finance.yahoo.com/l?m=&s="+LineEdit1->text()+"\"";
    QString tempHtml = "/tmp/stockticker.html";
     QString cmd = "wget -O "+tempHtml+" "+url;
     qDebug(cmd);


/*
      OProcess proc;
      proc << "/usr/bin/wget";
      proc<<"-O"<< tempHtml<< url;
    
      connect( &proc, SIGNAL( processExited( OProcess *)),this, SLOT( showBrowser(OProcess *)));
      proc.start( OProcess::NotifyOnExit);
*/
     system(cmd.latin1());
     HelpWindow *StockLookup = new HelpWindow( tempHtml,".",this, "SymbolLookup");
     StockLookup->setCaption("Symbol");
     StockLookup->showMaximized();
     StockLookup->show();
     LineEdit1->text();
    
    
}

void InputDialog::showBrowser(OProcess*) {
    qDebug("BLAH");
    QString tempHtml = "/tmp/stockticker.html";
    
    HelpWindow *StockLookup = new HelpWindow( tempHtml,".",this, "SymbolLookup");
    StockLookup->setCaption("Symbol");
    StockLookup->showMaximized();
    StockLookup->show();
    LineEdit1->text();
    
}
