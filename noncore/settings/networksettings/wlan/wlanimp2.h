#ifndef WLANIMP_H
#define WLANIMP_H

#include "wlan.h"
#include "interfaces.h"
#include <qstringlist.h>
#include <opie2/onetutils.h>

class InterfaceSetupImp;
class Interface;
class Config;
class OPacket;
class QListViewItem;

const int col_mode= 0;
const int col_ssid = 1;
const int col_chn = 2;
const int col_mac = 3;

class WLANImp : public WLAN {
  Q_OBJECT

public:
  WLANImp( QWidget* parent = 0, const char* name = 0, Interface *i=0, bool modal = FALSE, WFlags fl = 0 );
  ~WLANImp();
  void setProfile(const QString &profile);

public slots:
  void rescanNeighbourhood();
  void selectNetwork( QListViewItem* );

protected:
  void accept();

private:
  void parseOpts();
  void writeOpts();

  void parseKeyStr(QString keystr);

  void handlePacket( OPacket* );
  void displayFoundNetwork( const QString& mode, int channel, const QString& ssid, const OMacAddress& mac );

  InterfaceSetupImp *interfaceSetup;
  Interfaces *interfaces;
  Interface  *interface;

  QString currentProfile;
};

#endif

