#ifndef NETCONF_MODULE_H
#define NETCONF_MODULE_H

#include <qobject.h>
#include <qlist.h>
#include <qmap.h>
#include "interface.h"

class QWidget;
class QTabWidget;

class Module : QObject{

signals:
  void updateInterface(Interface *i);
	
public:
  Module(){};

  /**
   * The current profile has been changed and the module should do any
   * neccesary changes also.
   * @param newProfile what the profile should be changed to.
   */ 
  virtual void setProfile(QString newProfile) = 0;
  
  /**
   * get the icon name for this device.
   * @param Interface* can be used in determining the icon.
   * @return QString the icon name (minus .png, .gif etc) 
   */ 
  virtual QString getPixmapName(Interface *) = 0;

  /**
   * Check to see if the interface i is owned by this module.
   * @param Interface* interface to check against
   * @return bool true if i is owned by this module, false otherwise.
   */
  virtual bool isOwner(Interface *){ return false; };
  
  /**
   * Create, set tabWiget and return the WLANConfigure Module
   * @param Interface *i the interface to configure.
   * @param tabWidget a pointer to the tab widget that this configure has.
   * @return QWidget* pointer to the tab widget in this modules configure.
   */
  virtual QWidget *configure(Interface *, QTabWidget **){ return NULL; } ;
  
  /**
   * Create, set tabWiget and return the Information Module
   * @param Interface *i the interface to get info on.
   * @param tabWidget a pointer to the tab widget that this information has.
   * @return QWidget* pointer to the tab widget in this modules info.
   */
  virtual QWidget *information(Interface *, QTabWidget **){ return NULL; };
  
  /**
   * Get all active (up or down) interfaces
   * @return QList<Interface> A list of interfaces that exsist that havn't
   * been called by isOwner()
   */  
  virtual QList<Interface> getInterfaces() = 0;
  
  /**
   * Adds possible new interfaces to the list (Example: usb(ppp), ir(ppp),
   *  modem ppp)
   */
  virtual void possibleNewInterfaces(QMap<QString, QString> &list) = 0;
  
  /**
   * Attempts to create a new interface from name
   * @return Interface* NULL if it was unable to be created.
   * @param name the type of interface to create
   */  
  virtual Interface *addNewInterface(QString name) = 0;
  
  /**
   * Attempts to remove the interface, doesn't delete i
   * @return bool true if successfull, false otherwise.
   */
  virtual bool remove(Interface* i) = 0;

};

#endif

// module.h

