#ifndef NETCONF_MODULE_H
#define NETCONF_MODULE_H

#include <qobject.h>
#if QT_VERSION < 300
#include <qlist.h>
#else
#include <qptrlist.h>
#endif
#include <qmap.h>
#include "interface.h"

class QWidget;
class QTabWidget;


/**
 * \brief The basis of all plugins
 *
 * This is the way to extend networksettings with
 * extra functionality.
 *
 *
 * Networksettings in the 1.0 release does not use QCOM
 * for activation. You need to provide the following function yourself.
 *
 * A module needs to provide Name, Images, and methods for
 * claiming interfaces. For example you can claim physicla
 * interfaces like wlan0, ppp0 or virtual like a VPN
 * connection and hide the real ppp device or ethernet device
 * behind your VPN plugin.
 *
 * During start up. The main application searches for network devices
 * and then looks for an owner under the plugins for them.
 * For example the WLAN Plugin looks if there is a WLAN Extension
 * on that interface and then claims it by returning true from isOwner()
 *
 * \code
 * extern "C"
 *  {
 *   void* create_plugin() {
 *   return new WLANModule();
 * }
 * };
 * \endcode
 * @see isOwner(Interface*)
 */
class Module : private QObject{

signals:
/**
 * Emit this Signal once you change the Interface
 * you're operating on
 *
 * @param i The Interface
 */
  void updateInterface(Interface *i);


public:
  Module(){};

  /**
   * The type of the plugin
   * and the name of the qcop call
   */
  virtual const QString type() = 0;

  /**
   * The current profile has been changed and the module should do any
   * neccesary changes also.
   * As of Opie1.0 profiles are disabled.
   *
   * @param newProfile what the profile should be changed to.
   */
  virtual void setProfile(const QString &newProfile) = 0;

  /**
   * get the icon name for this device.
   * @param Interface* can be used in determining the icon.
   * @return QString the icon name (minus .png, .gif etc)
   */
  virtual QString getPixmapName(Interface *) = 0;

  /**
   * Check to see if the interface i is owned by this module.
   * See if you can handle it. And if you can claim ownership
   * by returning true.
   * @param Interface* interface to check against
   * @return bool true if i is owned by this module, false otherwise.
   */
  virtual bool isOwner(Interface *){ return false; };

  /**
   * Create and return the Configure Module
   * @param Interface *i the interface to configure.
   * @return QWidget* pointer to this modules configure.
   *
   * @see InterfaceSetupImp
   */
  virtual QWidget *configure(Interface *){ return NULL; } ;

  /**
   * Create, and return the Information Module.
   *
   * An default Implementation is InterfaceInformationImp
   *
   * @param Interface *i the interface to get info on.
   * @return QWidget* pointer to this modules info.
   *
   * @see InterfaceInformationImp
   *
   */
  virtual QWidget *information(Interface *){ return NULL; };

  /**
   * Get all active (up or down) interfaces managed by this
   * module.
   * @return QList<Interface> A list of interfaces that exsist that havn't
   * been called by isOwner()
   */
  virtual QList<Interface> getInterfaces() = 0;

  /**
   * Adds possible new interfaces to the list (Example: usb(ppp), ir(ppp),
   *  modem ppp)
   * Both strings need to be translated. The first string is a Shortcut
   * like PPP and the second argument is a description.
   *
   * <code>
   *  list.insert(
   *
   * </code>
   *
   * @param list A reference to the list of supported additionns.
   */
  virtual void possibleNewInterfaces(QMap<QString, QString> &list) = 0;

  /**
   * Attempts to create a new interface from name you gave
   * possibleNewInterfaces()
   * @return Interface* NULL if it was unable to be created.
   * @param name the type of interface to create
   *
   * @see possibleNewInterfaces
   */
  virtual Interface *addNewInterface(const QString &name) = 0;

  /**
   * Attempts to remove the interface, doesn't delete i
   * @return bool true if successful, false otherwise.
   */
  virtual bool remove(Interface* i) = 0;

  /**
   * get dcop calls
   */
  virtual void receive(const QCString &msg, const QByteArray &arg) = 0;

    QStringList handledInterfaceNames()const { return m_inter; }
protected:
    /**
     * set which interfaceNames should not be shown cause they're handled
     * internally of this module.. An already running ppp link or
     * a tunnel... VPN an such
     */
    void setHandledInterfaceNames( const QStringList& in) { m_inter = in; }

private:
    QStringList m_inter;
};

#endif

// module.h

