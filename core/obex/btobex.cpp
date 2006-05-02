
#include "btobex.h"
#include <manager.h>
#include <services.h>

/* OPIE */
#include <opie2/oprocess.h>
#include <opie2/odebug.h>

/* QT */
#include <qfileinfo.h>
#include <qstring.h>
#include <qmap.h>
#include <qmessagebox.h>

using namespace  OpieObex;

using namespace Opie::Core;
/* TRANSLATOR OpieObex::Obex */
using namespace OpieTooth;

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
    btManager = NULL;
};

BtObex::~BtObex() {
    if (btManager)
        delete btManager;
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
    //Now we need to find out if the OBEX push is supported for this device
    //And get the port number
    if (!btManager) {
        btManager = new Manager("hci0");
        connect(btManager,
            SIGNAL(foundServices(const QString&, Services::ValueList)),
            this, SLOT(slotFoundServices(const QString&, Services::ValueList)));
    }
    btManager->searchServices(bdaddr);
}

/**
 * This function reacts on the service discovery finish
 */
void BtObex::slotFoundServices(const QString&, Services::ValueList svcList)
{
    QValueList<OpieTooth::Services>::Iterator it; 
    QMap<int, QString> classList; //The classes list
    QMap<int, QString>::Iterator classIt; //Iterator in the class list
    int portNum = -1; //The desired port number
    odebug << "BtObex slotFoundServices" << oendl;
    if (svcList.isEmpty()) {
        QMessageBox::critical(NULL, tr("Object send"), tr("No services found"));
        emit error(-1);
        return;    
    }
    for (it = svcList.begin(); it != svcList.end(); it++) {
        classList = (*it).classIdList();
        classIt = classList.begin();
        if (classIt == classList.end())
            continue;
////We really need symbolic names for service IDs
        //Ok, we have found the object push service
        if (classIt.key() == 4357) { 
            portNum = (*it).protocolDescriptorList().last().port();
            break;
        }
    }
    if (portNum == -1) {
        QMessageBox::critical(NULL, tr("Object send"), 
            tr("No OBEX Push service"));
        emit error(-1);
        return;
    }
    m_port = portNum;
    sendNow();
}

void BtObex::sendNow(){
    if ( m_count >= 25 ) { // could not send
        emit error(-1 );
        emit sent(false);
        return;
    }
    // OProcess inititialisation
    m_send = new OProcess(0, "ussp-push");
    m_send->setWorkingDirectory( QFileInfo(m_file).dirPath(true) );

    // ussp-push --timeo 30 <btaddr:port> file file
    *m_send << "ussp-push" << "--timeo 30";
    *m_send << m_bdaddr + "@" + QString::number(m_port);
    *m_send << QFile::encodeName(QFileInfo(m_file).fileName());
    *m_send << QFile::encodeName(QFileInfo(m_file).fileName());
    m_send->setUseShell(true);
    
    // connect to slots Exited and and StdOut
    connect(m_send,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this, SLOT(slotExited(Opie::Core::OProcess*)) );
    connect(m_send,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int)),
            this, SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );
    // now start it
    if (!m_send->start(OProcess::NotifyOnExit,  OProcess::AllOutput) ) {
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
    odebug << proc->name() << " exited with result " 
           << proc->exitStatus() << oendl;
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
    }else if (m_send->exitStatus() != 0 ) { // it failed maybe the other side wasn't ready
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
QString BtObex::parseOut(){
  QString path;
  QStringList list = QStringList::split("\n",  m_outp);
  QStringList::Iterator it;
  for (it = list.begin(); it != list.end(); ++it ) {
    odebug << (*it) << oendl;
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
