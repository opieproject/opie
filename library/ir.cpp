#include "ir.h"

#include <qstring.h>
#include "qcopenvelope_qws.h"
#include <qcopchannel_qws.h>
#include "applnk.h"

/*! 
  \class Ir ir.h
  \brief The Ir class implements basic support for sending objects over an
  infrared communication link.
  
  The Ir class is a small helper class to enable sending of objects over an infrared communication link.
*/

/*!
  Constructs an Ir object. The \a parent and \a name classes are the
  standard QObject parameters.
*/
Ir::Ir( QObject *parent, const char *name )
    : QObject( parent, name )
{
    ch = new QCopChannel( "QPE/Obex" );
    connect( ch, SIGNAL(received(const QCString &, const QByteArray &)),
	     this, SLOT(obexMessage( const QCString &, const QByteArray &)) );
}

/*! 
  Returns TRUE if the system supports infrared communication.
*/
bool Ir::supported()
{
    return QCopChannel::isRegistered( "QPE/Obex" );
}

/*!
  Send the object in the file \a fn over the infrared link.
  The \a description will be shown to the user while
  sending is in progress.
  The optional \a mimetype parameter specifies the mimetype of the object. If this parameter is not
  set, it will be determined by the extension of the filename.
*/
void Ir::send( const QString &fn, const QString &description, const QString &mimetype)
{
    if ( !filename.isEmpty() ) return;
    filename = fn;
    QCopEnvelope e("QPE/Obex", "send(QString,QString,QString)");
    e << description << filename << mimetype;
}

/*!
  \overload

  Uses the DocLnk::file() and DocLnk::type() of \a doc.
*/
void Ir::send( const DocLnk &doc, const QString &description )
{
    send( doc.file(), description, doc.type() );
}

/*! 
  \fn Ir::done( Ir *ir );
  
  This signal is emitted by \a ir, when the send comand has been processed.
*/

/*!\internal
 */
void Ir::obexMessage( const QCString &msg, const QByteArray &data)
{
    if ( msg == "done(QString)" ) {
	QString fn;
	QDataStream stream( data, IO_ReadOnly );
	stream >> fn;
	if ( fn == filename )
	    emit done( this );
    }
}
    
