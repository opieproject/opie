
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
ObexDialog::ObexDialog(const QString& device, int port,
    QWidget* parent,  const char* name, bool modal, WFlags fl)
    : QDialog( parent, name, modal, fl ) {

    if ( !name )
        setName( "ObexDialog" );
    setCaption( tr( "beam files " ) ) ;

    m_device = device;
    m_port = port;

    layout = new QVBoxLayout( this );
    obexSend  = new ObexPush();

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

    connect(obexSend, SIGNAL(sendComplete(int)),
        this, SLOT(slotPushComplete(int)));
    connect(obexSend, SIGNAL(sendError(int)),
        this, SLOT(slotPushError(int)));
    connect(obexSend, SIGNAL(status(QCString&)),
        this, SLOT(slotPushStatus(QCString&)));

}

ObexDialog::~ObexDialog() {
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
    int result; //function call result
    QString fileURL = cmdLine->text();
    QString modifiedName = chNameLine->text();
    result = obexSend->send(m_device, m_port, fileURL, modifiedName);
    if (result > 0)
        return;
    else if (result < 0)
		    statLine->setText( tr("Error: couln't start process") );
    else
        statLine->setText( tr("Sending") );
}

void ObexDialog::slotPushStatus(QCString& str) {
  status->append(str);
}

void ObexDialog::slotPushComplete(int result) {
  status->append( tr("Finished with result ") );
  status->append( QString::number(result) );
  status->append( tr("\n") );
  odebug << result << oendl;
  statLine->setText( tr("Finished: ") + tr(strerror(result)) );
}

void ObexDialog::slotPushError(int) {
  status->append( tr("Exited abnormally\n") );
  statLine->setText( tr("Exited abnormally") );
}
//eof
