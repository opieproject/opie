// -*- c++ -*-

#ifndef KVNCCONNECTDLG_H
#define KVNCCONNECTDLG_H

#include <qdialog.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qurl.h>

class KRFBConnection;

class KVNCConnectDlg : public QDialog
{
  Q_OBJECT

public: 
  KVNCConnectDlg( KRFBConnection *con,
		  QWidget *parent = 0, const char *name = 0 );

  QString hostname() { return hostNameCombo->currentText(); };
  int display() { return displayNumberEdit->value(); };
  QString password() const { return passwordEdit->text(); }
 
protected:
  void accept();

protected slots:
  void options();

private:
  QComboBox *hostNameCombo;
  QSpinBox *displayNumberEdit;
  QLineEdit *passwordEdit;
  KRFBConnection *con;
};

#endif // KVNCCONNECTDLG_H
