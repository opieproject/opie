
#include "obex.h"

/* OPIE */
#include <opie2/oprocess.h>
#include <opie2/odebug.h>

/* QT */
#include <qfileinfo.h>



using namespace  OpieObex;

using namespace Opie::Core;
/* TRANSLATOR OpieObex::Obex */

Obex::Obex( QObject *parent, const char* name )
  : QObject(parent, name )
{
    m_rec = 0;
    m_send=0;
    m_count = 0;
    m_receive = false;
    connect( this, SIGNAL(error(int) ), // for recovering to receive
             SLOT(slotError() ) );
    connect( this, SIGNAL(sent(bool) ),
             SLOT(slotError() ) );
};
Obex::~Obex() {
    delete m_rec;
    delete m_send;
}
void Obex::receive()  {
    m_receive = true;
    m_outp = QString::null;
    owarn << "Receive" << oendl;
    m_rec = new OProcess();
    *m_rec << "irobex_palm3";
    // connect to the necessary slots
    connect(m_rec,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this,  SLOT(slotExited(Opie::Core::OProcess*) ) );

    connect(m_rec,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int ) ),
            this,  SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );

    if(!m_rec->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        owarn << "could not start :(" oendl;
        emit done( false );
        delete m_rec;
        m_rec = 0;
    }
//    emit currentTry(m_count );

}
void Obex::send( const QString& fileName) { // if currently receiving stop it send receive
    m_count = 0;
    m_file = fileName;
    owarn << "send " << fileName.latin1() << oendl;
    if (m_rec != 0 ) {
        owarn << "running" oendl;
        if (m_rec->isRunning() ) {
            emit error(-1 );
            owarn << "is running" << oendl;
            delete m_rec;
            m_rec = 0;

        }else{
            owarn << "is not running" << oendl;
            emit error( -1 ); // we did not delete yet but it's not running slotExited is pending
            return;
        }
    }
    sendNow();
}
void Obex::sendNow(){
    owarn << "sendNow" << oendl;
    if ( m_count >= 25 ) { // could not send
        emit error(-1 );
        emit sent(false);
        return;
    }
    // OProcess inititialisation
    m_send = new OProcess();
    *m_send << "irobex_palm3";
    *m_send << QFile::encodeName(m_file);

    // connect to slots Exited and and StdOut
    connect(m_send,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this, SLOT(slotExited(Opie::Core::OProcess*)) );
    connect(m_send,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int )),
            this, SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );

    // now start it
    if (!m_send->start(/*OProcess::NotifyOnExit,  OProcess::AllOutput*/ ) ) {
        owarn << "could not send" << oendl;
        m_count = 25;
        emit error(-1 );
        delete m_send;
        m_send=0;
    }
    // end
    m_count++;
    emit currentTry( m_count );
}

void Obex::slotExited(OProcess* proc ){
    if (proc == m_rec ) { // receive process
        received();
    }else if ( proc == m_send ) {
        sendEnd();
    }
}
void Obex::slotStdOut(OProcess* proc, char* buf, int len){
    if ( proc == m_rec ) { // only receive
        QByteArray ar( len  );
        memcpy( ar.data(), buf, len );
        owarn << "parsed: " << ar.data() << oendl;
        m_outp.append( ar );
    }
}

void Obex::received() {
  if (m_rec->normalExit() ) {
      if ( m_rec->exitStatus() == 0 ) { // we got one
          QString filename = parseOut();
          owarn << "ACHTUNG " << filename.latin1() << oendl;
          emit receivedFile( filename );
      }
  }else{
      emit done(false);
  };
  delete m_rec;
  m_rec = 0;
  receive();
}

void Obex::sendEnd() {
  if (m_send->normalExit() ) {
    if ( m_send->exitStatus() == 0 ) {
      delete m_send;
      m_send=0;
      owarn << "done" << oendl;
      emit sent(true);
    }else if (m_send->exitStatus() == 255 ) { // it failed maybe the other side wasn't ready
      // let's try it again
      delete m_send;
      m_send = 0;
      owarn << "try sending again" << oendl;
      sendNow();
    }
  }else {
    emit error( -1 );
    delete m_send;
    m_send = 0;
  }
}
QString Obex::parseOut(     ){
  QString path;
  QStringList list = QStringList::split("\n",  m_outp);
  QStringList::Iterator it;
  for (it = list.begin(); it != list.end(); ++it ) {
    if ( (*it).startsWith("Wrote"  ) ) {
        int pos = (*it).findRev('(' );
        if ( pos > 0 ) {
            owarn << pos << " " << (*it).mid(6 ).latin1() << oendl;
            owarn << (*it).length() << " " << (*it).length()-pos << oendl;

            path = (*it).remove( pos, (*it).length() - pos );
            owarn << path.latin1() << oendl;
            path = path.mid(6 );
            path = path.stripWhiteSpace();
            owarn << "path " << path.latin1() << oendl;
        }
    }
  }
  return path;
}
/**
 * when sent is done slotError is called we  will start receive again
 */
void Obex::slotError() {
    owarn << "slotError" << oendl;
    if ( m_receive )
        receive();
};
void Obex::setReceiveEnabled( bool receive ) {
    if ( !receive ) { //
        m_receive = false;
        shutDownReceive();
    }
}

void Obex::shutDownReceive() {
    if (m_rec != 0 ) {
        owarn << "running" << oendl;
        if (m_rec->isRunning() ) {
            emit error(-1 );
            owarn << "is running" << oendl;
            delete m_rec;
            m_rec = 0;
        }
    }

}
