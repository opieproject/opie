
#include "pindlg.h"

/* OPIE */
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>

/* STD */
#include <stdio.h>

using namespace OpieTooth;

PinDlg::PinDlg( const QString& status,
                const QString& mac, QWidget* parent,
                const char* name )
        : PinDlgBase( parent, name, WType_Modal )
{
    m_mac = mac;
    test( mac );
    txtStatus->setText(status);
    if(!m_mac.isEmpty())
	    QPEApplication::showDialog( this );
}

PinDlg::PinDlg(QWidget* parent,
               const char* name, 
               Qt::WFlags f )
        : PinDlgBase( parent, name, f )
{
    m_mac = makeMacFromArgs();

    if(m_mac.isEmpty())
    {
        // can't obtain MAC
        printf("ERR\n"); 
	qApp->quit();
    }
    else
    {
        test( m_mac );
        txtStatus->setText(makeTextFromArgs());
        QPEApplication::showDialog( this ) ; 
    }
}

PinDlg::~PinDlg()
{}

void PinDlg::setMac( const QString& mac )
{
    txtStatus->setText( mac );
}

QString PinDlg::pin() const
{
    return lnePin->text();
}

void PinDlg::test( const QString& mac )
{
    if (!mac.isEmpty() )
    {
        Config cfg("bluepin");
        cfg.setGroup(mac);
        lnePin->setText(cfg.readEntryCrypt("pin", QString::null ) );
        if ( !lnePin->text().isEmpty() )
        {
            //QTimer::singleShot(100,  this,  SLOT(accept() ) );
        }
    }

}

QString PinDlg::makeTextFromArgs()
{
    if(qApp->argc() > 2)
    {
        QCString dir(qApp->argv()[1]) ;
        QCString bdaddr(qApp->argv()[2]) ;
        
        QCString name;
        if ( qApp->argc() > 3 ) {
            name = qApp->argv()[3];
        }
        QString status;
        if (dir == "out" ) {
            status = QObject::tr("Outgoing connection to ");
        } else
            status = QObject::tr("Incoming connection from ");

        status += name;
        status += "<br>";
        status += "[" + bdaddr + "]";
        
        return status ;
    }
    else
        return QString();
}

QString PinDlg::makeMacFromArgs()
{
    if(qApp->argc() < 3)
        return QString();
    else
        return qApp->argv()[2] ;
}

void PinDlg::accept()
{
    if ( ckbPin->isChecked() )
    {
        Config cfg("bluepin");
        cfg.setGroup(m_mac );
        cfg.writeEntryCrypt("pin", lnePin->text() );
    }
    printf("PIN:%s\n", lnePin->text().latin1());
    QDialog::accept();
    qApp->quit();
}
