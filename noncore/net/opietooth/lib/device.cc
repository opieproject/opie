
#include "kprocess.h"

#include "device.h"

using namespace OpieTooth;

Device::Device(const QString &device, const QString &mode )
  : QObject(0, "device" ) {
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
  if(m_process != 0 ){
    m_process = new KProcess();
    *m_process << "hciattach";
    *m_process << "-p";
    *m_process << m_device << m_mode;
    connect(m_process, SIGNAL( processExited(KProcess*) ),
	    this, SLOT( slotExited(KProcess* ) ) );
    connect(m_process, SIGNAL( receivedStdout(KProcess*, char*, int) ),
	    this, SLOT(slotStdOut(KProcess*,char*,int ) ) );
    connect(m_process, SIGNAL(receivedStderr(KProcess*, char*, int ) ),
	    this, SLOT(slotStdErr(KProcess*,char*,int) ) );
    if(!m_process->start(KProcess::NotifyOnExit, KProcess::AllOutput ) ){
      delete m_process;
      m_process = 0;
    }
  };
}
void Device::detach(){
  delete m_hci;
  delete m_process;
  // kill the pid we got
  if(m_attached )
    //kill the pid
    ;
}
bool Device::isLoaded()const{
  return m_attached;
}
QString Device::devName()const {
  return QString::fromLatin1("hci0");
};
void Device::slotExited( KProcess* proc)
{
  if(proc== m_process ){
    if( m_process->normalExit() ){ // normal exit
      int ret = m_process->exitStatus();
      if( ret == 0 ){ // attached
	// now hciconfig hci0 up ( determine hciX FIXME)
	// and call hciconfig hci0 up
	// FIXME hardcoded to hci0 now :(
	m_hci = new KProcess( );
	*m_hci << "hciconfig";
	*m_hci << "hci0 up";
	connect(m_hci, SIGNAL( processExited(KProcess*) ),
		this, SLOT( slotExited(KProcess* ) ) );
      }else{
	m_attached = false;
	emit device("hci0", false );

      }
    }
    delete m_process;
    m_process = 0;
  }else if(proc== m_hci ){
    if( m_hci->normalExit() ){
      int ret = m_hci->normalExit();
      if( ret == 0 ){
	emit device("hci0", true );
      }else{
	emit device("hci0", false );
	m_attached = false;
      }
    }// normal exit
    delete m_hci;
    m_hci = 0;
  }
}
void Device::slotStdOut(KProcess* proc, char* chars, int len)
{
  if( len <1 )
    return; 
  if(proc == m_process ){
    QCString string( chars );
    if(string.left(3) != "CSR" ){ // it's the pid
      pid = string.toInt();
    };
  }
}
void Device::slotStdErr(KProcess*, char*, int )
{

}
