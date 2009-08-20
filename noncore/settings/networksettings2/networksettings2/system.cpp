#include <sys/types.h>
#include <sys/wait.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <opie2/oprocess.h>

#include <qdir.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qapplication.h>

#include "resources.h"
#include "system.h"

#define PROCNETDEV "/proc/net/dev"

#ifndef ARPHRD_IEEE80211
#define ARPHRD_IEEE80211 801
#endif

static char Dig2Hex[] = {
  '0', '1', '2', '3',
  '4', '5', '6', '7',
  '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F'
};

// get HIGH nibble of byte
#define HN(x)           Dig2Hex[(((x)&0xf0)>>4)]
// get LOW nibble of byte
#define LN(x)           Dig2Hex[((x)&0x0f)]

System::System( void ) : QObject(), ProbedInterfaces() {
    ProcDevNet = 0;
}

System::~System( void ) {
    if( ProcDevNet )
      delete ProcDevNet;
}

QDict<InterfaceInfo> & System::interfaces( void ) {
    if( ProbedInterfaces.count() == 0 ) {
      probeInterfaces();
    }
    return ProbedInterfaces;
}

int System::runAsRoot( QStringList & S, MyProcess * Prc ) {
    char * usr = getenv("USER");

    if( S.count() == 0 ) {
      // close loophole to start shell
      return 8888;
    }
    if( usr == 0 || strcmp( usr, "root" ) ) {
      // unknown or non-root user -> use SUDO
      S.prepend( "sudo" );
    }

    if( getenv( "NS2TESTMODE" ) ) {
      odebug << "TESTMODE !!! execute "
            << S.join( " ")
            << oendl;
    } else {
      MyProcess * P;

      if( Prc ) {
        P = Prc;
      } else {
        P = new MyProcess();
        emit processEvent( tr("Command : ") + S.join( " " ) );

        connect( P,
                 SIGNAL( stdoutLine( const QString & ) ),
                 this,
                 SIGNAL( stdoutLine( const QString & ) ) );

        connect( P,
                 SIGNAL( stderrLine( const QString & ) ),
                 this,
                 SIGNAL( stderrLine( const QString & ) ) );

        connect( P,
                 SIGNAL(processExited(MyProcess*) ),
                 this, SLOT
                 (SLOT_ProcessExited(MyProcess*) ) );
      }

      P->process() << S;

      Log(("Executing %s\n", S.join( " " ).latin1() ));

      if( ! P->process().start( OProcess::DontCare,
                      OProcess::AllOutput ) ) {
        odebug << "Error starting " << S << oendl;
        if( ! Prc )
          delete P;
        // error starting app
        return 0;
      }
      odebug << "Started " << S << oendl;
    }

    // all is fine
    return 1;
}

int System::execAsUser( QStringList & SL, bool Synchronous ) {
      MyProcess * P = new MyProcess();
      CurrentQPEUser CU = NSResources->currentUser();
      char * usr = getenv("USER");

      if( usr == 0 ||
          strcmp( usr, "root" ) == 0 ) {
        // find user running qpe
        if( CU.UserName.isEmpty() ) {
          // if we come here, the exec was not successfull
          Log(("User not known \n" ));
          return 0;
        }
      }

      // now we are ready to exec the requested command
      setuid( CU.Uid );
      setgid( CU.Gid );

      for( unsigned int i = 0 ; i < CU.EnvList.count() ; i ++ ) {
        QString X;
        QStringList SL;
        X = CU.EnvList[i];
        SL = QStringList::split( "=", X );
        P->process().setEnvironment( SL[0], SL[1] );
      }

      P->process() << SL;

      emit processEvent( tr("Command : ") + SL.join( " " ) );

      Log(("Executing as user %s : %s\n",
            CU.UserName.latin1(),
            SL.join( " " ).latin1() ));

      P->setEchoMode( Synchronous );

      bool rv = P->process().start(
            (Synchronous) ? OProcess::Block :
                            OProcess::DontCare,
            (Synchronous) ? OProcess::AllOutput :
                            OProcess::NoCommunication );
      delete P;

      if( ! rv ) {
        // if we come here, the exec was not successfull
        Log(("Could not exec : %d\n", errno ));
      }

      return rv;
}

void System::SLOT_ProcessExited( MyProcess * P ) {
      QString R;

      for( QValueListConstIterator<QCString> it = P->process().args().begin();
           it != P->process().args().end();
           ++it ) {
        R += (*it);
        R += " ";
      }

      R += "Returned with " + QString().setNum( P->process().exitStatus() );
      emit processEvent( R );
      delete P;
}

void System::refreshStatistics( InterfaceInfo & I ) {
    if( ! ProcDevNet ) {
      return;
    }
    // cannot seek on dev
    ProcDevNet->close();
    ProcDevNet->open( IO_ReadOnly );

    QString line;
    QTextStream procTs(ProcDevNet);
    QStringList SL;
    int loc = -1;
    int version;

    procTs.readLine();
    line = procTs.readLine();
    // get version
    if( line.find("compressed") )
      version = 3;
    else if( line.find( "bytes" ) )
      version = 2;
    else
      version = 1;
    while((line = procTs.readLine().simplifyWhiteSpace()) != QString::null) {
      if( (loc = line.find(":") ) == -1) {
        continue;
      }

      if( I.Name != line.left(loc) )
        continue;

      // tokenize
      SL = QStringList::split( ' ', line.mid(loc+1), FALSE );

      // update data
      switch( version ) {
        case 1 :
          I.RcvBytes = "";
          I.RcvPackets = SL[0];
          I.RcvErrors = SL[1];
          I.RcvDropped = SL[2];

          I.SndBytes = "";
          I.SndPackets = SL[5];
          I.SndErrors = SL[6];
          I.SndDropped = SL[7];

          I.Collisions = SL[9];
          break;
        case 2 :
          I.RcvBytes = SL[0];
          I.RcvPackets = SL[1];
          I.RcvErrors = SL[2];
          I.RcvDropped = SL[3];

          I.SndBytes = SL[6];
          I.SndPackets = SL[7];
          I.SndErrors = SL[8];
          I.SndDropped = SL[9];

          I.Collisions = SL[11];
          break;
        case 3 :
          I.RcvBytes = SL[0];
          I.RcvPackets = SL[1];
          I.RcvErrors = SL[2];
          I.RcvDropped = SL[3];

          I.SndBytes = SL[8];
          I.SndPackets = SL[9];
          I.SndErrors = SL[10];
          I.SndDropped = SL[11];

          I.Collisions = SL[13];
          break;
      }
      break;
    }
}

//
// THIS UPDATES THE LIST -> INTERFACES ARE NOT DELETED BUT
// FLAGGED AS ! 'IsUp' IF NO LONGER PRESENT
//

void System::probeInterfaces( void ) {

    // probe interfaces
    int sockfd;
    // get list of all interfaces
    struct ifreq ifrs;
    InterfaceInfo * IFI;

    // flag all as 'down'
    for( QDictIterator<InterfaceInfo> it( ProbedInterfaces );
         it.current();
         ++it ) {
      it.current()->IsUp = 0;
    }

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) {
      odebug << "Cannot open INET socket "
            << errno
            << " "
            << strerror( errno )
            << oendl;
      return;
    }

    // read interfaces from /proc/dev/net
    // SIOCGIFCONF does not return ALL interfaces ???!?
    ProcDevNet = new QFile(PROCNETDEV);
    if( ! ProcDevNet->open(IO_ReadOnly) ) {
      odebug << "Cannot open "
            << PROCNETDEV
            << " "
            << errno
            << " "
            << strerror( errno )
            << oendl;
      delete ProcDevNet;
      ProcDevNet =0;
      ::close( sockfd );
      return;
    }

    QString line;
    QString NicName;
    QTextStream procTs(ProcDevNet);
    int loc = -1;

    procTs.readLine(); // eat a line
    procTs.readLine(); // eat a line
    while((line = procTs.readLine().simplifyWhiteSpace()) != QString::null) {
      if((loc = line.find(":")) == -1) {
        continue;
      }

      NicName = line.left(loc);

      // set name for ioctl
      strncpy( ifrs.ifr_name, NicName.latin1(), 16 );

      if ( ! ( IFI = ProbedInterfaces.find( NicName ) ) ) {
        // new nic
        Log(("New NIC found : %s\n", NicName.latin1()));
        IFI = new InterfaceInfo;
        IFI->Name = line.left(loc);
        IFI->Collection = 0;
        ProbedInterfaces.insert( IFI->Name, IFI );

        // get dynamic info
        if( ioctl(sockfd, SIOCGIFFLAGS, &ifrs) >= 0 ) {
          IFI->IsPointToPoint = ((ifrs.ifr_flags & IFF_POINTOPOINT) == IFF_POINTOPOINT);
        } else {
          IFI->IsPointToPoint = 0;
        }

        // settings that never change
        IFI->DstAddress = "";

        if( IFI->IsPointToPoint ) {
          if( ioctl(sockfd, SIOCGIFDSTADDR, &ifrs) >= 0 ) {
            IFI->DstAddress =
             inet_ntoa(((struct sockaddr_in*)&ifrs.ifr_dstaddr)->sin_addr);
          }
        }

        IFI->CardType = 999999;
        IFI->MACAddress = "";

        if( ioctl(sockfd, SIOCGIFHWADDR, &ifrs) >= 0 ) {
          Log(("Family for NIC %s : %d\n", IFI->Name.latin1(),
              ifrs.ifr_hwaddr.sa_family ));

          IFI->CardType = ifrs.ifr_hwaddr.sa_family;
          switch( ifrs.ifr_hwaddr.sa_family ) {
            case ARPHRD_ETHER : // regular MAC address
              // valid address -> convert to regular ::: format
              // length = 6 bytes = 12 DIGITS -> 6 :
              IFI->MACAddress.sprintf(
                "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
                HN( ifrs.ifr_hwaddr.sa_data[0] ),
                LN( ifrs.ifr_hwaddr.sa_data[0] ),
                HN( ifrs.ifr_hwaddr.sa_data[1] ),
                LN( ifrs.ifr_hwaddr.sa_data[1] ),
                HN( ifrs.ifr_hwaddr.sa_data[2] ),
                LN( ifrs.ifr_hwaddr.sa_data[2] ),
                HN( ifrs.ifr_hwaddr.sa_data[3] ),
                LN( ifrs.ifr_hwaddr.sa_data[3] ),
                HN( ifrs.ifr_hwaddr.sa_data[4] ),
                LN( ifrs.ifr_hwaddr.sa_data[4] ),
                HN( ifrs.ifr_hwaddr.sa_data[5] ),
                LN( ifrs.ifr_hwaddr.sa_data[5] )
              );
              break;
#ifdef ARPHRD_IEEE1394
            case ARPHRD_IEEE1394 : // Firewire Eth address
              IFI->MACAddress.sprintf(
                "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c-00-00",
                HN( ifrs.ifr_hwaddr.sa_data[0] ),
                LN( ifrs.ifr_hwaddr.sa_data[0] ),
                HN( ifrs.ifr_hwaddr.sa_data[1] ),
                LN( ifrs.ifr_hwaddr.sa_data[1] ),
                HN( ifrs.ifr_hwaddr.sa_data[2] ),
                LN( ifrs.ifr_hwaddr.sa_data[2] ),
                HN( ifrs.ifr_hwaddr.sa_data[3] ),
                LN( ifrs.ifr_hwaddr.sa_data[3] ),
                HN( ifrs.ifr_hwaddr.sa_data[4] ),
                LN( ifrs.ifr_hwaddr.sa_data[4] ),
                HN( ifrs.ifr_hwaddr.sa_data[5] ),
                LN( ifrs.ifr_hwaddr.sa_data[5] ),
                HN( ifrs.ifr_hwaddr.sa_data[6] ),
                LN( ifrs.ifr_hwaddr.sa_data[6] ),
                HN( ifrs.ifr_hwaddr.sa_data[7] ),
                LN( ifrs.ifr_hwaddr.sa_data[7] ),
                HN( ifrs.ifr_hwaddr.sa_data[8] ),
                LN( ifrs.ifr_hwaddr.sa_data[8] ),
                HN( ifrs.ifr_hwaddr.sa_data[9] ),
                LN( ifrs.ifr_hwaddr.sa_data[9] ),
                HN( ifrs.ifr_hwaddr.sa_data[10] ),
                LN( ifrs.ifr_hwaddr.sa_data[10] ),
                HN( ifrs.ifr_hwaddr.sa_data[11] ),
                LN( ifrs.ifr_hwaddr.sa_data[11] ),
                HN( ifrs.ifr_hwaddr.sa_data[12] ),
                LN( ifrs.ifr_hwaddr.sa_data[12] ),
                HN( ifrs.ifr_hwaddr.sa_data[13] ),
                LN( ifrs.ifr_hwaddr.sa_data[13] )
              );
              break;
#endif
            case ARPHRD_PPP : // PPP
              break;
            case ARPHRD_IEEE80211 : // WLAN
              break;
            case ARPHRD_IRDA : // IRDA
              break;
          }
        }
      } else // else already probed before -> just update
        Log(("Redetected NIC %s\n", NicName.latin1()));

      // get dynamic info
      if( ioctl(sockfd, SIOCGIFFLAGS, &ifrs) >= 0 ) {
        IFI->IsUp = ((ifrs.ifr_flags & IFF_UP) == IFF_UP);
        IFI->HasMulticast = ((ifrs.ifr_flags & IFF_MULTICAST) == IFF_MULTICAST);
      } else {
        IFI->IsUp = 0;
        IFI->HasMulticast = 0;
      }

      if( ioctl(sockfd, SIOCGIFADDR, &ifrs) >= 0 ) {
        IFI->Address =
           inet_ntoa(((struct sockaddr_in*)&ifrs.ifr_addr)->sin_addr);
      } else {
        IFI->Address = "";
        IFI->IsUp = 0;
      }
      if( ioctl(sockfd, SIOCGIFBRDADDR, &ifrs) >= 0 ) {
        IFI->BCastAddress =
           inet_ntoa(((struct sockaddr_in*)&ifrs.ifr_broadaddr)->sin_addr);
      } else {
        IFI->BCastAddress = "";
      }
      if( ioctl(sockfd, SIOCGIFNETMASK, &ifrs) >= 0 ) {
        IFI->Netmask =
           inet_ntoa(((struct sockaddr_in*)&ifrs.ifr_netmask)->sin_addr);
      } else {
        IFI->Netmask = "";
      }
      Log(("NIC %s UP ? %d\n", NicName.latin1(), IFI->IsUp ));
    }

    ::close( sockfd );
}

InterfaceInfo * System::findInterface( const QString & N ) {
      InterfaceInfo * Run;
      // has PAN NetworkSetup UP interface ?
      for( QDictIterator<InterfaceInfo> It(ProbedInterfaces);
           It.current();
           ++It ) {
        Run = It.current();
        if( N == Run->Name ) {
          // this PAN NetworkSetup is up
          return Run;
        }
      }
      return 0;
}

#include <stdarg.h>
static FILE * logf = 0;

void VLog( char * Format, ... ) {
      va_list l;

      va_start(l, Format );

      if( logf == (FILE *)0 ) {
        QString S = getenv("NS2LOG");
        if( S == "stderr" ) {
          logf = stderr;
        } else if( S.isEmpty() ) {
          logf = fopen( "/tmp/ns2log", "a" );
        } else {
          logf = fopen( S, "a" );
        }

        if( ! logf ) {
          fprintf( stderr, "Cannot open logfile %s : %d\n",
              S.latin1(), errno );
          logf = (FILE *)1;
        } else {
          fprintf( logf, "____ OPEN LOGFILE ____\n");
        }
      }

      if( (unsigned long)logf > 1 ) {
        vfprintf( logf, Format, l );
      }
      va_end( l );
      fflush( logf );

}

void LogClose( void ) {
      if( (long)logf > 1 ) {
        fprintf( logf, "____ CLOSE LOGFILE ____\n");
        if( logf != stderr ) {
          fclose( logf );
        }
        logf = 0;
      }
}

QString removeSpaces( const QString & X ) {
      QString Y;
      Y = X.simplifyWhiteSpace();
      Y.replace( QRegExp(" "), "_" );
      odebug << X <<  " **" << Y << "**" << oendl;
      return Y;
}

//
//
//
//
//

MyProcess::MyProcess() : QObject(), StdoutBuffer(), StderrBuffer() {
      P = new OProcess();
      connect( P,
               SIGNAL( receivedStdout(Opie::Core::OProcess*, char*, int ) ),
               this,
               SLOT( SLOT_Stdout(Opie::Core::OProcess*,char*,int) ) );

      connect( P,
               SIGNAL( receivedStderr(Opie::Core::OProcess*, char*, int ) ),
               this,
               SLOT( SLOT_Stderr(Opie::Core::OProcess*,char*,int) ) );
      connect( P,
               SIGNAL( processExited(Opie::Core::OProcess*) ),
               this,
               SLOT( SLOT_ProcessExited(Opie::Core::OProcess*) ) );
}

MyProcess::~MyProcess() {
      delete P;
}

void MyProcess::SLOT_Stdout( Opie::Core::OProcess * , char * Buf, int len ) {
      if( EchoMode ) {
        write( 1, Buf, len );
        return;
      }

      char * LB = (char *)alloca( len + 1 );
      memcpy( LB, Buf, len );
      LB[len] = '\0';

      // now input is zero terminated
      StdoutBuffer += LB;

      odebug << "Received " << len << " bytes on stdout" << oendl;
      // see if we have some lines (allow empty lines)
      QStringList SL = QStringList::split( "\n", StdoutBuffer, TRUE );

      for( unsigned int i = 0; i < SL.count()-1; i ++ ) {
        Log(( "Stdout : \"%s\"\n", SL[i].latin1() ) );
        emit stdoutLine( SL[i] );
      }

      // last line is rest
      StdoutBuffer = SL[ SL.count()-1 ];
}

void MyProcess::SLOT_Stderr( Opie::Core::OProcess * , char * Buf, int len ) {
      if( EchoMode ) {
        write( 2, Buf, len );
        return;
      }

      char * LB = (char *)alloca( len + 1 );
      memcpy( LB, Buf, len );
      LB[len] = '\0';

      // now input is zero terminated
      StderrBuffer += LB;

      odebug << "Received " << len << " bytes on stderr" << oendl;
      // see if we have some lines (allow empty lines)
      QStringList SL = QStringList::split( "\n", StderrBuffer, TRUE );

      for( unsigned int i = 0; i < SL.count()-1; i ++ ) {
        Log(( "Stderr : \"%s\"\n", SL[i].latin1() ) );
        emit stderrLine( SL[i] );
      }

      // last line is rest
      StderrBuffer = SL[ SL.count()-1 ];
}

void MyProcess::SLOT_ProcessExited( Opie::Core::OProcess * ) {
      emit processExited( this );
}
