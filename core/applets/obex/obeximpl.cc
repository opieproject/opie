
#include <qdatastream.h>
#include <qmessagebox.h>


#include <qpe/qcom.h>
#include <qpe/applnk.h>

#include <qlabel.h>

#include "obex.h"
#include "obeximpl.h"



using namespace OpieObex;

ObexImpl::ObexImpl( )
 :  QObject() {
  // register to a channel
  qWarning( "c'tor" );
  m_obex = new Obex(this, "obex");
  m_sendgui = new ObexDlg();
  m_recvgui = new ObexInc();
  m_chan = new QCopChannel("QPE/Obex" );
  connect(m_chan, SIGNAL(received(const QCString&,  const QByteArray& ) ),
	   this, SLOT(slotMessage(const QCString&, const QByteArray&) ) );
  connect(m_obex, SIGNAL(receivedFile(const QString& ) ),
          this,  SLOT(slotReceivedFile(const QString& ) ) );
}

ObexImpl::~ObexImpl() {
  delete m_obex;
  delete m_chan;
  delete m_sendgui;
}

QRESULT ObexImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
  *iface = 0;
  if( uuid  ==  IID_QUnknown )
    *iface = this;
  else if( uuid == IID_ObexInterface )
    *iface = this;

  qWarning("query" );
  if(*iface )
    (*iface)->addRef();
  return QS_OK;
}

void ObexImpl::slotMessage( const QCString& msg, const QByteArray&data ) {
  QDataStream stream( data, IO_ReadOnly );
  qWarning("Message %s", msg.data() );
  if(msg == "send(QString,QString,QString)" ) {
    QString desc;
    stream >> desc;
    stream >> m_name;
    m_sendgui->raise(); // should be on top
    m_sendgui->showMaximized();
    m_sendgui->lblPath->setText(m_name);
    connect( (QObject*)m_sendgui->PushButton2, SIGNAL(clicked()),
             this, SLOT(slotCancelSend()));
    m_obex->send(m_name );
    connect( (QObject*)m_obex, SIGNAL( sent() ), this,
             SLOT( slotSent() ) );
    connect( (QObject*)m_obex, SIGNAL( error(int) ),  this,
             SLOT( slotSent() ) );
  }else if(msg == "receive(int)" ) { // open a GUI
      //m_recvgui->showMaximized();
      int receiveD = 0;
      stream >> receiveD;
      if ( receiveD == 1)
          m_obex->receive();
      else
          m_obex->setReceiveEnabled( false );

  } else if (msg =="done(QString)") {
      QString text;
      stream >> text;
      m_sendgui->lblPath->setText(tr("Done transfering " + text));

  }
}

void ObexImpl::slotCancelSend() {
    // cancel sync process too
    //m_obex->cancel(); // not ready yet
    m_sendgui->hide();
}

void ObexImpl::slotDone(bool) {
    QCopEnvelope e ("QPE/Obex", "done(QString)" ); //but this into a slot
    e << m_name;
}

void ObexImpl::slotSent() {
    m_sendgui->lblPath->setText("Done!");
    m_sendgui->hide();
}

void ObexImpl::slotError( int errorCode) {

    QString errorString = "";
    if (errorCode == -1) {
        errorString = "test";
    }
    qDebug("Error: " + errorString);
    m_sendgui->hide();
}

// Received a file via beam
// check for mime  type and then either
// add to App via setDocument
void ObexImpl::slotReceivedFile( const QString &fileName ) {
    qWarning("filename %s",  fileName.latin1() );
    DocLnk lnk( fileName );
    QString exec = lnk.exec();
    qWarning("executing %s", exec.latin1() );
    if ( exec.isEmpty() || exec == "" ) {
        qWarning("empty");
        if ( fileName.right(4) == ".vcf" )
            exec = "addressbook";
        else if ( fileName.right(4) == ".vcs" ) {
            exec = "datebook";
        }
    } // now prompt and then add it

    m_recvgui->PixmapLabel->setPixmap(lnk.pixmap());
    m_recvgui->TextLabel1_2->setText(lnk.name());
    m_recvgui->showMaximized();

    QCString str= "QPE/Application/";
    str += exec.latin1();
    qWarning("channel %s", str.data() );
    QCopEnvelope e(str ,  "setDocument(QString)" );
    e << fileName;

}


Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( ObexImpl )
}
