// -*- c++ -*-

#ifndef KVNCOPTIONSDIALOG_H
#define KVNCOPTIONSDIALOG_H

#include "vncoptionsbase.h"

class KRFBOptions;

class KVNCOptionsDlg : public VncOptionsBase
{
Q_OBJECT

public:
  KVNCOptionsDlg( KRFBOptions *options,
		  QWidget *parent = 0, char *name = 0, bool modal = true );
  ~KVNCOptionsDlg();

protected:
  void accept();

private:
  KRFBOptions *options;
};

#endif // KVNCOPTIONSDIALOG_H




