
#include <stdio.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtimer.h>

#include <qpe/config.h>

#include "pindlg.h"

using namespace OpieTooth;

PinDlg::PinDlg( const QString& status,
                const QString& mac, QWidget* parent,
                const char* name )
    : PinDlgBase( parent, name, WType_Modal )
{
    m_mac = mac;
    test( mac );
    txtStatus->setText(status);
}
PinDlg::~PinDlg() {

}
void PinDlg::setMac( const QString& mac ) {
    txtStatus->setText( mac );
}
QString PinDlg::pin() const{
    return lnePin->text();
}

void PinDlg::test( const QString& mac ) {
    if (!mac.isEmpty() ) {
        Config cfg("bluepin");
        cfg.setGroup(mac);
        lnePin->setText(cfg.readEntryCrypt("pin", QString::null ) );
        if ( !lnePin->text().isEmpty() ) {
            //QTimer::singleShot(100,  this,  SLOT(accept() ) );
        }

    }

}
void PinDlg::accept() {
    if ( ckbPin->isChecked() ) {
        Config cfg("bluepin");
        cfg.setGroup(m_mac );
        cfg.writeEntryCrypt("pin", lnePin->text() );
    }
    QDialog::accept();
}
