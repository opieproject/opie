#ifndef INTERFACE_H
#define INTERFACE_H

#include <qstring.h>

class Module;
class Interface {

public:
  Interface(QString name = "unknown", bool status = false);
  virtual ~Interface(){};
  
  virtual bool getStatus(){ return status; };
  virtual void setStatus(bool newSatus){ status = newSatus; refresh(); };

  virtual bool isAttached(){ return attached; };
  virtual void setAttached(bool isAttached=false){ attached = isAttached; };
  
  virtual QString getInterfaceName(){ return interfaceName; };
  virtual void setInterfaceName(QString name="unknown"){ interfaceName = name; };
  
  virtual QString getHardwareName(){ return hardareName; };
  virtual void setHardwareName(QString name="Unknown"){ hardareName = name; };
  
  virtual Module* getModuleOwner(){ return moduleOwner; };
  virtual void setModuleOwner(Module *owner=NULL){ moduleOwner = owner; };

  // inet information.
  QString getMacAddress(){ return macAddress; };
  QString getIp(){ return ip; };
  QString getSubnetMask(){ return subnetMask; };
  QString getBroadcast(){ return broadcast; };
  bool isDhcp(){ return dhcp; };
  QString getDhcpServerIp(){ return dhcpServerIp; };
  QString getLeaseObtained(){ return leaseObtained; };
  QString getLeaseExpires(){ return leaseExpires; };
  
  bool refresh();
  bool start();
  bool stop();
  bool restart();

private:
  // Interface information
  bool status;
  bool attached;
  QString interfaceName;
  QString hardareName;
  Module *moduleOwner;
  
  // Network information
  QString macAddress;
  QString ip;
  QString broadcast;
  QString subnetMask;
  bool dhcp;
  QString dhcpServerIp;
  QString leaseObtained;
  QString leaseExpires;

};

#endif

// interface.h

