
#ifndef DEVICE_H
#define DEVICE_H
#include <qobject.h>
#include <sys/soundcard.h>

class Device : public QObject {
   Q_OBJECT
public:
   Device( QObject * parent=0, const char * dspStr=0, const char * mixerStr=0, bool record=0 );
   ~Device() {};
   bool closeDevice( bool);
   int getChannels();
   int getFormat();
   int getInVolume();
   int getOutVolume();
   int getRate();
   int getRes();
   int sd; //sound descriptor
   void changedInVolume(int);
   void changedOutVolume(int);
   bool openDsp();
   int getDeviceFormat();
   int getDeviceRate();
   int getDeviceBits();
   int getDeviceChannels();
   int getDeviceFragSize();
   bool setFragSize(int);
   bool setDeviceChannels(int);
   bool setDeviceRate(int);
   bool setDeviceFormat(int);
   bool reset();

   int devRead(int, short *, int);
   int devWrite(int, short *, int);   

private:
   int devRes, devCh, devRate, devForm, flags;
   char *dspstr, *mixstr;
   bool selectMicInput();
   int openDevice( int );
private slots:

protected:
  
};

#endif
