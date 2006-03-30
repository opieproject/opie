
#include "pppdialog.h"
#include "rfcommhelper.h"
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <opie2/oprocess.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

using namespace OpieTooth;

using namespace Opie::Core;

Connection PPPDialog::conns[NCONNECTS];

PPPDialog::PPPDialog( const QString& device, int port, QWidget* parent,  
    const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
    int i; //Just an index variable

    if ( !name )
        setName( "PPPDialog" );
    setCaption( tr( "ppp connection " ) ) ;

    m_device = device;
    m_port = port;

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
    
    serPort = new QComboBox(this);
    for (i = 0; i < NCONNECTS; i++) {
        if (!PPPDialog::conns[i].proc.isRunning())
            serPort->insertItem(tr("rfcomm%1").arg(i));
    }
    
    layout->addWidget(info);
    layout->addWidget(cmdLine);
    layout->addWidget(serPort);
    layout->addWidget(outPut);
    layout->addWidget(connectButton);

    connect( connectButton, SIGNAL( clicked() ), this,  SLOT( connectToDevice() ) );
}

PPPDialog::~PPPDialog() {
}

void PPPDialog::connectToDevice() {
    int portNum = serPort->currentText().right(1).toInt();
    if (PPPDialog::conns[portNum].proc.isRunning()) {
        outPut->append(tr("Work in progress"));
        return;
    }
    outPut->clear();
    PPPDialog::conns[portNum].proc.clearArguments();
    // vom popupmenu beziehen
    if (cmdLine->text().isEmpty()) {//Connect by rfcomm
        PPPDialog::conns[portNum].proc << "rfcomm" << "connect" 
            << QString::number(portNum) << m_device << QString::number(m_port);
    }
    else {
        PPPDialog::conns[portNum].proc << "pppd" 
            << tr("/dev/bluetooth/rfcomm/%1").arg(portNum) 
            << "call" 
            << cmdLine->text();
    }
    if (!PPPDialog::conns[portNum].proc.start(OProcess::NotifyOnExit, 
        OProcess::All)) {
        outPut->append(tr("Couldn't start"));
    }
    else
    {
        PPPDialog::conns[portNum].proc.resume();
        outPut->append(tr("Started"));
        PPPDialog::conns[portNum].btAddr = m_device;
        PPPDialog::conns[portNum].port = m_port;
        connect(&PPPDialog::conns[portNum].proc, 
		    SIGNAL(receivedStdout(Opie::Core::OProcess*, char*, int)),
            this, SLOT(fillOutPut(Opie::Core::OProcess*, char*, int)));
        connect( &PPPDialog::conns[portNum].proc, 
            SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int)),
            this,    SLOT(fillErr(Opie::Core::OProcess*, char*, int)));
        connect( &PPPDialog::conns[portNum].proc, 
            SIGNAL(processExited(Opie::Core::OProcess*)),
            this, SLOT(slotProcessExited(Opie::Core::OProcess*)));
    }
}

void PPPDialog::fillOutPut( OProcess*, char* cha, int len ) {
    QCString str(cha, len);
    outPut->append(str);
}

void PPPDialog::fillErr(OProcess*, char* buf, int len)
{
    QCString str(buf, len);
    outPut->append(str);
}

void PPPDialog::slotProcessExited(OProcess* proc) {
    if (proc->normalExit()) {
        outPut->append( tr("Finished with result ") );
        outPut->append( QString::number(proc->exitStatus()) );
    }
    else
        outPut->append( tr("Exited abnormally") );
}

void PPPDialog::closeEvent(QCloseEvent* e)
{
    int i; //index variable
    for (i = 0; i < NCONNECTS; i++) {
        if(PPPDialog::conns[i].proc.isRunning())
            PPPDialog::conns[i].proc.kill();
    }
    QDialog::closeEvent(e);
}

//eof
