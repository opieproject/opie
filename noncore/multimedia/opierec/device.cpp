// device.cpp

#include "device.h"

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>
#include<sys/wait.h>
// #include <sys/stat.h>
// #include <sys/time.h>
// #include <sys/types.h>
#include <unistd.h>
#include <errno.h>

//extern QtRec *qperec;

Device::Device( QObject * parent, const char * dsp, const char * mixr, bool record )
        : QObject( parent)
{
    dspstr = (char *)dsp;
    mixstr = (char *)mixr;
 
    devForm=-1;
    devCh=-1;
    devRate=-1;

    if( !record){ //playing
//        qDebug("setting up DSP for playing");
        flags = O_WRONLY;
    } else { //recording
//        qDebug("setting up DSP for recording");
        flags = O_RDWR;
//        flags = O_RDONLY;

//        selectMicInput();

    }
}

bool Device::openDsp() {
    if( openDevice( flags) == -1) {
        perror("<<<<<<<<<<<<<<ioctl(\"Open device\")");
        return false;
    }
    return true;
}

int Device::getOutVolume( ) {
    unsigned int volume;
    int mixerHandle = open( mixstr, O_RDWR );
    if ( mixerHandle >= 0 ) {
        if(ioctl( mixerHandle, MIXER_READ(SOUND_MIXER_VOLUME), &volume )==-1)
            perror("<<<<<<<<<<<<<<ioctl(\"MIXER_READ\")");
        ::close( mixerHandle );
    } else
        perror("open(\"/dev/mixer\")");
//    printf("<<<<<<<<<<<<<<<<<<<<output volume %d\n",volume);

    Config cfg("qpe");
    cfg.setGroup("Volume");

    return cfg.readNumEntry("VolumePercent");
}

int Device::getInVolume() {
    unsigned int volume=0;
    int mixerHandle = ::open( mixstr, O_RDWR );
    if ( mixerHandle >= 0 ) {
        if(ioctl( mixerHandle, MIXER_READ(SOUND_MIXER_MIC), &volume )==-1)
            perror("<<<<<<<<<<<<<<<ioctl(\"MIXER_READ\")");
        ::close( mixerHandle );
    } else
        perror("open(\"/dev/mixer\")");
//    printf("<<<<<<<<<<<<<<input volume %d\n", volume );
    Config cfg("qpe");
    cfg.setGroup("Volume");

    return cfg.readNumEntry("Mic");
}

void Device::changedOutVolume(int vol) {
    int level = (vol << 8) + vol;
    int fd = 0;
    if ((fd = open("/dev/mixer", O_RDWR))>=0) {
        if(ioctl(fd, MIXER_WRITE(SOUND_MIXER_VOLUME), &level) == -1)
            perror("ioctl(\"MIXER_IN_WRITE\")");

        Config cfg("qpe");
        cfg.setGroup("Volume");
        cfg.writeEntry("VolumePercent", QString::number( vol ));
        QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << false;
	qWarning("changing output vol %d", vol);
    }
    ::close(fd);
}

void Device::changedInVolume(int vol ) {
    int level = (vol << 8) + vol;
    int fd = 0;
    if ((fd = open("/dev/mixer", O_RDWR))>=0) {
        if(ioctl(fd, MIXER_WRITE(SOUND_MIXER_MIC), &level) == -1)
            perror("ioctl(\"MIXER_IN_WRITE\")");
        Config cfg("qpe");
        cfg.setGroup("Volume");
        cfg.writeEntry("Mic", QString::number(vol ));
        QCopEnvelope( "QPE/System", "micChange(bool)" ) << false;
	qWarning("changing input volume %d", vol);
    }
    ::close(fd);
}

bool Device::selectMicInput() {

  int md=0;
  int info=MIXER_WRITE(SOUND_MIXER_MIC);
  md = ::open(  "/dev/mixer", O_RDWR );
  if ( md == -1)
  perror("open(\"/dev/mixer\")");
  else {
  if( ioctl( md, SOUND_MIXER_WRITE_RECSRC, &info) == -1)
  perror("ioctl(\"SOUND_MIXER_WRITE_RECSRC\")");
  ::close(md);
  return false;
  }
  ::close(md);

    return true;
}

int Device::openDevice( int flags) {
/*     pid_t pid;
     int status;
     int pipefd[2];
     char message[20];
     if (pipe(pipefd) == -1){
         perror ("Error creating pipe");
exit(1);
     }
     switch (pid = fork()) {
       case -1:
           perror("The fork failed!");
           break;
       case 0: {
 */
    if (( sd = ::open( dspstr, flags)) == -1) {
              perror("open(\"/dev/dsp\")");
              QString errorMsg="Could not open audio device\n /dev/dsp\n"
                  +(QString)strerror(errno);
              qDebug("XXXXXXXXXXXXXXXXXXXXXXX  "+errorMsg);
              return -1;
          }

    int mixerHandle=0;
        if  (( mixerHandle = open("/dev/mixer",O_RDWR))<0) {
            perror("open(\"/dev/mixer\")");
              QString errorMsg="Could not open audio device\n /dev/dsp\n"
                  +(QString)strerror(errno);
              qDebug("XXXXXXXXXXXXXXXXXXXXXX  "+errorMsg);
        }

        if(ioctl(sd,SNDCTL_DSP_RESET,0)<0){
            perror("ioctl RESET");
        }
//           sprintf(message, "%d", sd);

/*           QFile f1("/pid");
           f1.open(IO_WriteOnly );
           f1.writeBlock(message, strlen(message));
           f1.close();
 */
  /*               close(pipefd[0]);
                 write(pipefd[1], message, sizeof(message));
     close(pipefd[1]);
 //              qDebug("%d",soundDevice->sd );
           _exit(0);
       }
       default:
          // pid greater than zero is parent getting the child's pid
           printf("Child's pid is %d\n",pid);
           QString s;
                close(pipefd[1]);
                read(pipefd[0], message, sizeof(message));
    s = message;
                 close(pipefd[0]);

//     while(wait(NULL)!=pid)
  //             ;
           printf("child %ld terminated normally, return status is zero\n", (long) pid);
             */
       //    filePara.sd=(long) pid;
   /*        QFile f2("/pid");
           f2.open(IO_ReadOnly);
           QTextStream t(&f2);
 //                for(int f=0; f < t.atEnd() ;f++) {
           s = t.readLine();
 //               }
           */
//     bool ok;
//           sd = s.toInt(&ok, 10);
//           qDebug("<<<<<<<<<<<<<>>>>>>>>>>>>"+s);
                 
//           f2.close();
//     }
::close(mixerHandle );
//         qDebug("open device %s", dspstr);
//     qDebug("success! %d",sd);
    return sd;
}

bool Device::closeDevice( bool) {
//      if(b) {//close now
//          if (ioctl( sd, SNDCTL_DSP_RESET, 0) == -1) {
//              perror("ioctl(\"SNDCTL_DSP_RESET\")");
//          }
//      } else { //let play
//          if (ioctl( sd, SNDCTL_DSP_SYNC, 0) == -1) {
//              perror("ioctl(\"SNDCTL_DSP_SYNC\")");
//          }
//      }

    ::close( sd); //close sound device
//    sdfd=0;
 //   sd=0;
//    qDebug("closed dsp");
    return true;
}

bool Device::setDeviceFormat( int form) {
//    qDebug("set device res %d %d", form, sd);
    if (ioctl( sd, SNDCTL_DSP_SETFMT,  &form)==-1) { //set format
        perror("ioctl(\"SNDCTL_DSP_SETFMT\")");
        return false;
    }
    devRes=form;
    return true;
}

bool Device::setDeviceChannels( int ch) {
//    qDebug("set channels %d %d", ch, sd);
    if (ioctl( sd, SNDCTL_DSP_CHANNELS, &ch)==-1) {
        perror("ioctl(\"SNDCTL_DSP_CHANNELS\")");
        return false;
    }
    devCh=ch;
    return true;
}

bool Device::setDeviceRate( int rate) {
//    qDebug("set rate %d %d", rate, sd);
    if (ioctl( sd, SNDCTL_DSP_SPEED, &rate) == -1) {
        perror("ioctl(\"SNDCTL_DSP_SPEED\")");
        return false;
    }

    devRate=rate;

    return true;
}

int Device::getRes() {
    return devRes;
}

int Device::getFormat() {
    return devForm;
}

int Device::getRate() {
    return devRate;
}

int Device::getChannels() {
    return devCh;
}

int Device::getDeviceFormat() {
    return 0;
}


int Device::getDeviceRate() {
    int dRate=0;
    if (ioctl( sd, SOUND_PCM_READ_RATE, &dRate) == -1) {
        perror("ioctl(\"SNDCTL_PCM_READ_RATE\")");
    }
    return dRate;

}

int Device::getDeviceBits() {
    int dBits=0;
#ifndef QT_QWS_EBX // zaurus doesnt have this
     if (ioctl( sd, SOUND_PCM_READ_BITS, &dBits) == -1) {
         perror("ioctl(\"SNDCTL_PCM_READ_BITS\")");
     }
#endif
    return dBits;
}

int Device::getDeviceChannels() {
    int dCh=0;
    if (ioctl( sd, SOUND_PCM_READ_CHANNELS, &dCh) == -1) {
        perror("ioctl(\"SNDCTL_PCM_READ_CHANNELS\")");
    }
    return dCh;
}

int Device::getDeviceFragSize() {
    int frag_size;
    
    if (ioctl( sd, SNDCTL_DSP_GETBLKSIZE, &frag_size) == -1) {
//      qDebug("no fragsize");
    } else {
//      qDebug("driver says frag size is %d", frag_size);
    }
    return frag_size;
}

bool Device::setFragSize(int frag) {
    if (ioctl(sd, SNDCTL_DSP_SETFRAGMENT, &frag)) {
        perror("ioctl(\"SNDCTL_DSP_SETFRAGMENT\")");
        return false;
    }
    return true;
}

bool Device::reset() {
  closeDevice(true);
  openDsp();
         if (ioctl( sd, SNDCTL_DSP_RESET, 0) == -1) {
             perror("ioctl(\"SNDCTL_DSP_RESET\")");
       return false;
         }
   return true;
}

int Device::devRead(int soundDescriptor, short *buf, int size) {
   int number = 0;
   number = ::read(  soundDescriptor, (char *)buf, size);
   return number;
}

int Device::devWrite(int soundDescriptor, short * buf, int size) {
   int bytesWritten = 0;
   bytesWritten = ::write( soundDescriptor, buf, size);
   return bytesWritten;
}


