#ifndef _PPPCONFIG_H_
#define _PPPCONFIG_H_

#include <qdialog.h>

class QTabWidget;
class AccountWidget;
class GeneralWidget;
class ModemWidget;
class ModemWidget2;

class PPPConfigWidget : public QDialog {
    Q_OBJECT
public:

    PPPConfigWidget( QWidget *parent=0, const char *name=0, 
		     bool modal = false, WFlags fl = 0 );
    ~PPPConfigWidget();	


protected slots:
    virtual void accept();
    virtual void reject();

 private:
  QTabWidget *tabWindow;
  AccountWidget *accounts;
  GeneralWidget *general;
  ModemWidget *modem1;
  ModemWidget2 *modem2;
};


#endif
