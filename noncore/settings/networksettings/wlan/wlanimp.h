#ifndef WLANIMP_H
#define WLANIMP_H

#include "wlan.h"
#include <qstringlist.h>

class InterfaceSetupImp;
class Interface;
class Config;

class WLANImp : public WLAN { 
  Q_OBJECT

public:
  WLANImp( QWidget* parent = 0, const char* name = 0, Interface *i=0, bool modal = FALSE, WFlags fl = 0 );
  void setProfile(const QString &profile);

protected:
  void accept();

private slots:
  void typeChanged(int);
  
private:
  void parseSettingFile();
  void changeAndSaveSettingFile();
  
  InterfaceSetupImp *interfaceSetup;
  QStringList settingsFileText;
  QString currentProfile;
};

#endif
 
