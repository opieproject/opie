
#include "obexdialog.h"
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qfileinfo.h>

#include <qpe/resource.h>

#include <opie/oprocess.h>
#include <opie/ofiledialog.h>

ObexDialog::ObexDialog( QWidget* parent,  const char* name, bool modal, WFlags fl, const QString& device )
    : QDialog( parent, name, modal, fl ) {

    if ( !name )
	setName( "ObexDialog" );
    setCaption( tr( "beam files " ) ) ;

    m_device = device;

    layout = new QVBoxLayout( this );

    QLabel* info = new QLabel( this );
    info->setText( tr("Which file should be beamed?") );

    cmdLine = new QLineEdit( this );

    QPushButton *browserButton;
    browserButton = new QPushButton( Resource::loadIconSet("fileopen"),"",this,"BrowseButton");
    connect( browserButton, SIGNAL(released() ), this , SLOT(browse() ) );

    chNameLine = new QLineEdit( this );

    sendButton = new QPushButton( this );
    sendButton->setText( tr( "Send" ) );

    layout->addWidget(info);
    layout->addWidget(cmdLine);
    layout->addWidget(browserButton);
    layout->addWidget(chNameLine);
    layout->addWidget(sendButton);

    connect( sendButton, SIGNAL( clicked() ), this,  SLOT( sendData()  ) );

}

ObexDialog::~ObexDialog() {
}

void ObexDialog::browse() {

    MimeTypes types;
    QStringList all;
    all << "*/*";
    types.insert("All Files", all );

    QString str = OFileDialog::getOpenFileName( 1,"/","", types, 0 );
    cmdLine->setText( str );

}

void ObexDialog::sendData() {
    QString fileURL = cmdLine->text();
    QString file = QFileInfo( fileURL ).fileName();
    QString modifiedName = chNameLine->text();

       // vom popupmenu beziehen
    OProcess* obexSend = new OProcess();
    if ( !modifiedName.isEmpty() ) {
        *obexSend << "ussp-push" << m_device << fileURL << modifiedName;
    } else {
        *obexSend << "ussp-push" << m_device << fileURL << file;
    }
    if (!obexSend->start(OProcess::DontCare, OProcess::AllOutput) ) {
        qWarning("could not start");
        delete obexSend;
    }



}
