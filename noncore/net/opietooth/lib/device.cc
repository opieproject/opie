
#include "device.h"

/* OPIE */
#include <opie2/oprocess.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

/* STD */
#include <signal.h>


using namespace OpieTooth;

using Opie::Core::OProcess;
namespace {
  int parsePid( const QCString& par ){
    int id=0;
    QString string( par );
    QStringList list =  QStringList::split( '\n', string );
    for( QStringList::Iterator it = list.begin(); it != list.end(); ++it ){
    owarn << "parsePID: " << (*it).latin1() << oendl;
      if( !(*it).startsWith("CSR") ){
    id = (*it).toInt();
    break;
      }
    }
    return id;
  }
}

Device::Device(const QString &device, const QString &mode, const QString &speed )
  : QObject(0, "device") {

  owarn << "OpieTooth::Device create" << oendl;
  m_hci = 0;
  m_process = 0;
  m_attached = false;
  m_device = device;
  m_mode = mode;
  m_speed = speed;
  attach();
}
Device::~Device(){
  detach();
}
void Device::attach(){
  owarn << "attaching " << m_device.latin1() << " " << m_mode.latin1() << " " << m_speed.latin1() << oendl;
  if(m_process == 0 ){
    m_output.resize(0);
    owarn << "new process to create" << oendl;
    m_process = new OProcess();
    *m_process << "hciattach";
    *m_process << "-p";
    *m_process << m_device << m_mode << m_speed;
    connect(m_process, SIGNAL( processExited(Opie::Core::OProcess*) ),
            this, SLOT( slotExited(Opie::Core::OProcess* ) ) );
    connect(m_process, SIGNAL( receivedStdout(Opie::Core::OProcess*, char*, int) ),
            this, SLOT(slotStdOut(Opie::Core::OProcess*,char*,int ) ) );
    connect(m_process, SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int ) ),
            this, SLOT(slotStdErr(Opie::Core::OProcess*,char*,int) ) );
    if(!m_process->start(OProcess::NotifyOnExit, OProcess::AllOutput ) ){
      owarn << "Could not start" << oendl;
      delete m_process;
      m_process = 0;
    }
  };
}
void Device::detach(){
  delete m_hci;
  delete m_process;
  // kill the pid we got
  if(m_attached ){
    //kill the pid
    warn << "killing" << oendl;
    kill(pid, 9);
  }
  owarn << "detached" << oendl;
}
bool Device::isLoaded()const{
  return m_attached;
}
QString Device::devName()const {
  return QString::fromLatin1("hci0");
};
void Device::slotExited( OProcess* proc)
{
  owarn << "prcess exited" << oendl;
  if(proc== m_process ){
    owarn << "proc == m_process" << oendl;
    if( m_process->normalExit() ){ // normal exit
      owarn << "normalExit" << oendl;
      int ret = m_process->exitStatus();
      if( ret == 0 ){ // attached
    owarn << "attached" << oendl;
    owarn << "Output: " << m_output.data() << oendl;
    pid = parsePid( m_output );
    owarn << "Pid = " << pid << oendl;
    // now hciconfig hci0 up ( determine hciX FIXME)
    // and call hciconfig hci0 up
    // FIXME hardcoded to hci0 now :(
    m_hci = new OProcess( );
    *m_hci << "hciconfig";
    *m_hci << "hci0 up";
    connect(m_hci, SIGNAL( processExited(Opie::Core::OProcess*) ),
                this, SLOT( slotExited(Opie::Core::OProcess* ) ) );
    if(!m_hci->start() ){
      owarn << "could not start" << oendl;
      m_attached = false;
      emit device("hci0", false );
    }
      }else{
        owarn << "crass" << oendl;
    m_attached = false;
    emit device("hci0", false );

      }
    }
    delete m_process;
    m_process = 0;
  }else if(proc== m_hci ){
    owarn << "M HCI exited" << oendl;
    if( m_hci->normalExit() ){
      owarn << "normal exit" << oendl;
      int ret = m_hci->exitStatus();
      if( ret == 0 ){
    owarn << "attached really really attached" << oendl;
    m_attached = true;
    emit device("hci0", true );
      }else{
    owarn << "failed" << oendl;
    emit device("hci0", false );
    m_attached = false;
      }
    }// normal exit
    delete m_hci;
    m_hci = 0;
  }
}
void Device::slotStdOut(OProcess* proc, char* chars, int len)
{
  owarn << "std out" << oendl;
  if( len <1 ){
    owarn << "len < 1 " << oendl;
    return;
  }
  if(proc == m_process ){
    QCString string( chars, len+1 ); // \0 == +1
    owarn << "output: " << string.data() << oendl;
    m_output.append( string.data() );
  }
}
void Device::slotStdErr(OProcess* proc, char* chars, int len)
{
  owarn << "std err" << oendl;
  slotStdOut( proc, chars, len );
}
