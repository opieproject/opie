
#include <qdatastream.h>
#include <qpe/qcom.h>

#include "obex.h"
#include "obeximpl.h"



using namespace OpieObex;

ObexImpl::ObexImpl( )
 :  QObject() {
  // register to a channel
  qWarning( "c'tor" );
  m_obex = new Obex(this, "obex");
  m_chan = new QCopChannel("QPE/Obex" );
  connect(m_chan, SIGNAL(received(const QCString&,  const QByteArray& ) ),
	   this, SLOT(slotMessage(const QCString&, const QByteArray&) ) );
}
ObexImpl::~ObexImpl() {
  delete m_obex;
  delete m_chan;
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
    m_obex->send(filename );
    QCopEnvelope e ("QPE/Obex", "done(QString)" );
    e << filename;
  }else if(msg == "receive(bool)" ) {
    ;
  }
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( ObexImpl )
}
