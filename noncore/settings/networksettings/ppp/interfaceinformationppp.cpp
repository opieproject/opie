#include "interfaceinformationppp.h"


#include <qpushbutton.h>
#include <qlabel.h>
//#include <qgroupbox.h>
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
    :InterfaceInformationImp(parent, name, i, f)
{
    qDebug("InterfaceInformationPPP::InterfaceInformationPPP %s", name);
    con = new ConnectWidget( (InterfacePPP*)i, this, "con" );
    con->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding,
                                    QSizePolicy::Fixed) );
//      QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//      InterfaceInformationLayout->addItem( spacer, 7, 0 );
//    InterfaceInformationLayout->addMultiCellWidget( con, 8, 8, 0, 1 );
     InterfaceInformationLayout->addWidget( con, 1, 0 );
     connect(i, SIGNAL(begin_connect()),con, SLOT(preinit()));
}



