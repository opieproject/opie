#include <qframe.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include "krfbserver.h"

#include "kvncconndlg.h"

KVNCConnDlg::KVNCConnDlg( KRFBServer *options,
				QWidget *parent, char *name, bool modal )
: KVNCConnDlgBase( parent, name, modal )
{
    this->options=options;
    tmpOptions=*options;

    serverHostname->setText(options->hostname);
    serverDisplay->setValue(options->display);
    serverPassword->setText(options->password);
    serverBookmark->setText(options->name);

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

KVNCConnDlg::~KVNCConnDlg()
{
}

void KVNCConnDlg::accept()
{
		save();
		QDialog::accept();
}

void KVNCConnDlg::save()
{
		tmpOptions.hexTile = hex->isChecked();
		tmpOptions.corre = corre->isChecked();
		tmpOptions.rre = rre->isChecked();
		tmpOptions.copyrect = copyRect->isChecked();
		tmpOptions.deIconify = deIconify->isChecked();
		tmpOptions.colors256 = bit->isChecked();
		tmpOptions.shared = shared->isChecked();
		tmpOptions.hostname = serverHostname->text();
		tmpOptions.password = serverPassword->text();
		tmpOptions.display = serverDisplay->value();
		tmpOptions.name = serverBookmark->text();

		if (!serverBookmark->text().isEmpty()) {
       if ( options) {
          *options=tmpOptions;
        }
    }
}
