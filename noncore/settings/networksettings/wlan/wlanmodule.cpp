
#include "wlanmodule.h"
#include "wlanimp2.h"
#include "infoimp.h"
#include "wextensions.h"
#include "interfaceinformationimp.h"

/* OPIE */
#include <qpe/qpeapplication.h>

/* QT */
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qspinbox.h>
#include <qtabwidget.h>


/**
 * Constructor, find all of the possible interfaces
 */
WLANModule::WLANModule()
    : Module(),
      wlanconfigWiget(0)
{
}

/**
 * Delete any interfaces that we own.
 */
WLANModule::~WLANModule(){
  Interface *i;
  for ( i=list.first(); i != 0; i=list.next() )
    delete i;

}

/**
 * Change the current profile
 */
void WLANModule::setProfile(const QString &newProfile){
  profile = newProfile;
}

/**
 * get the icon name for this device.
 * @param Interface* can be used in determining the icon.
 * @return QString the icon name (minus .png, .gif etc)
 */
QString WLANModule::getPixmapName(Interface* ){
  return "wlan";
}

/**
 * Check to see if the interface i is owned by this module.
 * @param Interface* interface to check against
 * @return bool true if i is owned by this module, false otherwise.
 */
bool WLANModule::isOwner(Interface *i){
  WExtensions we(i->getInterfaceName());
  if(!we.doesHaveWirelessExtensions())
    return false;

  i->setHardwareName("802.11b");
  list.append(i);
  return true;
}

/**
 * Create, and return the WLANConfigure Module
 * @return QWidget* pointer to this modules configure.
 */
QWidget *WLANModule::configure(Interface *i){
  WLANImp *wlanconfig = new WLANImp(0, "WlanConfig", i, true,  Qt::WDestructiveClose);
  wlanconfig->setProfile(profile);
  return wlanconfig;
}

/**
 * Create, and return the Information Module
 * @return QWidget* pointer to this modules info.
 */
QWidget *WLANModule::information(Interface *i){
  WExtensions we(i->getInterfaceName());
  if(!we.doesHaveWirelessExtensions())
    return NULL;

  return getInfo( i );
}

/**
 * Get all active (up or down) interfaces
 * @return QList<Interface> A list of interfaces that exsist that havn't
 * been called by isOwner()
 */
QList<Interface> WLANModule::getInterfaces(){
  return list;
}

/**
 * Attempt to add a new interface as defined by name
 * @param name the name of the type of interface that should be created given
 *  by possibleNewInterfaces();
 * @return Interface* NULL if it was unable to be created.
 */
Interface *WLANModule::addNewInterface(const QString &){
  // We can't add a 802.11 interface, either the hardware will be there
  // or it wont.
  return NULL;
}

/**
 * Attempts to remove the interface, doesn't delete i
 * @return bool true if successful, false otherwise.
 */
bool WLANModule::remove(Interface*){
  // Can't remove a hardware device, you can stop it though.
  return false;
}

void WLANModule::receive(const QCString &param, const QByteArray &arg)
{
    odebug << "WLANModule::receive "+param << oendl; 
    QStringList params = QStringList::split(",",param);
    int count = params.count();
    odebug << "WLANModule got " << count << " params" << oendl; 
    if (count < 2){
        odebug << "Erorr less than 2 parameter" << oendl; 
        odebug << "RETURNING" << oendl; 
        return;
    }

    QDataStream stream(arg,IO_ReadOnly);
    QString interface;
    QString action;
    int countMsgs = 0;
    stream >> interface;
    odebug << "got count? >" << interface.latin1() << "<" << oendl; 
    if (interface == "count"){
        odebug << "got count" << oendl; 
        stream >> action;
        odebug << "Got count num >" << action.latin1() << "<" << oendl; 
        countMsgs = action.toInt();
    }

    QDialog *toShow;
    //while (! stream.atEnd() ){
    for (int i = 0; i < countMsgs; i++){
        odebug << "start stream " << i << "/" << countMsgs << "" << oendl; 
        if (stream.atEnd()){
            odebug << "end of stream" << oendl; 
            return;
        }
        stream >> interface;
        odebug << "got iface" << oendl; 
        stream >> action;
        odebug << "WLANModule got interface " << interface.latin1() << " and acion " << action.latin1() << "" << oendl; 
        // find interfaces
        Interface *ifa=0;
        for ( Interface *i=list.first(); i != 0; i=list.next() ){
            if (i->getInterfaceName() == interface){
                odebug << "WLANModule found interface " << interface.latin1() << "" << oendl; 
                ifa = i;
            }
        }

        if (ifa == 0){
            odebug << "WLANModule Did not find " << interface.latin1() << "" << oendl; 
            odebug << "skipping" << oendl; 
            count = 0;
        }

        if (count == 2){
            // those should call the interface directly
            QWidget *info = getInfo( ifa );
            QPEApplication::showWidget( info );

            if ( action.contains("start" ) ){
                ifa->start();
            } else if ( action.contains("restart" ) ){
                ifa->restart();
            } else if ( action.contains("stop" ) ){
                ifa->stop();
            }else if ( action.contains("refresh" ) ){
                ifa->refresh();
            }
        }else if (count == 3){
            QString value;
            if (!wlanconfigWiget){
                //FIXME: what if it got closed meanwhile?
                wlanconfigWiget = (WLANImp*) configure(ifa);
                toShow = (QDialog*) wlanconfigWiget;
            }
            QPEApplication::showWidget( wlanconfigWiget );
            stream >> value;
            odebug << "WLANModule (build 4) is setting " << action.latin1() << " of " << interface.latin1() << " to " << value.latin1() << "" << oendl; 
            if (value.isEmpty()){
                odebug << "value is empty!!!\nreturning" << oendl; 
                return;
            }
            if ( action.contains("ESSID") ){
                QComboBox *combo = wlanconfigWiget->essid;
                bool found = false;
                for ( int i = 0; i < combo->count(); i++)
                    if ( combo->text( i ) == value ){
                        combo->setCurrentItem( i );
                        found = true;
                }
                if (!found) combo->insertItem( value, 0 );
            }else if ( action.contains("Mode") ){
                QComboBox *combo = wlanconfigWiget->mode;
                for ( int i = 0; i < combo->count(); i++)
                    if ( combo->text( i ) == value ){
                    combo->setCurrentItem( i );
                    }

            }else if (action.contains("Channel")){
                bool ok;
                odebug << "converting channel" << oendl; 
                int chan = value.toInt( &ok );
                if (ok){
                    odebug << "ok setting channel" << oendl; 
                    wlanconfigWiget->specifyChan->setChecked( true );
                    wlanconfigWiget->networkChannel->setValue( chan );
                }
            }else if (action.contains("MacAddr")){
                wlanconfigWiget->specifyAp->setChecked( true );
                wlanconfigWiget->macEdit->setText( value );
            }else
                odebug << "wlan plugin has no clue" << oendl; 
        }
        odebug << "next stream" << oendl; 
    }// while stream
    odebug << "end of stream" << oendl; 
    if (toShow) toShow->exec();
    odebug << "returning" << oendl; 
}

QWidget *WLANModule::getInfo( Interface *i)
{
    odebug << "WLANModule::getInfo start" << oendl; 
    WlanInfoImp *info = new WlanInfoImp(0, i->getInterfaceName(), Qt::WDestructiveClose);
    InterfaceInformationImp *information = new InterfaceInformationImp(info->tabWidget, "InterfaceSetupImp", i);
    info->tabWidget->insertTab(information, "TCP/IP", 0);
    info->tabWidget->setCurrentPage( 0 );
    info->tabWidget->showPage( information );
    if (info->tabWidget->currentPage() == information ) odebug << "infotab OK" << oendl; 
    else odebug << "infotab NOT OK" << oendl; 
    odebug << "current idx " << info->tabWidget->currentPageIndex() << "" << oendl; 
    odebug << "WLANModule::getInfo return" << oendl; 
    return info;
}

