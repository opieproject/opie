#include <qframe.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qapplication.h>
#include "krfboptions.h"

#include "kvncoptionsdlg.h"

KVNCOptionsDlg::KVNCOptionsDlg( KRFBOptions *options,
				QWidget *parent, char *name, bool modal )
  : VncOptionsBase( parent, name, modal )
{
  this->options = options;

  hex->setChecked( options->hexTile );
  corre->setChecked( options->corre );
  rre->setChecked( options->rre );
  copyRect->setChecked( options->copyrect );

  // TODO
  hex->setEnabled( false );
  corre->setEnabled( false );
  rre->setEnabled( false );
  // /TODO

  deIconify->setChecked( options->deIconify );
  bit->setChecked( options->colors256 );
  shared->setChecked( options->shared );
  timeBox->setValue( options->updateRate );
}

KVNCOptionsDlg::~KVNCOptionsDlg()
{
}

void KVNCOptionsDlg::accept()
{
  options->hexTile = hex->isChecked();
  options->corre = corre->isChecked();
  options->rre = rre->isChecked();
  options->copyrect = copyRect->isChecked();
  options->deIconify = deIconify->isChecked();
  options->colors256 = bit->isChecked();
  options->shared = shared->isChecked();
  options->updateRate = timeBox->value();
  options->writeSettings();

  QDialog::accept();
}

