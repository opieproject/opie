
#include "pandialog.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qstring.h>
#include <qmultilineedit.h>
#include <opie2/oprocess.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

using namespace OpieTooth;

using namespace Opie::Core;

PanDialog::PanDialog( const QString& device, QWidget* parent,  
    const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
    m_panConnect = NULL;
    
    if ( !name )
        setName( "PANDialog" );
    setCaption( tr( "PAN connection " ) ) ;

    m_device = device;

    layout = new QVBoxLayout( this );

    outPut = new QMultiLineEdit( this );
    QFont outPut_font(  outPut->font() );
    outPut_font.setPointSize( 8 );
    outPut->setFont( outPut_font );
    outPut->setWordWrap( QMultiLineEdit::WidgetWidth );

    connectButton = new QPushButton( this );
    connectButton->setText( tr( "Connect" ) );
    
    doEncryption = new QCheckBox(this, "encrypt");
    doEncryption->setText( tr( "encrypt" ) );
    doSecure = new QCheckBox(this, "secure connection");
    doSecure->setText( tr( "secure connection" ) );

    layout->addWidget(doEncryption);
    layout->addWidget(doSecure);
    layout->addWidget(outPut);
    layout->addWidget(connectButton);

    connect( connectButton, SIGNAL( clicked() ), this,  SLOT( connectToDevice() ) );
}

PanDialog::~PanDialog() {
}

void PanDialog::connectToDevice() {
    bool doEnc = doEncryption->isChecked();
    bool doSec = doSecure->isChecked();

    if (m_panConnect) {
        outPut->append(tr("Work in progress"));
        return;
    }
    m_panConnect = new OProcess();
    outPut->clear();
    
    // Fill process command line
    *m_panConnect << tr("pand")
            << tr("--connect") << m_device
            << tr("--nodetach");
    if (doEnc)
        *m_panConnect << tr("--encrypt");
    if (doSec)
        *m_panConnect << tr("--secure");
    if (!m_panConnect->start(OProcess::NotifyOnExit, 
        OProcess::All)) {
        outPut->append(tr("Couldn't start"));
        delete m_panConnect;
        m_panConnect = NULL;
    }
    else
    {
        m_panConnect->resume();
        outPut->append(tr("Started"));
        connect(m_panConnect, 
		    SIGNAL(receivedStdout(Opie::Core::OProcess*, char*, int)),
            this, SLOT(fillOutPut(Opie::Core::OProcess*, char*, int)));
        connect(m_panConnect, 
            SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int)),
            this,    SLOT(fillErr(Opie::Core::OProcess*, char*, int)));
        connect(m_panConnect, 
            SIGNAL(processExited(Opie::Core::OProcess*)),
            this, SLOT(slotProcessExited(Opie::Core::OProcess*)));
    }
}

void PanDialog::fillOutPut( OProcess*, char* cha, int len ) {
    QCString str(cha, len);
    outPut->append(str);
}

void PanDialog::fillErr(OProcess*, char* buf, int len)
{
    QCString str(buf, len);
    outPut->append(str);
}

void PanDialog::slotProcessExited(OProcess* proc) {
    if (m_panConnect->normalExit()) {
        outPut->append( tr("Finished with result ") );
        outPut->append( QString::number(proc->exitStatus()) );
    }
    else
        outPut->append( tr("Exited abnormally") );
    delete m_panConnect;
    m_panConnect = NULL;
}

void PanDialog::closeEvent(QCloseEvent* e)
{
    if (m_panConnect && m_panConnect->isRunning())
        m_panConnect->kill();
    QDialog::closeEvent(e);
}

//eof
