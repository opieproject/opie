
#include "pppdialog.h"
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <opie/oprocess.h>

using namespace OpieTooth;

PPPDialog::PPPDialog( QWidget* parent,  const char* name, bool modal, WFlags fl, const QString& device )
    : QDialog( parent, name, modal, fl ) {

    if ( !name )
	setName( "PPPDialog" );
    setCaption( tr( "ppp connection " ) ) ;

    m_device = device;

    layout = new QVBoxLayout( this );

    QLabel* info = new QLabel( this );
    info->setText( tr("Enter an ppp script name:") );

    cmdLine = new QLineEdit( this );

    outPut = new QMultiLineEdit( this );
    QFont outPut_font(  outPut->font() );
    outPut_font.setPointSize( 8 );
    outPut->setFont( outPut_font );
    outPut->setWordWrap( QMultiLineEdit::WidgetWidth );

    connectButton = new QPushButton( this );
    connectButton->setText( tr( "Connect" ) );

    layout->addWidget(info);
    layout->addWidget(cmdLine);
    layout->addWidget(outPut);
    layout->addWidget(connectButton);

    connect( connectButton, SIGNAL( clicked() ), this,  SLOT( connectToDevice()  ) );

}

PPPDialog::~PPPDialog() {
}

void PPPDialog::connectToDevice() {
    outPut->clear();
    // vom popupmenu beziehen
    QString connectScript = "/etc/ppp/peers/" + cmdLine->text();
    OProcess* pppDial = new OProcess();
    *pppDial << "pppd" << m_device << "call" << connectScript;
    connect( pppDial, SIGNAL(receivedStdout(OProcess*, char*, int ) ),
                this, SLOT(fillOutPut(OProcess*, char*, int ) ) );
     if (!pppDial->start(OProcess::DontCare, OProcess::AllOutput) ) {
        qWarning("could not start");
        delete pppDial;
    }
}

void PPPDialog::fillOutPut( OProcess* pppDial, char* cha, int len ) {
    QCString str(cha, len );
    outPut->insertLine( str );
    delete pppDial;
}

