#include <qdir.h>
#include <qfileinfo.h>

#include <bluezlib.h>

/* OPIE */
#include <opie2/odevice.h>
#include <opie2/oprocess.h>
#include <opie2/odebug.h>

#include <OTDevice.h>

using namespace Opie::Core;
using namespace Opietooth2;
using Opie::Core::OProcess;

OTDevice::OTDevice( OTGateway * _OT ) : QObject(0, "device") {
      
      // initialize 
      OT = _OT;

      // detect bluetooth type
      QString a, b; // fake
      unsigned long c; // fake
      detectDeviceType( a, b, c );

      if( needsAttach() ) {
        // requires HCIATTACH
        // pid of hciattach
        m_hciattachPid = getPidOfHCIAttach();

        m_hciattach = 0;

        if( m_hciattachPid == 0 ) {
          // no pid -> not attached
          m_deviceNr = -1;
        } else {
          // system enabled
          // currently no way to figure out which attach produce which
          // hci
          m_deviceNr = 0;
        }
      } else {
        m_deviceNr = 0;
      }
}

OTDevice::~OTDevice(){
      if( needsAttach() && m_hciattach ) {
        // does not auto stop bluetooth
        m_hciattach->detach();
        delete m_hciattach;
      }
}

bool OTDevice::attach(){

      if( needsAttach() && m_hciattachPid == 0 ) {
        QString Dev, Mode;
        unsigned long Spd;

        detectDeviceType( Dev, Mode, Spd );

        // not yet started
        m_hciattach = new OProcess();
        *m_hciattach << "hciattach";
        *m_hciattach << "-p"; // so that it prints its pid
        *m_hciattach << Dev 
                     << Mode 
                     << QString().setNum(Spd);

        connect( m_hciattach, 
                 SIGNAL(receivedStdout(Opie::Core::OProcess*, char*, int ) ),
                 this, SLOT
                 (slotStdOut(Opie::Core::OProcess*,char*,int) ) );

        connect( m_hciattach, 
                 SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int ) ),
                 this, SLOT
                 (slotStdErr(Opie::Core::OProcess*,char*,int) ) );

        // we need to start this in 'dontcare' mode because
        // if qpe exists we want the hci to keep running
        if( ! m_hciattach->start( OProcess::DontCare, 
                                  OProcess::AllOutput
                                ) ){
          emit error( tr( "Could not start hciattach" ) );
          delete m_hciattach;
          m_hciattach = 0;
          return FALSE;
        }
      }

      return TRUE;
}

bool OTDevice::detach(){

      if( needsAttach() && m_hciattachPid ) {
        if( m_hciattach ) {
          delete m_hciattach;
          m_hciattach = 0;
        }

        if( kill( m_hciattachPid, 9) < 0 ) {
          odebug << "could not stop " << errno << oendl;
          emit error( tr( "Could not stop process" ) );
          return FALSE;
        }
        m_hciattachPid = 0;
        emit isEnabled( m_deviceNr, 0 );
        m_deviceNr = -1;
      }

      return TRUE;
}

bool OTDevice::isAttached()const{
      return ! needsAttach() || m_hciattachPid != 0;
}

bool OTDevice::checkAttach(){
      if( ! needsAttach() ) {
        m_deviceNr = 0;
        emit isEnabled( 0, 1 );
        return TRUE;
      }

      if( m_hciattachPid ) {
        QString S;
        S.setNum( m_hciattachPid );
        QDir D( "/proc" );
        if( !D.exists( S ) ) {
          // down
          m_hciattachPid = 0;
          emit isEnabled( m_deviceNr, 0 );
          m_deviceNr = -1;
        }
      } else {
        // check
        m_hciattachPid = getPidOfHCIAttach();
        if ( m_hciattachPid ) {
          m_deviceNr = 0;
          emit isEnabled( m_deviceNr, 1 );
        }
      }
      return m_hciattachPid != 0;
}

void OTDevice::slotStdOut(OProcess* proc, char* , int ) {
      if( proc == m_hciattach ) {
        m_hciattach->detach();

        // system enabled
        // currently no way to figure out which attach produce which
        // hci
        if( m_deviceNr == -1 ) {
          m_deviceNr = 0;
          emit isEnabled( m_deviceNr, 1 );
        }
      } 
}

void OTDevice::slotStdErr(OProcess* proc, char* chars, int len) {

      if(proc == m_hciattach && len >= 1 ){
        // collect output
        QCString string( chars, len+1 ); // \0 == +1
        QString m_output;
        m_output.append( string.data() );
        odebug << m_output << oendl;
      }
}

pid_t OTDevice::getPidOfHCIAttach( void ) {

      if( needsAttach() ) {
        // not yet attached -> perhaps now ?
        // load /proc dir and check if command name contains hciattach
        QRegExp R("[0-9]+");
        QDir ProcDir( "/proc" );
        QFileInfo FI;
        QStringList EL = ProcDir.entryList( QDir::Dirs );

        // print it out
        for ( QStringList::Iterator it = EL.begin(); 
              it != EL.end(); 
              ++it ) {
          if( R.match( (*it) ) >= 0 ) {
            // is pid

            // get command being executed
            FI.setFile( ProcDir.path()+"/"+ (*it) + "/exe" );

            // get the link
            if( FI.readLink().right( 9 ) == "hciattach" ) {
              // this is hci attach process

              return (*it).toULong();
              break;
            }
          }
        }
      }

      return 0;
}

void OTDevice::detectDeviceType( QString & Device,
                                 QString & Mode,
                                 unsigned long & Speed ) {

     // detect device type and determine parms
     odebug << "Detecting device" << oendl;
     switch ( ODevice::inst()->model() ) {
      case Model_iPAQ_H39xx:
          Device = "/dev/tts/1";
          Mode = "bcsp";
          Speed = 921600;
          NeedsAttach = 1;
          break;

      case Model_iPAQ_H5xxx:
          Device = "/dev/tts/1";
          Mode = "any";
          Speed = 921600;
          NeedsAttach = 1;
          break;

      case Model_GenuineIntel :
          Device = "";
          Mode = "";
          Speed = 0;
          NeedsAttach = 0;
          break;

      default:
          Device = "/dev/ttySB0";
          Mode = "bcsp";
          Speed = 230400;
          NeedsAttach = 1;
          break;
      }
}

QString OTDevice::getRFCommDevicePattern( void ) {

      QDir D( "/dev/bluetooth/rfcomm" );
      if( D.exists() ) {
        // devfs
        return QString( "/dev/bluetooth/rfcomm/%1" );
      }

      // regular 'dev' directory
      return QString( "/dev/rfcomm%1" );
}
