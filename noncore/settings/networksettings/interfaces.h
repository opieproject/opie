#ifndef INTERFACES_H
#define INTERFACES_H

#include <qstring.h>
#include <qstringlist.h>

#define INTERFACES_LOOPBACK "loopback"

#define INTERFACES_FAMILY_INET "inet"
#define INTERFACES_FAMILY_IPX "ipx"
#define INTERFACES_FAMILY_INET6 "inet6"

#define INTERFACES_METHOD_DHCP "dhcp"
#define INTERFACES_METHOD_STATIC "static"
#define INTERFACES_METHOD_PPP "ppp"

/**
 * This class provides a clean frontend for parsing the network interfaces file.
 * It provides helper functions to minipulate the options within the file.
 * See the interfaces man page for the syntax rules.
 */
class Interfaces {

public:
  Interfaces(QString useInterfacesFile = "/etc/network/interfaces");
  QStringList getInterfaceList();
  
  bool isAuto(QString interface);
  bool setAuto(QString interface, bool setAuto);
  
  bool removeInterface();
  bool addInterface(QString interface, QString family, QString method);
  bool setInterface(QString interface);
  bool isInterfaceSet();
  QString getInterfaceName(bool &error);
  bool setInterfaceName(QString newName);
  QString getInterfaceFamily(bool &error);
  bool setInterfaceFamily(QString newName);
  QString getInterfaceMethod(bool &error);
  bool setInterfaceMethod(QString newName);
  QString getInterfaceOption(QString option, bool &error); 
  bool setInterfaceOption(QString option, QString value);
  bool removeAllInterfaceOptions();
  
  bool setMapping(QString interface);
  void addMapping(QString interfaces);
  bool setMap(QString map, QString value);
  QString getMap(QString map, bool &error);
  bool setScript(QString);
  QString getScript(bool &error);
		  
  bool write();
  
private:
  bool setStanza(QString stanza, QString option,QStringList::Iterator &iterator);
  bool setOption(QStringList::Iterator start, QString option, QString value);
  QString getOption(QStringList::Iterator start, QString option, bool &error);
  bool removeAllOptions(QStringList::Iterator start);
  
  QString interfacesFile;
  QStringList interfaces;
  QStringList::Iterator currentIface;
  QStringList::Iterator currentMapping;

  QStringList acceptedFamily;
};

#endif

// interfaces

