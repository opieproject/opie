
#include "obexdialog.h"
#include <errno.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qfileinfo.h>

#include <qpe/resource.h>

#include <opie2/oprocess.h>
#include <opie2/ofiledialog.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

using namespace OpieTooth;

using namespace Opie::Core;
using namespace Opie::Ui;
using namespace Opie::Core;
ObexDialog::ObexDialog(const QString& device, QWidget* parent,  const char* name, bool modal, WFlags fl)
    : QDialog( parent, name, modal, fl ) {

    if ( !name )
        setName( "ObexDialog" );
    setCaption( tr( "beam files " ) ) ;

    m_device = device;

    layout = new QVBoxLayout( this );
    obexSend  = new OProcess();

    info = new QLabel( this );
    info->setText( tr("Which file should be beamed?") );

    statLine = new QLabel(this);
    statLine->setText( tr("Ready") );
  
    status = new QMultiLineEdit(this);
    status->setReadOnly(true);
    
    cmdLine = new QLineEdit( this );

    QPushButton *browserButton;
    browserButton = new QPushButton( Resource::loadIconSet("fileopen"),"",this,"BrowseButton");
    connect( browserButton, SIGNAL(released() ), this , SLOT(browse() ) );

    chNameLine = new QLineEdit( this );

    sendButton = new QPushButton( this );
    sendButton->setText( tr( "Send" ) );

    layout->addWidget(info);
    layout->addWidget(status);
    layout->addWidget(cmdLine);
    layout->addWidget(browserButton);
    layout->addWidget(chNameLine);
    layout->addWidget(sendButton);
    layout->addWidget(statLine);

    connect( sendButton, SIGNAL( clicked() ), this,  SLOT( sendData() ) );

    connect(obexSend, SIGNAL(processExited(Opie::Core::OProcess*)),
        this, SLOT(slotProcessExited(Opie::Core::OProcess*)));
    connect(obexSend, SIGNAL(receivedStdout(Opie::Core::OProcess*, char*, int)),
        this, SLOT(slotPushOut(Opie::Core::OProcess*, char*, int)));
    connect(obexSend, SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int)),
        this, SLOT(slotPushErr(Opie::Core::OProcess*, char*, int)));

}

ObexDialog::~ObexDialog() {
  if (obexSend->isRunning())
    obexSend->kill();
  delete obexSend;
  obexSend = NULL;
}

void ObexDialog::browse() {

    MimeTypes types;
    QStringList all;
    all << "*/*";
    types.insert("All Files", all );

    QString str = OFileDialog::getOpenFileName( 1,"/","", types, 0 );
    cmdLine->setText( str );
    statLine->setText( tr("Ready") );
}

void ObexDialog::sendData() {
    QString fileURL = cmdLine->text();
    QString file = QFileInfo( fileURL ).fileName();
    QString modifiedName = chNameLine->text();
    QString execName = "ussp-push";

    if (obexSend->isRunning())
        return;
    obexSend->clearArguments();
       // vom popupmenu beziehen
    if ( !modifiedName.isEmpty() ) {
        *obexSend << execName << "--timeo 30" << m_device << fileURL << modifiedName;
    } else {
        *obexSend << execName << "--timeo 30" << m_device << fileURL << file;
    }
    obexSend->setUseShell(true);
    if (!obexSend->start(OProcess::NotifyOnExit, OProcess::All) ) {
		    statLine->setText( tr("Error: couln't start process") );
    }
		else
	    statLine->setText( tr("Sending") );
}

void ObexDialog::slotPushOut(OProcess*, char* buf, int len) {
  QCString str(buf, len);
  status->append(str);
}

void ObexDialog::slotPushErr(OProcess*, char* buf, int len) {
  QCString str(buf, len);
  status->append(str);
}

void ObexDialog::slotProcessExited(OProcess*) {
  if (obexSend == NULL)
      return;
  if (obexSend->normalExit()) {
    status->append( tr("Finished with result ") );
    status->append( QString::number(obexSend->exitStatus()) );
    status->append( tr("\n") );
    odebug << obexSend->exitStatus() << oendl;
    statLine->setText( tr("Finished: ") + tr(strerror(obexSend->exitStatus())) );
  }
  else {
    status->append( tr("Exited abnormally\n") );
    statLine->setText( tr("Exited abnormally") );
  }
}
