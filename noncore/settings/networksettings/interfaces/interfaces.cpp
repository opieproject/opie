#include "interfaces.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>

// The three stanza's
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
 * Get a list of all interfaces in the interface file.  Usefull for
 * hardware that is not currently connected such as an 802.11b card 
 * not plugged in, but configured for when it is plugged in.
 * @return Return string list of interfaces.
 **/ 
QStringList Interfaces::getInterfaceList(){
  QStringList list;
  for ( QStringList::Iterator it = interfaces.begin(); it != interfaces.end(); ++it ) {
    QString line = (*it).simplifyWhiteSpace();
    if(line.contains(IFACE) && line.at(0) != '#'){
      line = line.mid(QString(IFACE).length() +1, line.length());
      line = line.simplifyWhiteSpace();
      int findSpace = line.find(" ");
      if( findSpace >= 0){
        line = line.mid(0, findSpace);
        list.append(line);
      }
    }
  }
  return list;
}

/**
 * Find out if interface is in an "auto" group or not.
 *  Report any duplicates such as eth0 being in two differnt auto's 
 * @param interface interface to check to see if it is on or not.
 * @return true is interface is in auto
 */ 
bool Interfaces::isAuto(const QString &interface) const {
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
bool Interfaces::setAuto(const QString &interface, bool setAuto){
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
      // else see if we need to remove from this one
      else{
        if((*it).contains(interface)){
          (*it) = (*it).replace(QRegExp(interface), "");
          // if AUTO is the only thing left clear the line
          if(((*it).simplifyWhiteSpace()).replace(QRegExp(" "),"") == AUTO)
            (*it) = "";
          changed = true;
	  // Don't break because we want to make sure we remove all cases.
        }
      }
    }
  }
  // In the case where there is no AUTO field add one.
  if(!changed && setAuto)
    interfaces.append(QString(AUTO" %1").arg(interface));
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
bool Interfaces::isInterfaceSet() const {
  return (interfaces.end() != currentIface);
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
bool Interfaces::addInterface(const QString &interface, const QString &family, const QString &method){
  if(0 == acceptedFamily.contains(family))
    return false;
  QString newInterface = interface.simplifyWhiteSpace();
  newInterface = newInterface.replace(QRegExp(" "), "");
  interfaces.append("");
  interfaces.append(QString(IFACE " %1 %2 %3").arg(newInterface).arg(family).arg(method));
  return true;
}

/**
 * Copies interface with name interface to name newInterface
 * @param newInterface name of the new interface.
 * @return bool true if successfull
 */ 
bool Interfaces::copyInterface(const QString &interface, const QString &newInterface){
  if(!setInterface(interface))
    return false;

  // Store the old interface and bump past the stanza line.
  QStringList::Iterator it = currentIface;
  it++;
  
  // Add the new interface
  bool error;
  addInterface(newInterface, getInterfaceFamily(error), getInterfaceMethod(error));
  if(!setInterface(newInterface))
    return false;
  
  QStringList::Iterator newIface = currentIface;
  newIface++;
 
  // Copy all of the lines
  for ( ; it != interfaces.end(); ++it ){
    if(((*it).contains(IFACE) || (*it).contains(MAPPING) || (*it).contains(AUTO)))
      break;
    newIface = interfaces.insert(newIface, *it);
  }
 
  return true;
}

/**
 * Remove the currently selected interface and all of its options.
 * @return bool if successfull or not.
 */ 
bool Interfaces::removeInterface(){
  return removeStanza(currentIface);
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
  if(error)
    return QString();
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
  if(error)
    return QString();
  QString family = getInterfaceFamily(error);
  if(error)
    return QString();
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
bool Interfaces::setInterfaceName(const QString &newName){
  if(currentIface == interfaces.end())
    return false;
  QString name = newName.simplifyWhiteSpace();
  name = name.replace(QRegExp(" "), "");
  bool returnValue = false;
  (*currentIface) = QString("iface %1 %2 %3").arg(name).arg(getInterfaceFamily(returnValue)).arg(getInterfaceMethod(returnValue));
  return !returnValue;  
}

/**
 * Sets the interface family to newName. 
 * @param newName the new name of the interface.  Must be one of the families
 *  defined in the interfaces.h file.
 * @return bool true if successfull.
 */ 
bool Interfaces::setInterfaceFamily(const QString &newName){
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
bool Interfaces::setInterfaceMethod(const QString &newName){
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
QString Interfaces::getInterfaceOption(const QString &option, bool &error){
  return getOption(currentIface, option, error);
}

/**
 * Set a value for an option in the currently selected interface.  If option
 *  doesn't exist then it is added along with the value.
 * @param option the options to set the value.
 * @param value the value that option should be set to.
 * @param error set to true if any error occurs, false otherwise.
 * @return QString the options value. QString::null if error == true
 */ 
bool Interfaces::setInterfaceOption(const QString &option, const QString &value){
  return setOption(currentIface, option, value);
}

/**
 * Removes a value for an option in the currently selected interface. 
 * @param option the options to set the value.
 * @param value the value that option should be set to.
 * @param error set to true if any error occurs, false otherwise.
 * @return QString the options value. QString::null if error == true
 */ 
bool Interfaces::removeInterfaceOption(const QString &option, const QString &value){
  return removeOption(currentIface, option, value);
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
bool Interfaces::setMapping(const QString &interface){
  QString interfaceName = interface.simplifyWhiteSpace();
  interfaceName = interfaceName.replace(QRegExp(" "), "");
  return setStanza(MAPPING, interfaceName, currentMapping);
}

/**
 * Adds a new Mapping to the interfaces file with interfaces.
 * @param interface the name(s) of the interfaces to set to this mapping
 */ 
void Interfaces::addMapping(const QString &option){
  interfaces.append("");
  interfaces.append(QString(MAPPING " %1").arg(option));
}

/**
 * Remove the currently selected map and all of its options.
 * @return bool if successfull or not.
 */
bool Interfaces::removeMapping(){
  return removeStanza(currentMapping);
} 

/**
 * Set a map option within a mapping.
 * @param map map to use
 * @param value value to go with map
 * @return bool true if it is successfull.
 */ 
bool Interfaces::setMap(const QString &map, const QString &value){
  return setOption(currentMapping, map, value);
}

/**
 * Removes a map option within a mapping.
 * @param map map to use
 * @param value value to go with map
 * @return bool true if it is successfull.
 */ 
bool Interfaces::removeMap(const QString &map, const QString &value){
  return removeOption(currentMapping, map, value);
}

/**
 * Get a map value within a mapping.
 * @param map map to get value of
 * @param bool true if it is successfull.
 * @return value that goes to the map
 */ 
QString Interfaces::getMap(const QString &map, bool &error){
  return getOption(currentMapping, map, error);
}

/**
 * Sets a script value of the current mapping to argument.
 * @param argument the script name.
 * @return true if successfull.
 */ 
bool Interfaces::setScript(const QString &argument){
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
bool Interfaces::setStanza(const QString &stanza, const QString &option, QStringList::Iterator &iterator){
  bool found = false;
  iterator = interfaces.end();
  for ( QStringList::Iterator it = interfaces.begin(); it != interfaces.end(); ++it ) {
    QString line = (*it).simplifyWhiteSpace();
    if(line.contains(stanza) && line.contains(option) && line.at(0) != '#'){
      uint point = line.find(option);
      bool valid = true;
      if(point > 0){
	// There are more chars in the line. check +1
        if(line.at(point-1) != ' ')
	  valid = false;
      }
      point += option.length();
      if(point < line.length()-1){
	// There are more chars in the line. check -1
        if(line.at(point) != ' ')
	  valid = false;
      }
      if(valid){
        if(found == true){
          qDebug(QString("Interfaces: Found multiple stanza's for search: %1 %2").arg(stanza).arg(option).latin1());
        }
        found = true;
        iterator = it;
      }
    }
  }
  return found;
}

/**
 * Sets a value of an option in a stanza
 * @param start the start of the stanza
 * @param option the option to use when setting value.
 * @return bool true if successfull, false otherwise.
 */
bool Interfaces::setOption(const QStringList::Iterator &start, const QString &option, const QString &value){
  if(start == interfaces.end())
    return false;
  
  bool found = false;
  for ( QStringList::Iterator it = start; it != interfaces.end(); ++it ) {
    if(((*it).contains(IFACE) || (*it).contains(MAPPING) || (*it).contains(AUTO))  && it != start){
      if(!found && value != ""){
        // Got to the end of the stanza without finding it, so append it.
        interfaces.insert(--it, QString("\t%1 %2").arg(option).arg(value));
      }
      found = true;
      break;
    }
    if((*it).contains(option) && it != start && (*it).at(0) != '#'){
      // Found it in stanza so replace it.
      if(found)
        qDebug(QString("Interfaces: Set Options found more then one value for option: %1 in stanza: %1").arg(option).arg((*start)).latin1());
      found = true;
        (*it) = QString("\t%1 %2").arg(option).arg(value);
    }
  }
  if(!found){
    QStringList::Iterator p = start;
    interfaces.insert(++p, QString("\t%1 %2").arg(option).arg(value));
    found = true;
  }
  return found;
}

/**
 * Removes a stanza and all of its options
 * @param stanza the stanza to remove
 * @return bool true if successfull.
 */ 
bool Interfaces::removeStanza(QStringList::Iterator &stanza){
  if(stanza == interfaces.end())
    return false;
  (*stanza) = "";
  return removeAllOptions(stanza);
}

/**
 * Removes a option in a stanza
 * @param start the start of the stanza
 * @param option the option to use when setting value.
 * @return bool true if successfull, false otherwise.
 */
bool Interfaces::removeOption(const QStringList::Iterator &start, const QString &option, const QString &value){
  if(start == interfaces.end())
    return false;
  
  bool found = false;
  for ( QStringList::Iterator it = start; it != interfaces.end(); ++it ) {
    if(((*it).contains(IFACE) || (*it).contains(MAPPING) || (*it).contains(AUTO))  && it != start){
      // got to the end without finding it
      break;
    }
    if((*it).contains(option) && (*it).contains(value) && it != start && (*it).at(0) != '#'){
      // Found it in stanza so replace it.
      if(found)
        qDebug(QString("Interfaces: Set Options found more then one value for option: %1 in stanza: %1").arg(option).arg((*start)).latin1());
      found = true;
      (*it) = "";
    }
  }
  return found;
}

/**
 * Removes all options in a stanza
 * @param start the start of the stanza
 * @return bool true if successfull, false otherwise.
 */
bool Interfaces::removeAllOptions(const QStringList::Iterator &start){
  if(start == interfaces.end())
    return false;
  
  QStringList::Iterator it = start;
  it = ++it;
  for (; it != interfaces.end(); ++it ) {
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
QString Interfaces::getOption(const QStringList::Iterator &start, const QString &option, bool &error){
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
    if((*it).contains(option) && (*it).at(0) != '#'){
      if(found)
        qDebug(QString("Interfaces: getOption found more then one value: %1 for option: %2 in stanza %3").arg((*it)).arg(option).arg((*start)).latin1());
      found = true;
      QString line = (*it).simplifyWhiteSpace();
      int space = line.find(" ", option.length());
      if(space != -1){
        value = line.mid(space+1, line.length());
        break;
      }
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

