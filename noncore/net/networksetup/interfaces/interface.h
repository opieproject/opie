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
  virtual ~Interface(){};
  
  virtual QString getInterfaceName(){ QString n(this->name()); return n; };
  
  virtual bool getStatus(){ return status; };
  virtual void setStatus(bool newStatus);

  virtual bool isAttached(){ return attached; };
  virtual void setAttached(bool isAttached=false);
  
  virtual QString getHardwareName(){ return hardwareName; };
  virtual void setHardwareName(QString name="Unknown");
  
  virtual Module* getModuleOwner(){ return moduleOwner; };
  virtual void setModuleOwner(Module *owner=NULL);

  // inet information.
  QString getMacAddress(){ return macAddress; };
  QString getIp(){ return ip; };
  QString getSubnetMask(){ return subnetMask; };
  QString getBroadcast(){ return broadcast; };
  bool isDhcp(){ return dhcp; };
  QString getDhcpServerIp(){ return dhcpServerIp; };
  QString getLeaseObtained(){ return leaseObtained; };
  QString getLeaseExpires(){ return leaseExpires; };

public slots:
  bool refresh();
  void start();
  void stop();
  void restart();

private:
  // Interface information
  bool status;
  bool attached;
  QString hardwareName;
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

