#ifndef PPP_DATA_H
#define PPP_DATA_H

#include <qstring.h>
#include <qarray.h>

class PPPData {

public :

      struct {
        bool LocalOverrule;
        bool RemoteOverrule;
        QString LocalAddress;
        QString RemoteAddress;
        bool GWAutomatic;
        QString GWAddress;
        bool GWIsDefault;
        bool GWIfNotSet;
      } IP;

      struct {
        short Mode; // 0 login, 1 chap/pap, 2 Terminal
        struct {
          QString Expect;
          QString Send;
        } Login;
        struct {
          QString Expect;
          QString Send;
        } Password;
        short PCEMode; // 0 pap, 1, chap 2, EAP
        QString Server;
        QString Client;
        QString Secret;
      } Auth;
      struct {
        bool ServerAssigned;
        QString DomainName;
        QArray<QString *> Servers;
      } DNS;
      struct {
        QString PreConnect;
        QString PostConnect;
        QString PreDisconnect;
        QString PostDisconnect;
      } Run;
}; 

#endif
