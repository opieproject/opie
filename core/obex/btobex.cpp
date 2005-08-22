
#include "btobex.h"

/* OPIE */
#include <opie2/oprocess.h>
#include <opie2/odebug.h>

/* QT */
#include <qfileinfo.h>



using namespace  OpieObex;

using namespace Opie::Core;
/* TRANSLATOR OpieObex::Obex */

BtObex::BtObex( QObject *parent, const char* name )
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
BtObex::~BtObex() {
    delete m_rec;
    delete m_send;
}
void BtObex::receive()  {
    m_receive = true;
    m_outp = QString::null;
    m_rec = new OProcess();

    // TODO mbhaynie: No idea if this actually works -- maybe opd is better.
    *m_rec << "obexftpd" << "-b";
    // connect to the necessary slots
    connect(m_rec,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this,  SLOT(slotExited(Opie::Core::OProcess*) ) );

    connect(m_rec,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int ) ),
            this,  SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );

    if(!m_rec->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        emit done( false );
        delete m_rec;
        m_rec = 0;
    }
}

void BtObex::send( const QString& fileName, const QString& bdaddr) {
    // if currently receiving stop it send receive
    m_count = 0;
    m_file = fileName;
    m_bdaddr = bdaddr;
    if (m_rec != 0 ) {
        if (m_rec->isRunning() ) {
            emit error(-1 );
            delete m_rec;
            m_rec = 0;

        }else{
            emit error( -1 ); // we did not delete yet but it's not running slotExited is pending
            return;
        }
    }
    sendNow();
}
void BtObex::sendNow(){
    if ( m_count >= 25 ) { // could not send
        emit error(-1 );
        emit sent(false);
        return;
    }
    // OProcess inititialisation
    m_send = new OProcess();
    m_send->setWorkingDirectory( QFileInfo(m_file).dirPath(true) );

    // obextool push file <bdaddr> [channel]
    // 9 for phones.
    // Palm T3 accepts pictures on 1
    *m_send << "obextool" << "push";
    *m_send << QFile::encodeName(QFileInfo(m_file).fileName());
    *m_send << m_bdaddr << "9";

    // connect to slots Exited and and StdOut
    connect(m_send,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this, SLOT(slotExited(Opie::Core::OProcess*)) );
    connect(m_send,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int )),
            this, SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );

    // now start it
    if (!m_send->start(/*OProcess::NotifyOnExit,  OProcess::AllOutput*/ ) ) {
        m_count = 25;
        emit error(-1 );
        delete m_send;
        m_send=0;
    }
    // end
    m_count++;
    emit currentTry( m_count );
}

void BtObex::slotExited(OProcess* proc ){
    if (proc == m_rec )  // receive process
        received();
    else if ( proc == m_send ) 
        sendEnd();
    
}
void BtObex::slotStdOut(OProcess* proc, char* buf, int len){
    if ( proc == m_rec ) { // only receive
        QByteArray ar( len  );
        memcpy( ar.data(), buf, len );
        m_outp.append( ar );
    }
}

void BtObex::received() {
  if (m_rec->normalExit() ) {
      if ( m_rec->exitStatus() == 0 ) { // we got one
          QString filename = parseOut();
          emit receivedFile( filename );
      }
  }else{
      emit done(false);
  };
  delete m_rec;
  m_rec = 0;
  receive();
}

void BtObex::sendEnd() {
  if (m_send->normalExit() ) {
    if ( m_send->exitStatus() == 0 ) {
      delete m_send;
      m_send=0;
      emit sent(true);
    }else if (m_send->exitStatus() == 255 ) { // it failed maybe the other side wasn't ready
      // let's try it again
      delete m_send;
      m_send = 0;
      sendNow();
    }
  }else {
    emit error( -1 );
    delete m_send;
    m_send = 0;
  }
}

// This probably doesn't do anything useful for bt.
QString BtObex::parseOut(     ){
  QString path;
  QStringList list = QStringList::split("\n",  m_outp);
  QStringList::Iterator it;
  for (it = list.begin(); it != list.end(); ++it ) {
    if ( (*it).startsWith("Wrote"  ) ) {
        int pos = (*it).findRev('(' );
        if ( pos > 0 ) {

            path = (*it).remove( pos, (*it).length() - pos );
            path = path.mid(6 );
            path = path.stripWhiteSpace();
        }
    }
  }
  return path;
}
/**
 * when sent is done slotError is called we  will start receive again
 */
void BtObex::slotError() {
    if ( m_receive )
        receive();
};
void BtObex::setReceiveEnabled( bool receive ) {
    if ( !receive ) { //
        m_receive = false;
        shutDownReceive();
    }
}

void BtObex::shutDownReceive() {
    if (m_rec != 0 ) {
        if (m_rec->isRunning() ) {
            emit error(-1 );
            delete m_rec;
            m_rec = 0;
        }
    }

}
