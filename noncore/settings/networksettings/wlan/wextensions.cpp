#include "wextensions.h"

#include <qfile.h>
#include <qtextstream.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <math.h>

#define PROCNETWIRELESS "/proc/net/wireless"
#define IW_LOWER 0
#define IW_UPPER 256

/**
 * Constructor.  Sets hasWirelessExtensions
 */ 
WExtensions::WExtensions(QString interfaceName){
  interface = interfaceName;
  fd = socket( AF_INET, SOCK_DGRAM, 0 );

  const char* buffer[200];
  memset( &iwr, 0, sizeof( iwr ) );
  iwr.u.essid.pointer = (caddr_t) buffer;
  iwr.u.essid.length = IW_ESSID_MAX_SIZE;
  iwr.u.essid.flags = 0;
  
  // check if it is an IEEE 802.11 standard conform
  // wireless device by sending SIOCGIWESSID
  // which also gives back the Extended Service Set ID
  // (see IEEE 802.11 for more information)

  const char* iname = interface.latin1();
  strcpy( iwr.ifr_ifrn.ifrn_name,  (const char *)iname );
  if ( 0 == ioctl( fd, SIOCGIWESSID, &iwr ) )
    hasWirelessExtensions = true;
  else
    hasWirelessExtensions = false;
}

/**
 * @return QString the station name of the access point.
 */ 
QString WExtensions::station(){
  if(!hasWirelessExtensions)
    return QString();
  const char* buffer[200];
  iwr.u.data.pointer = (caddr_t) buffer;
  iwr.u.data.length = IW_ESSID_MAX_SIZE;
  iwr.u.data.flags = 0;
  if ( 0 == ioctl( fd, SIOCGIWNICKN, &iwr )){
    iwr.u.data.pointer[(unsigned int) iwr.u.data.length-1] = '\0';
    return QString(iwr.u.data.pointer);
  }
  return QString();
}

/**
 * @return QString the essid of the host 802.11 access point.
 */ 
QString WExtensions::essid(){
  if(!hasWirelessExtensions)
    return QString();
  if ( 0 == ioctl( fd, SIOCGIWESSID, &iwr )){
    iwr.u.essid.pointer[(unsigned int) iwr.u.essid.length-1] = '\0';
    return QString(iwr.u.essid.pointer);
  }
  return QString();
}

/**
 * @return QString the mode of interface
 */ 
QString WExtensions::mode(){
  if(!hasWirelessExtensions)
    return QString();
  if ( 0 == ioctl( fd, SIOCGIWMODE, &iwr ) )
    return QString("%1").arg(iwr.u.mode == IW_MODE_ADHOC ? "Ad-Hoc" : "Managed");
  return QString();
}

/**
 * Get the frequency that the interface is running at.
 * @return int the frequency that the interfacae is running at.
 */ 
double WExtensions::frequency(){
  if(!hasWirelessExtensions)
    return 0;
  if ( 0 == ioctl( fd, SIOCGIWFREQ, &iwr ))
    return (double( iwr.u.freq.m ) * pow( 10, iwr.u.freq.e ) / 1000000000);
  return 0;
}

/***
 * Get the current rate that the card is transmiting at.
 */ 
double WExtensions::rate(){
  if(!hasWirelessExtensions)
    return 0;
  if(0 == ioctl(fd, SIOCGIWRATE, &iwr)){
    return ((double)iwr.u.bitrate.value)/1000000;
  }
  return 0;
}


/**
 * @return QString the AccessPoint that the interface is connected to.
 */ 
QString WExtensions::ap(){
  if(!hasWirelessExtensions)
    return QString();
  if ( 0 == ioctl( fd, SIOCGIWAP, &iwr )){
    QString ap;
    ap = ap.sprintf( "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
      iwr.u.ap_addr.sa_data[0]&0xff,
      iwr.u.ap_addr.sa_data[1]&0xff,
      iwr.u.ap_addr.sa_data[2]&0xff,
      iwr.u.ap_addr.sa_data[3]&0xff,
      iwr.u.ap_addr.sa_data[4]&0xff,
      iwr.u.ap_addr.sa_data[5]&0xff );
    return ap;
  }
  else return QString();
}

/**
 * Get the stats for interfaces
 * @param signal the signal strength of interface
 * @param noise the noise level of the interface
 * @param quality the quality level of the interface
 * @return bool true if successfull
 */ 
bool WExtensions::stats(int &signal, int &noise, int &quality){
  // gather link quality from /proc/net/wireless
  if(!QFile::exists(PROCNETWIRELESS))
    return false;

  char c;
  QString status;
  QString name;
  
  QFile wfile( PROCNETWIRELESS );
  if(!wfile.open( IO_ReadOnly ))
    return false;
  
  QTextStream wstream( &wfile );
  wstream.readLine();  // skip the first two lines
  wstream.readLine();  // because they only contain headers
  while(!wstream.atEnd()){
    wstream >> name >> status >> quality >> c >> signal >> c >> noise;
    if(name == QString("%1:").arg(interface)){
      if ( quality > 92 )
        qDebug( "WIFIAPPLET: D'oh! Quality %d > estimated max!\n", quality );
      if ( ( signal > IW_UPPER ) || ( signal < IW_LOWER ) )
        qDebug( "WIFIAPPLET: Doh! Strength %d > estimated max!\n", signal );
      if ( ( noise > IW_UPPER ) || ( noise < IW_LOWER ) )
        qDebug( "WIFIAPPLET: Doh! Noise %d > estimated max!\n", noise );
      //qDebug(QString("q:%1, s:%2, n:%3").arg(quality).arg(signal).arg(noise).latin1());
      signal = ( ( signal-IW_LOWER ) * 100 ) / IW_UPPER;
      noise = ( ( noise-IW_LOWER ) * 100 ) / IW_UPPER;
      quality = ( quality*100 ) / 92;
      return true;
    }  
  }

  qDebug("WExtensions::statsCard no longer present.");
  quality = -1;
  signal = IW_LOWER;
  noise = IW_LOWER;
  return false;
}

// wextensions.cpp
