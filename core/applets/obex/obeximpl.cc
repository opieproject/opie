

#include "obex.h"
#include "obeximpl.h"

using namespace OpieObex;

ObexImpl::ObexImpl( )
 :  QObject {
  // register to a channel
  m_obex = new Obex(this, "obex");
  m_chan = new QCopChannel("QPE/Obex" );
  cconnect(m_chan, SIGNAL(received(const QCString&,  const QByteArray& ) ),
	   this, SLOT(slotMessage(const QCString&, const QByteArray&) ) );
}
ObexImpl::~ObexImpl() {
  delete m_obex;
  delete m_chan;
}
QRESULT ObexImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
  *iface = 0;
  if( uuid  ==  IID_QUknown )
    *iface = this;
  else if( uuid == IID_ObexInterface )
    *iface = this;

  if(*iface )
    (*iface)->addRef();
  return QS_OK;

}
void ObexImpl::slotMessage( const QCString& msg, const QByteArray&data ) {
  QDataStrean stream( data, IO_ReadOnly );
  if(msg == "send(QString,QString,QString)" ) {
    QString desc;
    stream >> desc;
    QString filename;
    stream >> filename;
    m_obex->send(filename );
    QCopEnvelope e ("QPE/Obex", "done(QString)" ):
      e << filename;
  }else if( ) {

  }
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( IrdaAppletImpl )
}
