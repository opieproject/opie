#ifndef __SYSTEM__H
#define __SYSTEM__H

// for hardware types
#include <net/if_arp.h>
#include <qdict.h>

class ANetNodeInstance;
class QFile;

class InterfaceInfo {

public :

        InterfaceInfo() :
            Name(),
            MACAddress(),
            BCastAddress(),
            Netmask(),
            DstAddress() {
        }

        ANetNodeInstance * assignedNode()
          { return NetNode; }

        void assignNode( ANetNodeInstance * NNI )
          { NetNode = NNI; }

        ANetNodeInstance * NetNode;     // netnode taking care of me
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
        QString SndBytes;
        QString RcvErrors;
        QString SndErrors;
        QString RcvDropped;
        QString SndDropped;
        QString Collisions;
};

class System {

public :

      System( void );
      ~System( void );

      QDict<InterfaceInfo> & interfaces( void ) 
        { return ProbedInterfaces; }
      InterfaceInfo * interface( const QString& N ) 
        { return ProbedInterfaces[N]; }

      // exec command as root
      int runAsRoot( const QString & S );

      // exec command as user
      void execAsUser( QString & Cmd, char * argv[] );

      // refresh stats for this interface
      void refreshStatistics( InterfaceInfo & );

      // reloads interfaces
      void probeInterfaces( void );

private  :

      QDict<InterfaceInfo>  ProbedInterfaces;
      QFile *               ProcDevNet;

};

#endif
