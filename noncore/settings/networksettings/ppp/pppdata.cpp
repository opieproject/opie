/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: pppdata.cpp,v 1.4 2003-05-25 14:25:23 tille Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "pppdata.h"
#include "runtests.h"
#include "devices.h"
//#include <klocale.h>
#define i18n QObject::tr
#include <qpe/config.h>
#include <qmessagebox.h>
#include <qapplication.h>
// #include <klocale.h>
// #include <kconfig.h>
// #include <kmessagebox.h>
// #include <kapplication.h>
#include <assert.h>

PPPData *PPPData::_data = 0;
Config *PPPData::config = 0;

PPPData* PPPData::data()
{
    if (!_data){
        qDebug("PPPData::data() creates new Instance");
        _data = new PPPData();
    }
    if (!_data->config){
        qDebug("PPPData::data() opens conffile");
        _data->open();
    }
    return _data;
}

PPPData::PPPData()
    :  //config(0L),
     highcount(-1),        // start out with no entries
     caccount(-1),         // set the current account index also
     suidprocessid(-1),    // process ID of setuid child
     pppdisrunning(false),
     pppderror(0),
     modemDeviceGroup(-1)
{
}


//
// open configuration file
//
bool PPPData::open() {
    qDebug("opening configfile NetworkSetupPPP");
    if (config) return true;
    config = new Config("NetworkSetupPPP");

    highcount = readNumConfig(GENERAL_GRP, NUMACCOUNTS_KEY, 0) - 1;

  if (highcount > MAX_ACCOUNTS)
    highcount = MAX_ACCOUNTS;

  if(highcount >= 0 && defaultAccount().isEmpty()) {
    setAccountbyIndex(0);
    setDefaultAccount(accname());
  } else if(!setAccount(defaultAccount()))
    setDefaultAccount(accname());

  // start out with internal debugging disabled
  // the user is still free to specify `debug' on his own
  setPPPDebug(false);

  ::pppdVersion(&pppdVer, &pppdMod, &pppdPatch);

  return true;
}


//
// save configuration
//
void PPPData::save() {

  if (config) {
    writeConfig(GENERAL_GRP, NUMACCOUNTS_KEY, count());
    delete config;
    config = 0;
    qDebug("worte confi NetworkSetupPPP");
  }
  if (_data){
    delete _data;
    _data = 0;
  }

}


//
// cancel changes
//
void PPPData::cancel() {

//   if (config) {
//     config->rollback();
//     config->reparseConfiguration();
//   }

}


// // currently differentiates between READWRITE and NONE only
// int PPPData::access() const {

//     return 1;//config->getConfigState();
// }


// functions to read/write date to configuration file
QString PPPData::readConfig(const QString &group, const QString &key,
                            const QString &defvalue = "")
{
//    qDebug("PPPData::readConfig key >%s< group >%s<",key.latin1(), group.latin1());
  if (config) {
    config->setGroup(group);
    return config->readEntry(key, defvalue);
  } else
    return defvalue;
}


int PPPData::readNumConfig(const QString &group, const QString &key,
			   int defvalue) {
  if (config) {
    config->setGroup(group);
    return config->readNumEntry(key, defvalue);
  } else
    return defvalue;

}


bool PPPData::readListConfig(const QString &group, const QString &key,
                             QStringList &list, char sep) {
  list.clear();
  if (config) {
    config->setGroup(group);
    list = config->readListEntry(key, sep);
    return true;
  } else
    return false;
}


void PPPData::writeConfig(const QString &group, const QString &key,
			  const QString &value) {
  if (config) {
    config->setGroup(group);
    config->writeEntry(key, value);
  }
}


void PPPData::writeConfig(const QString &group, const QString &key, int value) {
  if (config) {
    config->setGroup(group);
    config->writeEntry(key, value);
  }
}


void PPPData::writeListConfig(const QString &group, const QString &key,
                              QStringList &list, char sep) {
  if (config) {
    config->setGroup(group);
    config->writeEntry(key, list, sep);
  }
}


//
// functions to set/return general information
//
QString PPPData::password() const {
  return passwd;
}


void PPPData::setPassword(const QString &pw) {
  passwd = pw;
}


const QString PPPData::defaultAccount() {
  return readConfig(GENERAL_GRP, DEFAULTACCOUNT_KEY);
}


void PPPData::setDefaultAccount(const QString &n) {
  writeConfig(GENERAL_GRP, DEFAULTACCOUNT_KEY, n);

  //now set the current account index to the default account
  setAccount(defaultAccount());
}


bool PPPData::get_show_clock_on_caption() {
  return (bool) readNumConfig(GENERAL_GRP, SHOWCLOCK_KEY, true);
}


void PPPData::set_show_clock_on_caption(bool set) {
  writeConfig(GENERAL_GRP, SHOWCLOCK_KEY, (int) set);
}


bool PPPData::get_xserver_exit_disconnect() {
  return (bool) readNumConfig(GENERAL_GRP, DISCONNECT_KEY, true);
}


void PPPData::setPPPDebug(bool set) {
  writeConfig(GENERAL_GRP, PPP_DEBUG_OPTION, (int)set);
}


bool PPPData::getPPPDebug() {
  return (bool)readNumConfig(GENERAL_GRP, PPP_DEBUG_OPTION, false);
}


void PPPData::set_xserver_exit_disconnect(bool set) {
  writeConfig(GENERAL_GRP, DISCONNECT_KEY, (int) set);
}


bool PPPData::quit_on_disconnect() {
  return (bool) readNumConfig(GENERAL_GRP, QUITONDISCONNECT_KEY, false);
}


void PPPData::set_quit_on_disconnect(bool set) {
  writeConfig(GENERAL_GRP, QUITONDISCONNECT_KEY, (int) set);
}


bool PPPData::get_show_log_window() {
  return (bool) readNumConfig (GENERAL_GRP, SHOWLOGWIN_KEY, false);
}


void PPPData::set_show_log_window(bool set) {
  writeConfig(GENERAL_GRP, SHOWLOGWIN_KEY, (int) set);
}


bool PPPData::automatic_redial() {
  return (bool) readNumConfig(GENERAL_GRP, AUTOREDIAL_KEY, FALSE);
}


void PPPData::set_automatic_redial(bool set) {
  writeConfig(GENERAL_GRP, AUTOREDIAL_KEY, (int) set);
}


bool PPPData::get_iconify_on_connect() {
  return (bool) readNumConfig(GENERAL_GRP, ICONIFY_ON_CONNECT_KEY, TRUE);
}


void PPPData::set_iconify_on_connect(bool set) {
  writeConfig(GENERAL_GRP, ICONIFY_ON_CONNECT_KEY, (int) set);
}


bool PPPData::get_dock_into_panel() {
  return (bool) readNumConfig(GENERAL_GRP, DOCKING_KEY, false);
}


void PPPData::set_dock_into_panel(bool set) {
  writeConfig(GENERAL_GRP, DOCKING_KEY, (int) set);
}


QString PPPData::pppdVersion() {
  return QString("%1.%2.%3").arg(pppdVer).arg(pppdMod).arg(pppdPatch);
}

bool PPPData::pppdVersionMin(int ver, int mod, int patch) {
  // check if pppd version fulfills minimum requirement
  return (pppdVer > ver
          || (pppdVer == ver && pppdMod > mod)
          || (pppdVer == ver && pppdMod == mod && pppdPatch >= patch));
}

int PPPData::pppdTimeout() {
  return readNumConfig(GENERAL_GRP, PPPDTIMEOUT_KEY, PPPD_TIMEOUT);
}


void PPPData::setpppdTimeout(int n) {
  writeConfig(GENERAL_GRP, PPPDTIMEOUT_KEY, n);
}


const QString PPPData::modemDevice() {
  return readConfig (modemGroup(), MODEMDEV_KEY, devices[DEV_DEFAULT]);
}


bool PPPData::setModemDevice(const QString &n) {
    //FIXME: change modem group
    bool ret = false;
    for (int i = 0; devices[i]; i++)
        if (devices[i] == n){
            modemDeviceGroup = i;
            writeConfig(modemGroup(), MODEMDEV_KEY, n);
            ret = true;
        }
    return ret;
}


const QString PPPData::flowcontrol() {
  return readConfig(modemGroup(), FLOWCONTROL_KEY, "CRTSCTS");
}


void PPPData::setFlowcontrol(const QString &n) {
  writeConfig(modemGroup(), FLOWCONTROL_KEY, n);
}


const QString PPPData::speed() {
  QString s = readConfig(modemGroup(), SPEED_KEY, "57600");
  // undo the damage of a bug in former versions. It left an empty Speed=
  // entry in kppprc. kppp did set the serial port to 57600 as default but
  // pppd wouldn't receive the speed via the command line.
  if(s.toUInt() == 0)
    s = "57600";
  return s;
}


void PPPData::setSpeed(const QString &n) {
  writeConfig(modemGroup(), SPEED_KEY, n);
}


#if 0
void PPPData::setUseCDLine(const int n) {
  writeConfig(modemGroup(),USECDLINE_KEY,n);
}


int PPPData::UseCDLine() {
  return  readNumConfig(modemGroup(),USECDLINE_KEY,0);
}
#endif

const QString  PPPData::modemEscapeStr() {
  return readConfig(modemGroup(),ESCAPESTR_KEY,"+++");
}


void PPPData::setModemEscapeStr(const QString &n) {
  writeConfig(modemGroup(),ESCAPESTR_KEY,n);
}


const QString  PPPData::modemEscapeResp() {
  return readConfig(modemGroup(),ESCAPERESP_KEY,"OK");
}


void PPPData::setModemEscapeResp(const QString &n) {
  writeConfig(modemGroup(),ESCAPERESP_KEY,n);
}


int  PPPData::modemEscapeGuardTime() {
  return readNumConfig(modemGroup(),ESCAPEGUARDTIME_KEY,50);
}


void PPPData::setModemEscapeGuardTime(int n) {
  writeConfig(modemGroup(),ESCAPEGUARDTIME_KEY,n);
}


bool PPPData::modemLockFile() {
  return readNumConfig(modemGroup(), LOCKFILE_KEY, 1);
}


void PPPData::setModemLockFile(bool set) {
  writeConfig(modemGroup(), LOCKFILE_KEY, set);
}


int PPPData::modemTimeout() {
  return readNumConfig(modemGroup(), TIMEOUT_KEY, MODEM_TIMEOUT);
}


void PPPData::setModemTimeout(int n) {
  writeConfig(modemGroup(), TIMEOUT_KEY, n);
}


int PPPData::modemToneDuration() {
  return readNumConfig(modemGroup(), TONEDURATION_KEY,MODEM_TONEDURATION);
}


void PPPData::setModemToneDuration(int n) {
  writeConfig(modemGroup(), TONEDURATION_KEY, n);
}


int PPPData::busyWait() {
  return readNumConfig(modemGroup(), BUSYWAIT_KEY, BUSY_WAIT);
}


void PPPData::setbusyWait(int n) {
  writeConfig(modemGroup(), BUSYWAIT_KEY, n);
}


//
//Advanced "Modem" dialog
//
// defaults: InitString=ATZ, InitString1="" etc.
const QString PPPData::modemInitStr(int i) {
  assert(i >= 0 && i < NumInitStrings);
  if(i == 0)
    return readConfig(modemGroup(), INITSTR_KEY, "ATZ");
  else
    return readConfig(modemGroup(), INITSTR_KEY + QString::number(i), "");
}


void PPPData::setModemInitStr(int i, const QString &n) {
  assert(i >= 0 && i < NumInitStrings);
  QString k = INITSTR_KEY + (i > 0 ? QString::number(i) : "");
  writeConfig(modemGroup(), k, n);
}


const QString PPPData::modemInitResp() {
  return readConfig(modemGroup(), INITRESP_KEY, "OK");
}


void PPPData::setModemInitResp(const QString &n) {
  writeConfig(modemGroup(), INITRESP_KEY, n);
}


int PPPData::modemPreInitDelay() {
  return readNumConfig(modemGroup(), PREINITDELAY_KEY, 50);
}


void PPPData::setModemPreInitDelay(int n) {
  writeConfig(modemGroup(), PREINITDELAY_KEY, n);
}


int PPPData::modemInitDelay() {
  return readNumConfig(modemGroup(), INITDELAY_KEY, 50);
}


void PPPData::setModemInitDelay(int n) {
  writeConfig(modemGroup(), INITDELAY_KEY, n);
}

QString PPPData::modemNoDialToneDetectionStr() {
  return readConfig(modemGroup(), NODTDETECT_KEY, "ATX3");
}

void PPPData::setModemNoDialToneDetectionStr(const QString &n) {
  writeConfig(modemGroup(), NODTDETECT_KEY, n);
}

const QString PPPData::modemDialStr() {
  return readConfig(modemGroup(), DIALSTR_KEY, "ATDT");
}


void PPPData::setModemDialStr(const QString &n) {
  writeConfig(modemGroup(), DIALSTR_KEY, n);
}


const QString PPPData::modemConnectResp() {
  return readConfig(modemGroup(), CONNECTRESP_KEY, "CONNECT");
}


void PPPData::setModemConnectResp(const QString &n) {
  writeConfig(modemGroup(), CONNECTRESP_KEY, n);
}


const QString PPPData::modemBusyResp() {
  return readConfig(modemGroup(), BUSYRESP_KEY, "BUSY");
}


void PPPData::setModemBusyResp(const QString &n) {
  writeConfig(modemGroup(), BUSYRESP_KEY, n);
}


const QString PPPData::modemNoCarrierResp() {
  return readConfig(modemGroup(), NOCARRIERRESP_KEY, "NO CARRIER");
}


void PPPData::setModemNoCarrierResp(const QString &n) {
  writeConfig(modemGroup(), NOCARRIERRESP_KEY, n);
}


const QString PPPData::modemNoDialtoneResp() {
  return readConfig(modemGroup(), NODIALTONERESP_KEY, "NO DIALTONE");
}


void PPPData::setModemNoDialtoneResp(const QString &n) {
  writeConfig(modemGroup(), NODIALTONERESP_KEY, n);
}


const QString PPPData::modemHangupStr() {
  return readConfig(modemGroup(), HANGUPSTR_KEY, "+++ATH");
}

void PPPData::setModemHangupStr(const QString &n) {
  writeConfig(modemGroup(), HANGUPSTR_KEY, n);
}


const QString PPPData::modemHangupResp() {
  return readConfig(modemGroup(), HANGUPRESP_KEY, "OK");
}

void PPPData::setModemHangupResp(const QString &n) {
  writeConfig(modemGroup(), HANGUPRESP_KEY, n);
}


const QString PPPData::modemAnswerStr() {
  return readConfig(modemGroup(), ANSWERSTR_KEY, "ATA");
}


QString PPPData::volumeOff() {
  return readConfig(modemGroup(), VOLUME_OFF, "M0L0");
}


void PPPData::setVolumeOff(const QString &s) {
  writeConfig(modemGroup(), VOLUME_OFF, s);
}


QString PPPData::volumeMedium() {
 return readConfig(modemGroup(), VOLUME_MEDIUM, "M1L1");
}


void PPPData::setVolumeMedium(const QString &s) {
  writeConfig(modemGroup(), VOLUME_MEDIUM, s);
}


QString PPPData::volumeHigh() {
  QString tmp = readConfig(modemGroup(), VOLUME_HIGH, "M1L3");
  if(tmp == "M1L4")
    tmp = "M1L3";
  return tmp;
}


void PPPData::setVolumeHigh(const QString &s) {
 writeConfig(modemGroup(), VOLUME_HIGH, s);
}


QString PPPData::volumeInitString() {
  QString s;

  switch(volume()) {
  case 0:
    s = volumeOff();
    break;
  case 1:
    s = volumeMedium();
    break;
  case 2:
    s = volumeHigh();
    break;
  default:
    s = volumeMedium();
  }

  return s;
}


int PPPData::volume() {
  return readNumConfig(modemGroup(), VOLUME_KEY, 1);
}


void PPPData::setVolume(int i) {
  writeConfig(modemGroup(), VOLUME_KEY, i);
}

int PPPData::waitForDialTone() {
  return readNumConfig(modemGroup(), DIALTONEWAIT_KEY, 1);
}

void PPPData::setWaitForDialTone(int i) {
  writeConfig(modemGroup(), DIALTONEWAIT_KEY, i);
}

void PPPData::setModemAnswerStr(const QString &n) {
  writeConfig(modemGroup(), ANSWERSTR_KEY, n);
}


const QString PPPData::modemRingResp() {
  return readConfig(modemGroup(), RINGRESP_KEY, "RING");
}


void PPPData::setModemRingResp(const QString &n) {
  writeConfig(modemGroup(), RINGRESP_KEY, n);
}


const QString PPPData::modemAnswerResp() {
  return readConfig(modemGroup(), ANSWERRESP_KEY, "CONNECT");
}


void PPPData::setModemAnswerResp(const QString &n) {
  writeConfig(modemGroup(), ANSWERRESP_KEY, n);
}


const QString PPPData::enter() {
  return readConfig(modemGroup(), ENTER_KEY, "CR");
}


void PPPData::setEnter(const QString &n) {
  writeConfig(modemGroup(), ENTER_KEY, n);
}


//
// functions to set/return account information
//

//returns number of accounts
int PPPData::count() const {
  return highcount + 1;
}


bool PPPData::setAccount(const QString &aname) {
  for(int i = 0; i <= highcount; i++) {
    setAccountbyIndex(i);
    if(accname() == aname) {
      caccount = i;
      return true;
    }
  }
  return false;
}


bool PPPData::setAccountbyIndex(int i) {
  if(i >= 0 && i <= highcount) {
    caccount = i;
    cgroup.sprintf("%s%i", ACCOUNT_GRP, i);
    return true;
  }
  return false;
}


bool PPPData::isUniqueAccname(const QString &n) {
  int current = caccount;
  for(int i=0; i <= highcount; i++) {
    setAccountbyIndex(i);
    if(accname() == n && i != current) {
      setAccountbyIndex(current);
      return false;
    }
  }
  setAccountbyIndex(current);
  return true;
}


bool PPPData::deleteAccount() {
    //FIXME:
//   if(caccount < 0)
     return false;

//   QMap <QString, QString> map;
//   QMap <QString, QString>::Iterator it;

//   // set all entries of the current account to ""
//   map = config->entryMap(cgroup);
//   it = map.begin();
//   while (it != map.end()) {
//     config->writeEntry(it.key(), "");
//     it++;
//   }

//   // shift the succeeding accounts
//   for(int i = caccount+1; i <= highcount; i++) {
//     setAccountbyIndex(i);
//     map = config->entryMap(cgroup);
//     it = map.begin();
//     setAccountbyIndex(i-1);
//     config->setGroup(cgroup);
//     while (it != map.end()) {
//       config->writeEntry(it.key(), *it);
//       it++;
//     }
//   }

//   // make sure the top account is cleared
//   setAccountbyIndex(highcount);
//   map = config->entryMap(cgroup);
//   it = map.begin();
//   config->setGroup(cgroup);
//   while (it.key() != QString::null) {
//     config->writeEntry(it.key(), "");
//     it++;
//   }

//    highcount--;
//    if(caccount > highcount)
//      caccount = highcount;

//   setAccountbyIndex(caccount);

//   return true;
}


bool PPPData::deleteAccount(const QString &aname) {
  if(!setAccount(aname))
    return false;

  deleteAccount();

  return true;
}


int PPPData::newaccount() {

    qDebug("PPPData::newaccount highcount %i/%i",highcount,MAX_ACCOUNTS);
  if(!config) open();
  if (highcount >= MAX_ACCOUNTS) return -1;

  highcount++;
  setAccountbyIndex(highcount);

  setpppdArgumentDefaults();
  qDebug("PPPData::newaccount -> %i",caccount);
  return caccount;
}

int PPPData::copyaccount(int i) {

//  if(highcount >= MAX_ACCOUNTS)
    return -1;

//   setAccountbyIndex(i);

//    QMap <QString, QString> map = config->entryMap(cgroup);
//    QMap <QString, QString>::ConstIterator it = map.begin();

//    QString newname = i18n("%1_copy").arg(accname());

//    newaccount();

//    while (it != map.end()) {
//      config->writeEntry(it.key(), *it);
//      it++;
//    }

//   setAccname(newname);

//   return caccount;
}


const QString PPPData::accname() {
  return readConfig(cgroup, NAME_KEY);
}

void PPPData::setAccname(const QString &n) {
  if(!cgroup.isNull()) {
    // are we manipulating the default account's name ? then change it, too.
    bool def = accname() == defaultAccount();
    writeConfig(cgroup, NAME_KEY, n);
    if (def)
      setDefaultAccount(n);
  }
}


#define SEPARATOR_CHAR '&'
QStringList &PPPData::phonenumbers() {

  readListConfig(cgroup, PHONENUMBER_KEY, phonelist, SEPARATOR_CHAR);
  return phonelist;

}


const QString PPPData::phonenumber() {
  return readConfig(cgroup, PHONENUMBER_KEY);
}


void PPPData::setPhonenumber(const QString &n) {
  writeConfig(cgroup, PHONENUMBER_KEY, n);
}


const QString PPPData::dialPrefix() {
  return readConfig(cgroup, DIAL_PREFIX_KEY, "");
}


void PPPData::setDialPrefix(const QString &s) {
  writeConfig(cgroup, DIAL_PREFIX_KEY, s);
}


int PPPData::authMethod() {
    return readNumConfig(cgroup, AUTH_KEY, 0);
}


void PPPData::setAuthMethod(int value) {
  writeConfig(cgroup, AUTH_KEY, value);
}


const QString  PPPData::storedUsername() {
  return readConfig(cgroup, STORED_USERNAME_KEY, "");
}


void PPPData::setStoredUsername(const QString &b) {
  writeConfig(cgroup, STORED_USERNAME_KEY, b);
}


const QString  PPPData::storedPassword() {
  return readConfig(cgroup, STORED_PASSWORD_KEY, "");
}


void PPPData::setStoredPassword(const QString &b) {
  writeConfig(cgroup, STORED_PASSWORD_KEY, b);
}


bool PPPData::storePassword() {
  return (bool)readNumConfig(cgroup, STORE_PASSWORD_KEY, 1);
}


const QString PPPData::command_before_connect() {
  return readConfig(cgroup, BEFORE_CONNECT_KEY);
}


void PPPData::setCommand_before_connect(const QString &n) {
  writeConfig(cgroup, BEFORE_CONNECT_KEY, n);
}


void PPPData::setStorePassword(bool b) {
  writeConfig(cgroup, STORE_PASSWORD_KEY, (int)b);
}


const QString PPPData::command_on_connect() {
  return readConfig(cgroup, COMMAND_KEY);
}


void PPPData::setCommand_on_connect(const QString &n) {
  writeConfig(cgroup, COMMAND_KEY, n);
}


const QString PPPData::command_on_disconnect() {
  return readConfig(cgroup, DISCONNECT_COMMAND_KEY);
}


void PPPData::setCommand_on_disconnect(const QString &n) {
  writeConfig(cgroup, DISCONNECT_COMMAND_KEY, n);
}


const QString PPPData::command_before_disconnect() {
  return readConfig(cgroup, BEFORE_DISCONNECT_KEY);
}


void PPPData::setCommand_before_disconnect(const QString &n) {
  writeConfig(cgroup, BEFORE_DISCONNECT_KEY, n);
}


const QString PPPData::ipaddr() {
  return readConfig(cgroup, IPADDR_KEY);
}


void PPPData::setIpaddr(const QString &n) {
  writeConfig(cgroup, IPADDR_KEY, n);
}


const QString PPPData::subnetmask() {
  return readConfig(cgroup, SUBNETMASK_KEY);
}


void PPPData::setSubnetmask(const QString &n) {
  writeConfig(cgroup, SUBNETMASK_KEY, n);
}


bool PPPData::autoname() {
  return (bool) readNumConfig(cgroup, AUTONAME_KEY, false);
}


void PPPData::setAutoname(bool set) {
  writeConfig(cgroup, AUTONAME_KEY, (int) set);
}


bool PPPData::AcctEnabled() {
  return (bool) readNumConfig(cgroup, ACCTENABLED_KEY, false);
}


void PPPData::setAcctEnabled(bool set) {
  writeConfig(cgroup, ACCTENABLED_KEY, (int) set);
}


int PPPData::VolAcctEnabled() {
  return readNumConfig(cgroup, VOLACCTENABLED_KEY, 0);
}


void PPPData::setVolAcctEnabled(int set) {
  writeConfig(cgroup, VOLACCTENABLED_KEY, set);
}


const QString PPPData::gateway() {
  return readConfig(cgroup, GATEWAY_KEY);
}


void PPPData::setGateway(const QString &n ) {
  writeConfig(cgroup, GATEWAY_KEY, n);
}


bool PPPData::defaultroute() {
  // default route is by default 'on'.
  return (bool) readNumConfig(cgroup, DEFAULTROUTE_KEY, true);
}


void PPPData::setDefaultroute(bool set) {
  writeConfig(cgroup, DEFAULTROUTE_KEY, (int) set);
}


bool PPPData::autoDNS() {
  bool set = (bool) readNumConfig(cgroup, AUTODNS_KEY, true);
  return (set && PPPData::data()->pppdVersionMin(2, 3, 7));
}


void PPPData::setAutoDNS(bool set) {
  writeConfig(cgroup, AUTODNS_KEY, (int) set);
}


void PPPData::setExDNSDisabled(bool set) {
  writeConfig(cgroup, EXDNSDISABLED_KEY, (int) set);
}


bool PPPData::exDNSDisabled() {
  return (bool) readNumConfig(cgroup, EXDNSDISABLED_KEY,0);
}


QStringList &PPPData::dns() {
  static QStringList dnslist;

  readListConfig(cgroup, DNS_KEY, dnslist);
  while(dnslist.count() > MAX_DNS_ENTRIES)
    dnslist.remove(dnslist.last());

  return dnslist;
}


void PPPData::setDns(QStringList &list) {
  writeListConfig(cgroup, DNS_KEY, list);
}


const QString PPPData::domain() {
  return readConfig(cgroup, DOMAIN_KEY);
}


void PPPData::setDomain(const QString &n ) {
  writeConfig(cgroup, DOMAIN_KEY, n);
}


QStringList &PPPData::scriptType() {
  static QStringList typelist;

  readListConfig(cgroup, SCRIPTCOM_KEY, typelist);
  while(typelist.count() > MAX_SCRIPT_ENTRIES)
    typelist.remove(typelist.last());

  return typelist;
}


void PPPData::setScriptType(QStringList &list) {
  writeListConfig(cgroup, SCRIPTCOM_KEY, list);
}


QStringList &PPPData::script() {
  static QStringList scriptlist;

  readListConfig(cgroup, SCRIPTARG_KEY, scriptlist);
  while(scriptlist.count() > MAX_SCRIPT_ENTRIES)
    scriptlist.remove(scriptlist.last());

  return scriptlist;
}


void PPPData::setScript(QStringList &list) {
  writeListConfig(cgroup, SCRIPTARG_KEY, list);
}


// const QString PPPData::accountingFile() {
//   return readConfig(cgroup, ACCTFILE_KEY);
// }


// void PPPData::setAccountingFile(const QString &n) {
//   writeConfig(cgroup, ACCTFILE_KEY, n);
// }


// const QString PPPData::totalCosts() {
//   return readConfig(cgroup, TOTALCOSTS_KEY);
// }


// void PPPData::setTotalCosts(const QString &n) {
//   writeConfig(cgroup, TOTALCOSTS_KEY, n);
// }


// int PPPData::totalBytes() {
//   return readNumConfig(cgroup, TOTALBYTES_KEY, 0);
// }

// void PPPData::setTotalBytes(int n) {
//   writeConfig(cgroup, TOTALBYTES_KEY, n);
// }


QStringList &PPPData::pppdArgument() {
  static QStringList arglist;

  while(arglist.count() > MAX_PPPD_ARGUMENTS)
    arglist.remove(arglist.last());
  readListConfig(cgroup, PPPDARG_KEY, arglist);

  return arglist;
}


void PPPData::setpppdArgument(QStringList &args) {
  writeListConfig(cgroup, PPPDARG_KEY, args);
}


void PPPData::setpppdArgumentDefaults() {
  QStringList arg;
  setpppdArgument(arg);
}


// // graphing widget
// void PPPData::setGraphingOptions(bool enable,
// 				 QColor bg,
// 				 QColor text,
// 				 QColor in,
// 				 QColor out)
// {
//   if(config) {
//     config->setGroup(GRAPH_GRP);
//     config->writeEntry(GENABLED, enable);
// //     config->writeEntry(GCOLOR_BG, bg);
// //     config->writeEntry(GCOLOR_TEXT, text);
// //     config->writeEntry(GCOLOR_IN, in);
// //     config->writeEntry(GCOLOR_OUT, out);
//   }
// }

// void PPPData::graphingOptions(bool &enable,
// 			      QColor &bg,
// 			      QColor &text,
// 			      QColor &in,
// 			      QColor &out)
// {
//   QColor c;

//   if(config) {
//     config->setGroup(GRAPH_GRP);
//     enable = config->readBoolEntry(GENABLED, true);
//     bg = Qt::white;
//     //bg = config->readColorEntry(GCOLOR_BG, &c);
//     text = Qt::black;
//     //text = config->readColorEntry(GCOLOR_TEXT, &c);
//     in = Qt::blue;
//     //in = config->readColorEntry(GCOLOR_IN, &c);
//     out = Qt::red;
//     //out = config->readColorEntry(GCOLOR_OUT, &c);
//   }
// }


// bool PPPData::graphingEnabled() {
//   return (bool) readNumConfig(GRAPH_GRP, GENABLED, true);
// }



//
//functions to change/set the child pppd process info
//
bool PPPData::pppdRunning() const {
  return pppdisrunning;
}

void PPPData::setpppdRunning(bool set) {
  pppdisrunning = set;
}

int PPPData::pppdError() const {
  return pppderror;
}

void PPPData::setpppdError(int err) {
  pppderror = err;
}

QString PPPData::modemGroup()
{
    if (modemDeviceGroup<0)qFatal("wrong modem %i",modemDeviceGroup);
    return QString("MODEM_GRP_%1").arg(modemDeviceGroup);
}

// //
// // window position
// //
// void PPPData::winPosConWin(int& p_x, int& p_y) {
//   p_x = readNumConfig(WINPOS_GRP, WINPOS_CONWIN_X, QApplication::desktop()->width()/2-160);
//   p_y = readNumConfig(WINPOS_GRP, WINPOS_CONWIN_Y, QApplication::desktop()->height()/2-55);
// }

// void PPPData::setWinPosConWin(int p_x, int p_y) {
//   writeConfig(WINPOS_GRP, WINPOS_CONWIN_X, p_x);
//   writeConfig(WINPOS_GRP, WINPOS_CONWIN_Y, p_y);
// }

// void PPPData::winPosStatWin(int& p_x, int& p_y) {
//   p_x = readNumConfig(WINPOS_GRP, WINPOS_STATWIN_X, QApplication::desktop()->width()/2-160);
//   p_y = readNumConfig(WINPOS_GRP, WINPOS_STATWIN_Y, QApplication::desktop()->height()/2-55);
// }

// void PPPData::setWinPosStatWin(int p_x, int p_y) {
//   writeConfig(WINPOS_GRP, WINPOS_STATWIN_X, p_x);
//   writeConfig(WINPOS_GRP, WINPOS_STATWIN_Y, p_y);
// }
