#include "interfaceinformationppp.h"


#include <qpushbutton.h>
#include <qlabel.h>
//#include <qgroupbox.h>
#include <qmessagebox.h>

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
}



