#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "kpppconfig.h"

#include <qwidget.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include "chooserwidget.h"

class QCheckBox;
class QLineEdit;
class QTabWidget;
class DialWidget;
class AuthWidget;
class IPWidget;
class DNSWidget;
class GatewayWidget;
class InterfacePPP;
class PPPData;
class ModemWidget;
class ModemWidget2;

class DevicesWidget : public ChooserWidget {
  Q_OBJECT
public:
  DevicesWidget( InterfacePPP* ip, QWidget *parent=0, const char *name=0, WFlags f=0 );
  ~DevicesWidget() {}


private slots:
  virtual void edit();
  virtual void copy();
  virtual void remove();
  virtual void create();
  virtual void slotListBoxSelect(int);
  int doTab();

// signals:
//   void resetaccounts();

private:
  ModemWidget *modem1;
  ModemWidget2 *modem2;
  InterfacePPP *_ifaceppp;

};


#endif
