#ifndef INTERFACESETUPIMP_H
#define INTERFACESETUPIMP_H

#include "interfacesetup.h"
#include <qdialog.h>

class Interface;
class Interfaces;

class InterfaceSetupImp : public InterfaceSetup {
  Q_OBJECT
	
public:
  InterfaceSetupImp( QWidget* parent = 0, const char* name = 0, Interface *i=0, WFlags fl = 0);
  bool saveChanges();
  
public slots:
  void setProfile(const QString &profile);
  bool saveSettings();

private:
  Interfaces *interfaces;
  Interface *interface;
};


#include <qlayout.h>

class InterfaceSetupImpDialog : public QDialog {
Q_OBJECT
	
public:
    InterfaceSetupImpDialog(QWidget* parent = 0, const char* name = 0, Interface *i=0, bool modal = false, WFlags fl = 0) : QDialog(parent, name, modal, fl){
    QVBoxLayout *InterfaceSetupLayout = new QVBoxLayout( this );
    setCaption("Interface Setup");
    interfaceSetup = new InterfaceSetupImp(this, "InterfaceSetup",i,fl);
    InterfaceSetupLayout->addWidget( interfaceSetup );
  };
  void setProfile(QString &profile){ interfaceSetup->setProfile(profile);};

private:
  InterfaceSetupImp *interfaceSetup;

protected slots:
  void accept(){
    if(interfaceSetup->saveChanges())
      QDialog::accept();
  };

};

#endif

// interfacesetupimp.h

