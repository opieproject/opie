
#include <qdatastream.h>
#include <qpe/qcom.h>
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
    QString filename;
    stream >> filename;
    m_sendgui->showMaximized();
    m_sendgui->lblPath->setText(filename);
    connect( (QObject*)m_sendgui->PushButton2, SIGNAL(clicked()),
             this, SLOT(slotCancelSend()));
    m_obex->send(filename );
    connect( (QObject*)m_obex, SIGNAL( sent() ), this,
             SLOT( slotSent() ) );
  }else if(msg == "receive(bool)" ) { // open a GUI
      m_recvgui->showMaximized();
      m_obex->receive();

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
    e << "Done!";
}

void ObexImpl::slotSent() {
    m_sendgui->lblPath->setText("Done!");
    m_sendgui->hide();
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( ObexImpl )
}
