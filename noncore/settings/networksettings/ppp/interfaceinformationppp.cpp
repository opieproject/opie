#include "interfaceinformationppp.h"

#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qabstractlayout.h>

#include "connect.h"
#include "conwindow.h"

#ifdef QWS
#else
 #define showMaximized show
#endif

/**
 * Constructor for the InterfaceInformationImp class.  This class pretty much
 * just display's information about the interface that is passed to it.
 */
InterfaceInformationPPP::InterfaceInformationPPP(QWidget *parent, const char *name, Interface *i, WFlags f)
    :InterfaceInformationImp(parent, name, i, Qt::WStyle_ContextHelp)
{
    qDebug("InterfaceInformationPPP::InterfaceInformationPPP %s", name);
    con = new ConnectWidget( (InterfacePPP*)i, this, "con" );
    con->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding,
                                    QSizePolicy::Fixed) );

    macAddressLabel->hide();
    subnetMaskLabel->hide();
    broadcastLabel->hide();
    TextLabel23->hide();
    TextLabel21->hide();
    TextLabel24->hide();

     InterfaceInformationLayout->addWidget( con, 1, 0 );
     connect(i, SIGNAL(begin_connect()),con, SLOT(preinit()));
     connect(i, SIGNAL(hangup_now() ), con, SLOT(cancelbutton() ) );
}



