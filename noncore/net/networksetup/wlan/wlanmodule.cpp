#include "wlanmodule.h"
#include "wlanimp.h"
#include "info.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

#include <qlabel.h>

/**
 * Constructor, find all of the possible interfaces
 */ 
WLANModule::WLANModule() : Module() {
  // get output from iwconfig
}

/**
 * Change the current profile
 */ 
void WLANModule::setProfile(QString newProfile){
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
  if(i->getInterfaceName() == "eth0" || i->getInterfaceName() == "wlan0"){
    i->setHardwareName("802.11b");
    return true;
  }
  return false;
}

/**
 * Create, set tabWiget and return the WLANConfigure Module
 * @param tabWidget a pointer to the tab widget that this configure has.
 * @return QWidget* pointer to the tab widget in this modules configure.
 */ 
QWidget *WLANModule::configure(Interface *i, QTabWidget **tabWidget){
  WLANImp *wlanconfig = new WLANImp(0, "WlanConfig");
  (*tabWidget) = wlanconfig->tabWidget;
  return wlanconfig;
}

/**
 * Create, set tabWiget and return the Information Module
 * @param tabWidget a pointer to the tab widget that this information has.
 * @return QWidget* pointer to the tab widget in this modules info.
 */ 
QWidget *WLANModule::information(Interface *i, QTabWidget **tabWidget){
  return NULL;
  WlanInfo *info = new WlanInfo(0, "wireless info");
  (*tabWidget) = info->tabWidget;
 
  struct ifreq ifr;
  struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;
  int fd = socket( AF_INET, SOCK_DGRAM, 0 );
  
  const char* buffer[200];
  struct iwreq iwr;
  memset( &iwr, 0, sizeof( iwr ) );
  iwr.u.essid.pointer = (caddr_t) buffer;
  iwr.u.essid.length = IW_ESSID_MAX_SIZE;
  iwr.u.essid.flags = 0;
  
  // check if it is an IEEE 802.11 standard conform
  // wireless device by sending SIOCGIWESSID
  // which also gives back the Extended Service Set ID
  // (see IEEE 802.11 for more information)

  QString n = (i->getInterfaceName());
  const char* iname = n.latin1();
  strcpy( iwr.ifr_ifrn.ifrn_name,  (const char *)iname );
  int result = ioctl( fd, SIOCGIWESSID, &iwr );
  if ( result == 0 ){
    //hasWirelessExtensions = true;
    iwr.u.essid.pointer[(unsigned int) iwr.u.essid.length-1] = '\0';
    info->essidLabel->setText(QString(iwr.u.essid.pointer));
  }
  else
    return info;
  //info->essidLabel->setText("*** Unknown ***");

  // Address of associated access-point
  result = ioctl( fd, SIOCGIWAP, &iwr );
  if ( result == 0 ){
    QString foo = foo.sprintf( "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
      iwr.u.ap_addr.sa_data[0]&0xff,
      iwr.u.ap_addr.sa_data[1]&0xff,
      iwr.u.ap_addr.sa_data[2]&0xff,
      iwr.u.ap_addr.sa_data[3]&0xff,
      iwr.u.ap_addr.sa_data[4]&0xff,
      iwr.u.ap_addr.sa_data[5]&0xff );
    info->apLabel->setText(foo);
  }
  else info->apLabel->setText("*** Unknown ***");

  iwr.u.data.pointer = (caddr_t) buffer;
  iwr.u.data.length = IW_ESSID_MAX_SIZE;
  iwr.u.data.flags = 0;
  result = ioctl( fd, SIOCGIWNICKN, &iwr );
  if ( result == 0 ){
    iwr.u.data.pointer[(unsigned int) iwr.u.data.length-1] = '\0';
    info->stationLabel->setText(iwr.u.data.pointer);
  }
  else info->stationLabel->setText("*** Unknown ***");

  result = ioctl( fd, SIOCGIWMODE, &iwr );
  if ( result == 0 )
    info->modeLabel->setText( QString("%1").arg(iwr.u.mode == IW_MODE_ADHOC ? "Ad-Hoc" : "Managed"));
  else
    info->modeLabel->setText("*** Unknown ***");

  result = ioctl( fd, SIOCGIWFREQ, &iwr );
  if ( result == 0 )
    info->freqLabel->setText(QString("%1").arg((double( iwr.u.freq.m ) * pow( 10, iwr.u.freq.e ) / 1000000000)));
  else
    info->freqLabel->setText("*** Unknown ***");

  /*
  // gather link quality from /proc/net/wireless

    char c;
    QString status;
    QString name;
    QFile wfile( PROCNETWIRELESS );
    bool hasFile = wfile.open( IO_ReadOnly );
    QTextStream wstream( &wfile );
    if ( hasFile )
    {
        wstream.readLine();  // skip the first two lines
        wstream.readLine();  // because they only contain headers
    }
    if ( ( !hasFile ) || ( wstream.atEnd() ) )
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: D'oh! Someone removed the card..." );
#endif
        quality = -1;
        signal = IW_LOWER;
        noise = IW_LOWER;
        return false;
    }

    wstream >> name >> status >> quality >> c >> signal >> c >> noise;

    if ( quality > 92 )
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: D'oh! Quality %d > estimated max!\n", quality );
#endif
    if ( ( signal > IW_UPPER ) || ( signal < IW_LOWER ) )
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: Doh! Strength %d > estimated max!\n", signal );
#endif
    if ( ( noise > IW_UPPER ) || ( noise < IW_LOWER ) )
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: Doh! Noise %d > estimated max!\n", noise );
#endif

    return true;

} 
*/
  return info;
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
Interface *WLANModule::addNewInterface(QString ){
  // We can't add a 802.11 interface, either the hardware will be there
  // or it wont. 
  return NULL; 
}

/**
 * Attempts to remove the interface, doesn't delete i
 * @return bool true if successfull, false otherwise.
 */ 
bool WLANModule::remove(Interface*){
  // Can't remove a hardware device, you can stop it though.
  return false; 
}

// wlanmodule.cpp

