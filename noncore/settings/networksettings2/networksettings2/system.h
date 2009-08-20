#ifndef __SYSTEM__H
#define __SYSTEM__H

#include <qstring.h>

#include <opie2/oprocess.h>

using namespace Opie::Core;

// for hardware types
#include <net/if_arp.h>
#include <qdict.h>
#include <qobject.h>
#include <stdio.h>

class NetworkSetup;
class ANetNodeInstance;
class QFile;

class MyProcess : public QObject {

      Q_OBJECT

public :

      MyProcess();
      ~MyProcess();

      inline OProcess & process()
        { return *P; }

      inline void setEchoMode( bool M ) {
        EchoMode = M;
      }

public slots :

      void SLOT_Stdout( Opie::Core::OProcess * P, char *, int );
      void SLOT_Stderr( Opie::Core::OProcess * P, char *, int );
      void SLOT_ProcessExited( Opie::Core::OProcess * P);

signals :

      void stdoutLine( const QString & );
      void stderrLine( const QString & );
      void processExited( MyProcess * );

private :

      QString StdoutBuffer;
      QString StderrBuffer;
      OProcess * P;
      // output all output to my output
      bool EchoMode;
};

class InterfaceInfo {

public :

        InterfaceInfo() :
            Name(),
            MACAddress(),
            BCastAddress(),
            Netmask(),
            DstAddress() {
        }

        NetworkSetup * assignedToNetworkSetup()
          { return Collection; }

        void assignToNetworkSetup( NetworkSetup * NNI )
          { Collection = NNI; }

        NetworkSetup * Collection;    // networkSetup taking care of me
        QString Name;                   // name of interface
        int     CardType;               // type of card
        QString MACAddress;             // MAC address
        QString Address;                // IP Address
        QString BCastAddress;           // Broadcast Address
        QString Netmask;                // Netmask
        QString DstAddress;             // Peer address (if P-t-P)
        bool    IsUp;                   // interface is UP
        bool    HasMulticast;           // Supports Multicast
        bool    IsPointToPoint;         // IsPointToPoint card

        QString RcvBytes;
        QString RcvErrors;
        QString RcvPackets;
        QString RcvDropped;

        QString SndBytes;
        QString SndErrors;
        QString SndPackets;
        QString SndDropped;
        QString Collisions;
};

class System : public QObject {

      Q_OBJECT

public :

      System( void );
      ~System( void );

      QDict<InterfaceInfo> & interfaces( void );
      InterfaceInfo * interface( const QString& N )
        { return interfaces()[N]; }

      // exec command as root
      int runAsRoot( QStringList & S, MyProcess * Prc = 0 );

      // exec command as user
      int execAsUser( QStringList & Cmd, bool Synchronous = 0 );

      // refresh stats for this interface
      void refreshStatistics( InterfaceInfo & );

      // reloads interfaces
      void probeInterfaces( void );

      InterfaceInfo * findInterface( const QString & DevName );

private slots :

      void SLOT_ProcessExited( MyProcess * );

signals :

      void stdoutLine( const QString & );
      void stderrLine( const QString & );
      void processEvent( const QString & );

private  :

      QDict<InterfaceInfo>  ProbedInterfaces;
      FILE *                OutputOfCmd;
      QFile *               ProcDevNet;
};

#endif
