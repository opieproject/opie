/* -*- C++ -*-
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: pppdata.h,v 1.10 2004-10-14 01:44:27 zecke Exp $
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

#ifndef _PPPDATA_H_
#define _PPPDATA_H_

#include <unistd.h>
#include <sys/types.h>

#include <qcolor.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>

#include "kpppconfig.h"

class Config;

// string lengths

#define PATH_SIZE 120
#define MODEMSTR_SIZE 80
#define ACCNAME_SIZE 50
#define PHONENUMBER_SIZE 60
#define COMMAND_SIZE 255
#define IPADDR_SIZE 15
#define DOMAIN_SIZE 50
#define TIMEOUT_SIZE 60

//
// keys for config file
//

// groups
#define GENERAL_GRP        "PPP_General"
#define MODEM_GRP          "PPP_Modem"
#define ACCOUNT_GRP        "PPP_Account"
#define ACCLIST_GRP        "PPP_Accounts_List"
//#define GRAPH_GRP          "Graph"
//#define WINPOS_GRP         "WindowPosition"

// general
#define ACCOUNT_LIST       "AccountList"
#define DEVICE_LIST        "DeviceList"
#define DEFAULTACCOUNT_KEY "DefaultAccount"
#define PPPDVERSION_KEY    "pppdVersion"
#define PPPDTIMEOUT_KEY    "pppdTimeout"
#define SHOWCLOCK_KEY      "ShowClock"
#define SHOWLOGWIN_KEY     "ShowLogWindow"
#define AUTOREDIAL_KEY     "AutomaticRedial"
#define DISCONNECT_KEY     "DisconnectOnXServerExit"
#define QUITONDISCONNECT_KEY "QuitOnDisconnect"
#define NUMACCOUNTS_KEY    "HighcountAccounts"
#define NUMDEVICES_KEY    "HighcountDevices"
#define ID_KEY		   "ID"

// modem
#define MODEMNAME_KEY      "Modem_Name"
#define MODEMDEV_KEY       "Device"
#define LOCKFILE_KEY       "UseLockFile"
#define FLOWCONTROL_KEY    "FlowControl"
#define SPEED_KEY          "Speed"
#define TIMEOUT_KEY        "Timeout"
#define TONEDURATION_KEY   "ToneDuration"
#define BUSYWAIT_KEY       "BusyWait"
#define INITSTR_KEY        "InitString"
#define INITRESP_KEY       "InitResponse"
#define PREINITDELAY_KEY   "PreInitDelay"
#define INITDELAY_KEY      "InitDelay"
#define NODTDETECT_KEY     "NoDialToneDetection"
#define DIALTONEWAIT_KEY   "WaitForDialTone"
#define DIALSTR_KEY        "DialString"
#define CONNECTRESP_KEY    "ConnectResponse"
#define BUSYRESP_KEY       "BusyResponse"
#define NOCARRIERRESP_KEY  "NoCarrierResponse"
#define NODIALTONERESP_KEY "NoDialToneResp"
#define HANGUPSTR_KEY      "HangupString"
#define HANGUPRESP_KEY     "HangUpResponse"
#define ANSWERSTR_KEY      "AnswerString"
#define RINGRESP_KEY       "RingResponse"
#define ANSWERRESP_KEY     "AnswerResponse"
#define ENTER_KEY          "Enter"
#define ESCAPESTR_KEY      "EscapeString"
#define ESCAPERESP_KEY     "EscapeResponse"
#define ESCAPEGUARDTIME_KEY "EscapeGuardTime"
#define USECDLINE_KEY      "UseCDLine"
#define VOLUME_HIGH        "VolumeHigh"
#define VOLUME_MEDIUM      "VolumeMedium"
#define VOLUME_OFF         "VolumeOff"
#define VOLUME_KEY         "Volume"

// account
#define NAME_KEY           "Name"
#define PHONENUMBER_KEY    "Phonenumber"
#define DIAL_PREFIX_KEY    "DialPrefix"
#define AUTH_KEY           "Authentication"
#define STORED_PASSWORD_KEY "Password"
#define STORED_USERNAME_KEY "Username"
#define STORE_PASSWORD_KEY "StorePassword"
#define BEFORE_CONNECT_KEY  "BeforeConnect"
#define COMMAND_KEY        "Command"
#define DISCONNECT_COMMAND_KEY "DisconnectCommand"
#define BEFORE_DISCONNECT_KEY  "BeforeDisconnect"
#define IPADDR_KEY         "IPAddr"
#define SUBNETMASK_KEY     "SubnetMask"
#define ACCTENABLED_KEY    "AccountingEnabled"
#define VOLACCTENABLED_KEY "VolumeAccountingEnabled"
#define ACCTFILE_KEY       "AccountingFile"
#define AUTONAME_KEY       "AutoName"
#define GATEWAY_KEY        "Gateway"
#define DEFAULTROUTE_KEY   "DefaultRoute"
#define DOMAIN_KEY         "Domain"
#define DNS_KEY            "DNS"
#define AUTODNS_KEY        "AutoDNS"
#define EXDNSDISABLED_KEY  "ExDNSDisabled"
#define SCRIPTCOM_KEY      "ScriptCommands"
#define SCRIPTARG_KEY      "ScriptArguments"
#define PPPDARG_KEY        "pppdArguments"
#define PPP_DEBUG_OPTION   "PPPDebug"
#define ICONIFY_ON_CONNECT_KEY "iconifyOnConnect"
#define DOCKING_KEY        "DockIntoPanel"
#define TOTALCOSTS_KEY     "TotalCosts"
#define TOTALBYTES_KEY     "TotalBytes"

// pppd errors
#define E_IF_TIMEOUT       1
#define E_PPPD_DIED        2

// account list
#define ACCOUNTS_COUNT     "Accounts_Count"
#define ACOUNTS_DEV        "Accounts_Modem"
#define ACOUNTS_ACC        "Accounts_Account"

#define DEVICESNAMES_LIST  "DevicesNames_List"
#define DEVICES_LIST       "Devices_List"
#define DEVICES_LIST_SEP   ','

class PPPData {
public:
    PPPData();
    ~PPPData() {};

    enum { NumInitStrings = 2 };
    enum LineTermination {
        EndCR,
        EndLF,
        EndCRLF
    };

    enum FlowControl {
        FlowHardware,
        FlowSoftware,
        FlowNone
    };

    // general functions
    void save();
    void cancel();

    QStringList getAccountList();

    static QMap<QString,QString> getConfiguredInterfaces();
    static void setConfiguredInterfaces( QMap<QString,QString> );

    // function to read/write date to configuration file
    static Config config();
    QString readConfig(const QString &, const QString &, const QString &);
    int readNumConfig(const QString &, const QString &, int);
    bool readListConfig(const QString &, const QString &,
                      QStringList &, char sep = ',');
    void writeConfig(const QString &, const QString &, const QString &);
    void writeConfig(const QString &, const QString &, int);
    void writeListConfig(const QString &, const QString &,
                       QStringList &, char sep = ',');

    // return the current account group
    QString currentGroup() { return cgroup; }
    QString modemGroup();

  // functions to set/get general kppp info
  QString password();
  void setPassword(const QString &);

//  int currentAccountID() { return caccount; };
  const QString defaultAccount();
  void setDefaultAccount(const QString &);

  void set_xserver_exit_disconnect(bool set);
  bool get_xserver_exit_disconnect();

  void setPPPDebug(bool set);
  bool getPPPDebug();

  void set_quit_on_disconnect(bool);
  bool quit_on_disconnect();

  void set_show_clock_on_caption(bool set);
  bool get_show_clock_on_caption();

  void set_show_log_window(bool set);
  bool get_show_log_window();

  void set_automatic_redial(bool set);
  bool automatic_redial();

//   void set_iconify_on_connect(bool set);
//   bool get_iconify_on_connect();

//   void set_dock_into_panel(bool set);
//   bool get_dock_into_panel();

  enum LineTermination enter();
  void setEnter(enum LineTermination);

  QString pppdVersion();
  bool pppdVersionMin(int ver, int mod, int patch);

  int pppdTimeout();
  void setpppdTimeout(int);

  int busyWait();
  void setbusyWait(int);

  bool modemLockFile();
  void setModemLockFile(bool set);

  int modemEscapeGuardTime();
  void setModemEscapeGuardTime(int i);

  void setModemEscapeStr(const QString &);
  const QString modemEscapeStr();

  void setModemEscapeResp(const QString &);
  const QString modemEscapeResp();

//  const QString modemName();
//  bool setModemName(const QString &);
//  bool changeModemName(const QString &);

   const QString modemDevice();
   bool setModemDevice(const QString &);

  enum FlowControl flowcontrol();
  void setFlowcontrol(enum FlowControl);

  int modemTimeout();
  void setModemTimeout(int);

  int modemToneDuration();
  void setModemToneDuration(int);

  QString volumeInitString();
  int volume();
  void setVolume(int);

  int waitForDialTone();
  void setWaitForDialTone(int i);

  // modem command strings/responses
  const QString modemInitStr(int i);
  void setModemInitStr(int i, const QString &);

  const QString modemInitResp();
  void setModemInitResp(const QString &);

  int modemPreInitDelay();
  void setModemPreInitDelay(int);

  int modemInitDelay();
  void setModemInitDelay(int);

  QString modemNoDialToneDetectionStr();
  void setModemNoDialToneDetectionStr(const QString &);

  const QString modemDialStr();
  void setModemDialStr(const QString &);

  const QString modemConnectResp();
  void setModemConnectResp(const QString &);

  const QString modemBusyResp();
  void setModemBusyResp(const QString &);

  const QString modemNoCarrierResp();
  void setModemNoCarrierResp(const QString &);

  const QString modemNoDialtoneResp();
  void setModemNoDialtoneResp(const QString &);

  const QString modemHangupStr();
  void setModemHangupStr(const QString &);

  const QString modemHangupResp();
  void setModemHangupResp(const QString &);

  const QString modemAnswerStr();
  void setModemAnswerStr(const QString &);

  const QString modemRingResp();
  void setModemRingResp(const QString &);

  const QString modemAnswerResp();
  void setModemAnswerResp(const QString &);

  QString volumeOff();
  void setVolumeOff(const QString &);

  QString volumeMedium();
  void setVolumeMedium(const QString &);

  QString volumeHigh();
  void setVolumeHigh(const QString &);

  // functions to set/get account information
  int count() const;
  bool setAccount(const QString &);
//  bool setAccountbyIndex(int);

  bool isUniqueAccname(const QString &);
  bool isUniqueDevname(const QString &);

  bool deleteAccount();
  bool deleteAccount(const QString &);
  int newaccount();
  int copyaccount(const QString&);

  const QString accname();
  void setAccname(const QString &);

  QStringList &phonenumbers();
  const QString phonenumber();
  void setPhonenumber(const QString &);

  const QString dialPrefix();
  void setDialPrefix(const QString &);

  int authMethod();
  void setAuthMethod(int);

  const QString storedUsername();
  void setStoredUsername(const QString &);

  const QString storedPassword();
  void setStoredPassword(const QString &);

  bool storePassword();
  void setStorePassword(bool);

  const QString speed();
  void setSpeed(const QString &);

  const QString command_before_connect();
  void setCommand_before_connect(const QString &);

  const QString command_on_connect();
  void setCommand_on_connect(const QString &);

  const QString command_on_disconnect();
  void setCommand_on_disconnect(const QString &);

  const QString command_before_disconnect();
  void setCommand_before_disconnect(const QString &);

  const QString ipaddr();
  void setIpaddr(const QString &);

  const QString subnetmask();
  void setSubnetmask(const QString &);

  bool AcctEnabled();
  void setAcctEnabled(bool set);

//   int VolAcctEnabled();
//   void setVolAcctEnabled(int set);

  bool autoDNS();
  void setAutoDNS(bool set);

  bool exDNSDisabled();
  void setExDNSDisabled(bool set);

  bool autoname();
  void setAutoname(bool set);

  const QString gateway();
  void setGateway(const QString &);

  bool defaultroute();
  void setDefaultroute(bool set);

  QStringList &dns();
  void setDns(QStringList &);

  const QString domain();
  void setDomain(const QString &);

  QStringList &scriptType();
  void setScriptType(QStringList &);

  QStringList &script();
  void setScript(QStringList &);

  QStringList &pppdArgument();
  void setpppdArgumentDefaults();
  void setpppdArgument(QStringList &);

  //functions to change/set the child pppd process info
  bool pppdRunning() const;
  void setpppdRunning(bool set);

  int pppdError() const;
  void setpppdError(int err);

  QStringList getDevicesList();

  static QString encodeWord(const QString &s);

  const QString devname();
  void setDevname(const QString &);
  bool setDevice(const QString& );
  bool deleteDevice();
  bool deleteDevice(const QString &);
  int newdevice();
  int copydevice(const QString&);
  QStringList getDevicesNamesList();

private:

    //static PPPData *_data;
//    int modemDeviceGroup;
    QString passwd;
    QString _modemName;
    int highcount;                         // index of highest account
    int highcountdev;                      // index of highest device
//    int caccount;                          // index of the current account
    QString cgroup;                        // name of current config group
    pid_t suidprocessid;                   // process ID of setuid child
    bool pppdisrunning;                    // pppd process
    // daemon
    int pppderror;                         // error encounterd running pppd
    int pppdVer, pppdMod, pppdPatch;       // pppd version


    QStringList phonelist;
    QStringList accountList;
    QStringList deviceList;
    QMap<QString,QString> stringEntries;
    QMap<QString,int> intEntries;
    QMap<QString,QStringList> listEntries;
    QMap<QString,QChar> sepEntries;

};

#endif
