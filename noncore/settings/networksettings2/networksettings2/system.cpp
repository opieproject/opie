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
    probeInterfaces();
}

System::~System( void ) {
    if( ProcDevNet )
      delete ProcDevNet;
}

int System::runAsRoot( const QString & S ) {
    QString MyS = S;
    char * usr = getenv("USER");
    char ch;

    if( S.isEmpty() ) {
      // loophole to start shell
      return 8888;
    }
    if( usr == 0 || strcmp( usr, "root" ) ) {
      // unknown or non-root user -> use SUDO
      MyS.prepend( "sudo " );
    }

    Log(("Executing %s\n", MyS.latin1() ));

    emit lineFromCommand( tr("Command : ") + MyS );
    emit lineFromCommand( "---------------" );
    Log(( "Command : %s\n", MyS.latin1() ) );
    MyS += " 2>&1 ";
    OutputOfCmd = popen( MyS.latin1(), "r" ) ;
    if( ! OutputOfCmd ) {
      // cannot fork
      return 1;
    }

    // read all data
    QString Line =  "";
    while( 1 ) {
      if( fread( &ch, 1, 1, OutputOfCmd ) < 1 ) 
        // eof
        break;
      if( ch == '\n' || ch == '\r' ) {
        if( ! Line.isEmpty() ) {
          Log(( "read cmd output : **%s**\n", Line.latin1() ) );
          emit lineFromCommand( Line );
          Line = "";
          qApp->processEvents();
        }
      } else {
        Line += ch;
      }
    }

    if( ! Line.isEmpty() ) {
      emit lineFromCommand( Line );
      Log(( "read cmd output : **%s**\n", Line.latin1() ) );
    }
    Log(( "End of command\n", Line.latin1() ) );

    if( pclose( OutputOfCmd ) < 0 ) {
      // error in command
      return 3;
    }

    // all is fine
    return 0;
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
      SL = QStringList::split( ' ', line, FALSE );

      // update data
      switch( version ) {
        case 1 :
          I.RcvBytes = SL[1];
          I.RcvErrors = SL[3];
          I.RcvDropped = SL[4];
          I.SndBytes = SL[6];
          I.SndErrors = SL[8];
          I.SndDropped = SL[9];
          I.Collisions = SL[11];
          break;
        case 2 :
          I.RcvBytes = SL[1];
          I.RcvErrors = SL[3];
          I.RcvDropped = SL[4];
          I.SndBytes = SL[7];
          I.SndErrors = SL[9];
          I.SndDropped = SL[10];
          I.Collisions = SL[12];
          break;
        case 3 :
          I.RcvBytes = SL[1];
          I.RcvErrors = SL[3];
          I.RcvDropped = SL[4];
          I.SndBytes = SL[9];
          I.SndErrors = SL[11];
          I.SndDropped = SL[12];
          I.Collisions = SL[14];
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
    if(sockfd == -1)
      return;

    // read interfaces from /proc/dev/net
    // SIOCGIFCONF does not return ALL interfaces ???!?
    ProcDevNet = new QFile(PROCNETDEV);
    if( ! ProcDevNet->open(IO_ReadOnly) ) {
      delete ProcDevNet;
      ProcDevNet =0;
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
      strcpy( ifrs.ifr_name, NicName.latin1() );

      if ( ! ( IFI = ProbedInterfaces.find( NicName ) ) ) {
        // new nic
        Log(("NEWNIC %s\n", NicName.latin1()));
        IFI = new InterfaceInfo;
        IFI->Name = line.left(loc);
        IFI->NetNode = 0;
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
          Log(("%s = %d\n", IFI->Name.latin1(), 
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
        Log(("OLDNIC %s\n", NicName.latin1()));

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
      Log(("NIC %s UP %d\n", NicName.latin1(), IFI->IsUp ));
    }
}

void System::execAsUser( QString & Cmd, char * argv[] ) {
      CurrentQPEUser CU = NSResources->currentUser();

      if( CU.UserName.isEmpty() ) {
        // if we come here, the exec was not successfull
        Log(("User not known \n" ));
        return;
      }

      // now we are ready to exec the requested command
      setuid( CU.Uid );
      setgid( CU.Gid );

      char ** envp = (char **)alloca( sizeof( char *) *
            (CU.EnvList.count()+1) );

      for( unsigned int i = 0 ; i < CU.EnvList.count() ; i ++ ) {
        *(envp+i) = CU.EnvList[i];
      }
      envp[CU.EnvList.count()]=NULL;

      execve( Cmd.latin1(), argv, envp );

      // if we come here, the exec was not successfull
      Log(("Could not exec : %d\n", errno ));
}

#include <stdarg.h>
static FILE * logf = 0;

void VLog( char * Format, ... ) {
      va_list l;

      va_start(l, Format );

      if( logf == (FILE *)0 ) {
        if( getenv("NS2STDERR") ) {
          logf = stderr;
        } else {
          logf = fopen( "/tmp/ns2log", "a" );
        }
        if( ! logf ) {
          fprintf( stderr, "Cannot open logfile /tmp/ns2log %d\n", 
              errno );
          logf = (FILE *)1;
        } else {
          fprintf( logf, "____ OPEN LOGFILE ____\n");
        }
      }

      if( (long)logf > 1 ) {
        vfprintf( logf, Format, l );
      }
      va_end( l );

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
      QStringList SL;

      SL = QStringList::split( " ", X );
      return SL.join( "_" );
}
