#include "interfaces.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>

#define AUTO "auto"
#define IFACE "iface"
#define MAPPING "mapping"

/**
 * Constructor.  Reads in the interfaces file and then split the file up by
 *  the \n for interfaces variable. 
 * @param useInterfacesFile if an interface file other then the default is
 *  desired to be used it should be passed in.
 */ 
Interfaces::Interfaces(QString useInterfacesFile){
  acceptedFamily.append(INTERFACES_FAMILY_INET);
  acceptedFamily.append(INTERFACES_FAMILY_IPX);
  acceptedFamily.append(INTERFACES_FAMILY_INET6);
	
  interfacesFile = useInterfacesFile;
  QFile file(interfacesFile);
  if (!file.open(IO_ReadOnly)){
    qDebug(QString("Interfaces: Can't open file: %1 for reading.").arg(interfacesFile).latin1());
    currentIface = interfaces.end();
    currentMapping = interfaces.end();
    return;
  }
  QTextStream stream( &file );
  QString line;
  while ( !stream.eof() ) {
    line += stream.readLine();
    line += "\n";
  }
  file.close();
  interfaces = QStringList::split("\n", line, true);
  
  currentIface = interfaces.end();
  currentMapping = interfaces.end();
}

/**
 * Find out if interface is in an "auto" group or not.
 *  Report any duplicates such as eth0 being in two differnt auto's 
 * @param 
 * @return true is interface is in auto
 */ 
bool Interfaces::isAuto(QString interface){
  QStringList autoLines = interfaces.grep(QRegExp(AUTO));
  QStringList awi = autoLines.grep(QRegExp(interface));
  if(awi.count() > 1)
    qDebug(QString("Interfaces: Found more then auto group with interface: %1.").arg(interface).latin1());
  if(awi.count() < 1)
    return false;
  return true;
}

/**
 * Attempt to set the auto option for interface to setAuto.
 * @param interface the interface to set
 * @param setAuto the value to set interface to.
 * @return false if already set to setAuto.
 * */ 
bool Interfaces::setAuto(QString interface, bool setAuto){
  // Don't need to set it if it is already set.
  if(isAuto(interface) == setAuto)
    return false;
  
  bool changed = false;
  for ( QStringList::Iterator it = interfaces.begin(); it != interfaces.end(); ++it ) {
    if((*it).contains(AUTO)){
      //We know that they are not in any group so let add to this auto.
      if(setAuto){
        (*it) = (*it) += " " + interface;
        // Don't care to have such thins as: auto eth0   lo  usb0
	(*it) = (*it).simplifyWhiteSpace();
        changed = true;
        break;
      }
      else{
        if((*it).contains(interface)){
          (*it) = (*it).replace(QRegExp(interface), "");
          // clean up
	  QString line = (*it).simplifyWhiteSpace();
          line = line.replace(QRegExp(" "),"");
          if(line == AUTO)
            (*it) = "";
          changed = true;
	  // Don't break because we want to make sure we remove all cases.
        }
      }
    }
  }
  if(changed == false){
    if(setAuto == true)
      interfaces.append(QString(AUTO" %1").arg(interface));
    else{
      qDebug(QString("Interfaces: Can't set interface %1 auto to false sense it is already false.").arg(interface).latin1());
    }
  }
  return true;
}

/**
 * Set the current interface to interface.  This needs to be done before you
 *  can call getFamily(), getMethod, and get/setOption().
 * @param interface the name of the interface to set.  All whitespace is
 *  removed from the interface name.
 * @return bool true if it is successfull. 
 */ 
bool Interfaces::setInterface(QString interface){
  interface = interface.simplifyWhiteSpace();
  interface = interface.replace(QRegExp(" "), "");
  return setStanza(IFACE, interface, currentIface);
}

/**
 * A quick helper funtion to see if the current interface is set.
 * @return bool true if set, false otherwise.
 */ 
bool Interfaces::isInterfaceSet(){
  return (currentIface != interfaces.end());
}

/**
 * Add a new interface of with the settings - family and method
 * @param interface the name of the interface to set.  All whitespace is
 *  removed from the interface name.
 * @param family the family of this interface inet or inet, ipx or inet6
 *  Must of one of the families defined in interfaces.h
 * @param method for the family.  see interfaces man page for family methods.
 * @return true if successfull.
 */ 
bool Interfaces::addInterface(QString interface, QString family, QString method){
  if(acceptedFamily.contains(family)==0)
    return false;
  interface = interface.simplifyWhiteSpace();
  interface = interface.replace(QRegExp(" "), "");
  interfaces.append("");
  interfaces.append(QString(IFACE " %1 %2 %3").arg(interface).arg(family).arg(method));
  return true;
}

/**
 * Remove the currently selected interface and all of its options.
 * @return bool if successfull or not.
 */ 
bool Interfaces::removeInterface(){
  if(currentIface == interfaces.end())
    return false;
  (*currentIface) = "";
  return removeAllInterfaceOptions();
}

/**
 * Gets the hardware name of the interface that is currently selected.
 * @return QString name of the hardware interface (eth0, usb2, wlan1...).
 * @param error set to true if any error occurs, false otherwise.
 */ 
QString Interfaces::getInterfaceName(bool &error){
  if(currentIface == interfaces.end()){
    error = true;
    return QString();
  }
  QString line = (*currentIface);
  line = line.mid(QString(IFACE).length() +1, line.length());
  line = line.simplifyWhiteSpace();
  int findSpace = line.find(" ");
  if( findSpace < 0){
    error = true;
    return QString();
  }
  error = false;
  return line.mid(0, findSpace);
}

/**
 * Gets the family name of the interface that is currently selected.
 * @return QString name of the family (inet, inet6, ipx).
 * @param error set to true if any error occurs, false otherwise.
 */ 
QString Interfaces::getInterfaceFamily(bool &error){
  QString name = getInterfaceName(error);
  if(error){
    error = true;
    return QString();
  }
  QString line = (*currentIface);
  line = line.mid(QString(IFACE).length() +1, line.length());
  line = line.mid(name.length()+1, line.length());
  line = line.simplifyWhiteSpace();
  int findSpace = line.find(" ");
  if( findSpace < 0){
    error = true;
    return QString();
  }
  error = false;
  return line.mid(0, findSpace);
}

/**
 * Gets the method of the interface that is currently selected.
 * @return QString name of the method such as staic or dhcp.
 * See the man page of interfaces for possible methods depending on the family.
 * @param error set to true if any error occurs, false otherwise.
 */ 
QString Interfaces::getInterfaceMethod(bool &error){
  QString name = getInterfaceName(error);
  if(error){
    error = true;
    return QString();
  }
  QString family = getInterfaceFamily(error);
  if(error){
    error = true;
    return QString();
  }
  QString line = (*currentIface);
  line = line.mid(QString(IFACE).length()+1, line.length());
  line = line.mid(name.length()+1, line.length());
  line = line.mid(family.length()+1, line.length());
  line = line.simplifyWhiteSpace();
  error = false;
  return line;
}

/**
 * Sets the interface name to newName.  
 * @param newName the new name of the interface.  All whitespace is removed.
 * @return bool true if successfull.
 */ 
bool Interfaces::setInterfaceName(QString newName){
  if(currentIface == interfaces.end())
    return false;
  newName = newName.simplifyWhiteSpace();
  newName = newName.replace(QRegExp(" "), "");
  bool returnValue = false;
  (*currentIface) = QString("iface %1 %2 %3").arg(newName).arg(getInterfaceFamily(returnValue)).arg(getInterfaceMethod(returnValue));
  return !returnValue;  
}

/**
 * Sets the interface family to newName. 
 * @param newName the new name of the interface.  Must be one of the families
 *  defined in the interfaces.h file.
 * @return bool true if successfull.
 */ 
bool Interfaces::setInterfaceFamily(QString newName){
  if(currentIface == interfaces.end())
    return false;
  if(acceptedFamily.contains(newName)==0)
    return false;
  bool returnValue = false;
  (*currentIface) = QString("iface %1 %2 %3").arg(getInterfaceName(returnValue)).arg(newName).arg(getInterfaceMethod(returnValue));
  return !returnValue;  
}

/**
 * Sets the interface method to newName
 * @param newName the new name of the interface
 * @return bool true if successfull.
 */ 
bool Interfaces::setInterfaceMethod(QString newName){
  if(currentIface == interfaces.end())
    return false;
  bool returnValue = false;
  (*currentIface) = QString("iface %1 %2 %3").arg(getInterfaceName(returnValue)).arg(getInterfaceFamily(returnValue)).arg(newName);
  return !returnValue;  
}

/**
 * Get a value for an option in the currently selected interface.  For example
 *  calling getInterfaceOption("address") on the following stanza would
 *  return 192.168.1.1.
 *  iface eth0 static
 *    address 192.168.1.1
 * @param option the options to get the value.
 * @param error set to true if any error occurs, false otherwise.
 * @return QString the options value. QString::null if error == true
 */ 
QString Interfaces::getInterfaceOption(QString option, bool &error){
  return getOption(currentIface, option, error);
}

/**
 * Set a value for an option in the currently selected interface.  If option
 *  doesn't exist then it is added along with the value. If value is set to an
 *  empty string then option is removed.
 * @param option the options to set the value.
 * @param value the value that option should be set to.
 * @param error set to true if any error occurs, false otherwise.
 * @return QString the options value. QString::null if error == true
 */ 
bool Interfaces::setInterfaceOption(QString option, QString value){
  return setOption(currentIface, option, value);
}

/**
 * Removes all of the options from the currently selected interface.
 * @return bool error if if successfull 
 */ 
bool Interfaces::removeAllInterfaceOptions(){
  return removeAllOptions(currentIface);
}

/**
 * Set the current map to interface's map.  This needs to be done before you
 *  can call addMapping(), set/getMap(), and get/setScript().
 * @param interface the name of the interface to set.  All whitespace is
 *  removed from the interface name.
 * @return bool true if it is successfull. 
 */ 
bool Interfaces::setMapping(QString interface){
  interface = interface.simplifyWhiteSpace();
  interface = interface.replace(QRegExp(" "), "");
  return setStanza(MAPPING, interface, currentMapping);
}

/**
 * Adds a new Mapping to the interfaces file with interfaces.
 * @param interface the name(s) of the interfaces to set to this mapping
 */ 
void Interfaces::addMapping(QString interfaces){
  interfaces.append("");
  interfaces.append(QString(MAPPING " %1").arg(interfaces));
}

/**
 * Set a map option within a mapping.
 * @param map map to use
 * @param value value to go with map
 * @return bool true if it is successfull.
 */ 
bool Interfaces::setMap(QString map, QString value){
  return setOption(currentMapping, map, value);
}

/**
 * Get a map value within a mapping.
 * @param map map to get value of
 * @param bool true if it is successfull.
 * @return value that goes to the map
 */ 
QString Interfaces::getMap(QString map, bool &error){
  return getOption(currentMapping, map, error);
}

/**
 * Sets a script value of the current mapping to argument.
 * @param argument the script name.
 * @return true if successfull.
 */ 
bool Interfaces::setScript(QString argument){
  return setOption(currentMapping, "script", argument);
}

/**
 * @param error true if could not retrieve the current script argument.
 * @return QString the argument of the script for the current mapping.
 */ 
QString Interfaces::getScript(bool &error){
  return getOption(currentMapping, "script", error);
}

/**
 * Helper function used to parse through the QStringList and put pointers in 
 *  the correct place.
 * @param stanza The stanza (auto, iface, mapping) to look for.
 * @param option string that must be in the stanza's main line.
 * @param interator interator to place at location of stanza if successfull.
 * @return bool true if the stanza is found.
 */ 
bool Interfaces::setStanza(QString stanza, QString option, QStringList::Iterator &iterator){
  bool found = false;
  iterator = interfaces.end();
  for ( QStringList::Iterator it = interfaces.begin(); it != interfaces.end(); ++it ) {
    QString line = (*it).simplifyWhiteSpace();
    if(line.contains(stanza) && line.contains(option)){
      if(found == true){
        qDebug(QString("Interfaces: Found multiple stanza's for search: %1 %2").arg(stanza).arg(option).latin1());
      }
      found = true;
      iterator = it;
    }
  }
  return !found;
}

/**
 * Sets a value of an option in a stanza
 * @param start the start of the stanza
 * @param option the option to use when setting value.
 * @return bool true if successfull, false otherwise.
 */
bool Interfaces::setOption(QStringList::Iterator start, QString option, QString value){
  if(start == interfaces.end())
    return false;
  
  bool found = false;
  for ( QStringList::Iterator it = start; it != interfaces.end(); ++it ) {
    if(((*it).contains(IFACE) || (*it).contains(MAPPING) || (*it).contains(AUTO))  && it != start){
      if(!found && value != ""){
        // Got to the end of the stanza without finding it, so append it.
        interfaces.insert(--it, QString("\t%1 %2").arg(option).arg(value));
      }
      break;
    }
    if((*it).contains(option)){
      // Found it in stanza so replace it.
      if(found)
        qDebug(QString("Interfaces: Set Options found more then one value for option: %1 in stanza: %1").arg(option).arg((*start)).latin1());
      found = true;
      if(value == "")
        (*it) = "";
      else      
        (*it) = QString("\t%1 %2").arg(option).arg(value);
    }
  }
  return true;
}

/**
 * Removes all options in a stanza
 * @param start the start of the stanza
 * @return bool true if successfull, false otherwise.
 */
bool Interfaces::removeAllOptions(QStringList::Iterator start){
  if(start == interfaces.end())
    return false;
  
  QStringList::Iterator it = start;
  it = ++it;
  for (it; it != interfaces.end(); ++it ) {
    if(((*it).contains(IFACE) || (*it).contains(MAPPING) || (*it).contains(AUTO))  && it != start){
      break;
    }
    it = interfaces.remove(it);
    it = --it;
  }
  // Leave a space between this interface and the next.
  interfaces.insert(it, QString(""));
  return true;
}

/**
 * Gets a value of an option in a stanza
 * @param start the start of the stanza
 * @param option the option to use when getting the value.
 * @param bool true if errors false otherwise.
 * @return QString the value of option QString::null() if error == true.
 */
QString Interfaces::getOption(QStringList::Iterator start, QString option, bool &error){
  if(start == interfaces.end()){
    error = false;
    return QString();
  }
  
  QString value;
  bool found = false;
  for ( QStringList::Iterator it = start; it != interfaces.end(); ++it ) {
    if(((*it).contains(IFACE) || (*it).contains(MAPPING) || (*it).contains(AUTO))  && it != start){
      break;
    }
    if((*it).contains(option)){
      if(found)
        qDebug(QString("Interfaces: Get Options found more then one value: %1 for option: %2 in stanza %3").arg((*it)).arg(option).arg((*start)).latin1());
      found = true;
      QString line = (*it).simplifyWhiteSpace();
      int space = line.find(" ", option.length());
      if(space != -1)
        value = line.mid(space+1, line.length());
      else
        qDebug(QString("Interfaces: Option %1 with no value").arg(option).latin1());
    }
  }
  error = !found;
  return value;
}

/**
 * Write out the interfaces file to the file passed into the constructor.
 * Removes any excess blank lines over 1 line long.
 * @return bool true if successfull, false if not.
 */ 
bool Interfaces::write(){
  QFile::remove(interfacesFile);
  QFile file(interfacesFile);

  if (!file.open(IO_ReadWrite)){
    qDebug(QString("Interfaces: Can't open file: %1 for writing.").arg(interfacesFile).latin1());
    return false;
  }
  QTextStream stream( &file );
  int whiteSpaceCount = 0;
  for ( QStringList::Iterator it = interfaces.begin(); it != interfaces.end(); ++it ) {
    QString line = (*it).simplifyWhiteSpace();
    line = line.replace(QRegExp(" "),"");
    if(line.length() == 0)
      whiteSpaceCount++;
    else
      whiteSpaceCount = 0;
    if(whiteSpaceCount < 2){
      qDebug((*it).latin1());
      stream << (*it) << '\n';
    }
  }
  file.close();
  return true;
}

// interfaces.cpp

