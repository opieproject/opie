
#include <signal.h>

#include <opie/oprocess.h>

#include "device.h"

using namespace OpieTooth;

namespace {
  int parsePid( const QCString& par ){
    int id=0;
    QString string( par );
    QStringList list =  QStringList::split( '\n', string );
    for( QStringList::Iterator it = list.begin(); it != list.end(); ++it ){
    qWarning("parsePID: %s", (*it).latin1() );
      if( !(*it).startsWith("CSR") ){
	id = (*it).toInt();
	break;
      }
    }
    return id;
  }
}

Device::Device(const QString &device, const QString &mode )
  : QObject(0, "device" ) {
  qWarning("OpieTooth::Device create" );
  m_hci = 0;
  m_process = 0;
  m_attached = false;
  m_device = device;
  m_mode = mode;
  attach();
}
Device::~Device(){
  detach();
}
void Device::attach(){
  qWarning("attaching %s %s", m_device.latin1(), m_mode.latin1() );
  if(m_process == 0 ){
    m_output.resize(0);
    qWarning("new process to create" );
    m_process = new OProcess();
    *m_process << "hciattach";
    *m_process << "-p";
    *m_process << m_device << m_mode;
    connect(m_process, SIGNAL( processExited(OProcess*) ),
	    this, SLOT( slotExited(OProcess* ) ) );
    connect(m_process, SIGNAL( receivedStdout(OProcess*, char*, int) ),
	    this, SLOT(slotStdOut(OProcess*,char*,int ) ) );
    connect(m_process, SIGNAL(receivedStderr(OProcess*, char*, int ) ),
	    this, SLOT(slotStdErr(OProcess*,char*,int) ) );
    if(!m_process->start(OProcess::NotifyOnExit, OProcess::AllOutput ) ){
      qWarning("Could not start" );
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
    qWarning( "killing" );
    kill(pid, 9);
  }
  qWarning("detached" );
}
bool Device::isLoaded()const{
  return m_attached;
}
QString Device::devName()const {
  return QString::fromLatin1("hci0");
};
void Device::slotExited( OProcess* proc)
{
  qWarning("prcess exited" );
  if(proc== m_process ){
    qWarning("proc == m_process" );
    if( m_process->normalExit() ){ // normal exit
      qWarning("normalExit" );
      int ret = m_process->exitStatus();
      if( ret == 0 ){ // attached
	qWarning("attached" );
	qWarning("Output: %s", m_output.data() );
	pid = parsePid( m_output );
	qWarning("Pid = %d", pid );
	// now hciconfig hci0 up ( determine hciX FIXME)
	// and call hciconfig hci0 up
	// FIXME hardcoded to hci0 now :(
	m_hci = new OProcess( );
	*m_hci << "hciconfig";
	*m_hci << "hci0 up";
	connect(m_hci, SIGNAL( processExited(OProcess*) ),
		this, SLOT( slotExited(OProcess* ) ) );
	if(!m_hci->start() ){
	  qWarning("could not start" );
	  m_attached = false;
	  emit device("hci0", false );
	}
      }else{
        qWarning("crass" );
	m_attached = false;
	emit device("hci0", false );

      }
    }
    delete m_process;
    m_process = 0;
  }else if(proc== m_hci ){
    qWarning("M HCI exited" );
    if( m_hci->normalExit() ){
      qWarning("normal exit" );
      int ret = m_hci->exitStatus();
      if( ret == 0 ){
	qWarning("attached really really attached" );
	m_attached = true;
	emit device("hci0", true );
      }else{
	qWarning( "failed" );
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
  qWarning("std out" );
  if( len <1 ){
    qWarning( "len < 1 " );
    return; 
  }
  if(proc == m_process ){
    QCString string( chars, len+1 ); // \0 == +1
    qWarning("output: %s", string.data() );
    m_output.append( string.data() );
  }
}
void Device::slotStdErr(OProcess* proc, char* chars, int len)
{
  qWarning("std err" );
  slotStdOut( proc, chars, len );
}
