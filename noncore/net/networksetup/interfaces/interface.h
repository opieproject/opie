#ifndef INTERFACE_H
#define INTERFACE_H

#include <qstring.h>
#include <qobject.h>

class Module;

class Interface : public QObject{
  Q_OBJECT

signals:
  void updateInterface(Interface *i);
  void updateMessage(const QString &message);
  
public:
  Interface(QObject * parent=0, const char * name= "unknown", bool status = false);
  
  QString getInterfaceName() const { QString n(this->name()); return n; };
  
  bool getStatus() const { return status; };
  void setStatus(bool newStatus);

  bool isAttached() const { return attached; };
  void setAttached(bool isAttached=false);
  
  QString getHardwareName() const { return hardwareName; };
  void setHardwareName(const QString &name="Unknown");
  
  Module* getModuleOwner() const { return moduleOwner; };
  void setModuleOwner(Module *owner=NULL);

  // inet information.
  QString getMacAddress() const { return macAddress; };
  QString getIp() const { return ip; };
  QString getSubnetMask() const { return subnetMask; };
  QString getBroadcast() const { return broadcast; };
  bool isDhcp() const { return dhcp; };
  QString getDhcpServerIp() const { return dhcpServerIp; };
  QString getLeaseObtained() const { return leaseObtained; };
  QString getLeaseExpires() const { return leaseExpires; };

public slots:
  bool refresh();
  void start();
  void stop();
  void restart();

private:
  // Interface information
  QString hardwareName;
  Module *moduleOwner;
  bool status;
  bool attached;

  // Network information
  bool dhcp;
  QString dhcpServerIp;
  QString leaseObtained;
  QString leaseExpires;

  QString macAddress;
  QString ip;
  QString broadcast;
  QString subnetMask;

};

#endif

// interface.h

