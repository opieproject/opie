// device.cpp

#include "device.h"

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie2/odebug.h>

/* STD */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

//extern QtRec *qperec;
#if defined(QT_QWS_SL5XXX)
///#if defined(QT_QWS_EBX)

#define DSPSTROUT "/dev/dsp"
#define DSPSTRMIXEROUT "/dev/mixer"

#ifdef SHARP
#define DSPSTRIN "/dev/dsp1"
#define DSPSTRMIXERIN "/dev/mixer1"
#else
#define DSPSTRIN "/dev/dsp"
#define DSPSTRMIXERIN "/dev/mixer"
#endif

#else

#ifdef QT_QWS_DEVFS
#define DSPSTROUT "/dev/sound/dsp"
#define DSPSTRIN "/dev/sound/dsp"
#define DSPSTRMIXERIN "/dev/sound/mixer"
#define DSPSTRMIXEROUT "/dev/sound/mixer"
#else
#define DSPSTROUT "/dev/dsp"
#define DSPSTRIN "/dev/dsp"
#define DSPSTRMIXERIN "/dev/mixer"
#define DSPSTRMIXEROUT "/dev/mixer"
#endif

#endif

Device::Device( QObject * parent, bool record )
        : QObject( parent)
{
//    dspstr = dsp;
    devForm = -1;
    devCh = -1;
    devRate = -1;

    if( !record){ //playing
        owarn << "setting up DSP for playing" << oendl;
        flags = O_WRONLY;
    } else { //recording
        owarn << "setting up DSP for recording" << oendl;
        flags = O_RDWR;
//        flags = O_RDONLY;
				selectMicInput();
    }
}

bool Device::openDsp() {
    if( openDevice( flags) == -1) {
        perror("<<<<<<<<<<<<<<ioctl(\"Open device\")");
        return false;
    }
    return true;
}

int Device::openDevice( int flags) {
		owarn << "Opening"<< dspstr;

		if (( sd = ::open( DSPSTROUT, flags)) == -1) {
				perror("open(\"/dev/dsp\")");
				QString errorMsg="Could not open audio device\n /dev/dsp\n"
						+(QString)strerror(errno);
				qDebug( "XXXXXXXXXXXXXXXXXXXXXXX  "+errorMsg );
				return -1;
		}

		if(ioctl(sd,SNDCTL_DSP_RESET,0)<0){
				perror("ioctl RESET");
		}
    return sd;
}

int Device::getInVolume() {
    unsigned int volume = 0;
    Config cfg("qpe");
    cfg.setGroup("Volume");

    return cfg.readNumEntry("Mic");
}

int Device::getOutVolume( ) {
    unsigned int volume;
    Config cfg("qpe");
    cfg.setGroup("Volume");

    return cfg.readNumEntry("VolumePercent");
}


void Device::changedInVolume(int vol ) {
		Config cfg("qpe");
		cfg.setGroup("Volume");
		cfg.writeEntry("Mic", QString::number(vol ));
		QCopEnvelope( "QPE/System", "micChange(bool)" ) << false;
}

void Device::changedOutVolume(int vol) {
		Config cfg("qpe");
		cfg.setGroup("Volume");
		cfg.writeEntry("VolumePercent", QString::number( vol ));

		QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << false;

		owarn << "changing output vol " << vol << oendl;
}

bool Device::selectMicInput() {

		int md = 0;
		int info = SOUND_MASK_MIC;//MIXER_WRITE(SOUND_MIXER_MIC);
		owarn << "sectMicInput" << oendl;
		md = ::open( DSPSTRMIXEROUT, O_RDWR );

		if ( md <= 0) {
				QString err;
				err.sprintf("open %s", DSPSTRMIXEROUT);
				perror(err.latin1());
		} else {
				if( ioctl( md, SOUND_MIXER_WRITE_RECSRC, &info) == -1)
						perror("ioctl(\"SOUND_MIXER_WRITE_RECSRC\")");
				::close(md);
				return false;
		}
		::close(md);

    return true;
}

bool Device::closeDevice( bool) {
    ::close( sd); //close sound device
    return true;
}

bool Device::setDeviceFormat( int form) {
    qDebug( "set device res %d: %d ",form, sd );
    if (ioctl( sd, SNDCTL_DSP_SETFMT,  &form)==-1) { //set format
        perror("ioctl(\"SNDCTL_DSP_SETFMT\")");
        return false;
    }
    devRes=form;
    return true;
}

bool Device::setDeviceChannels( int ch) {
    qDebug( "set channels %d: %d",ch ,sd);
    if (ioctl( sd, SNDCTL_DSP_CHANNELS, &ch)==-1) {
        perror("ioctl(\"SNDCTL_DSP_CHANNELS\")");
        return false;
    }
    devCh=ch;
    return true;
}

bool Device::setDeviceRate( int rate) {
    qDebug( "set rate %d: %d",rate,sd);
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
    int dRate = 0;
    if (ioctl( sd, SOUND_PCM_READ_RATE, &dRate) == -1) {
        perror("ioctl(\"SNDCTL_PCM_READ_RATE\")");
    }
    return dRate;

}

int Device::getDeviceBits() {
    int dBits = 0;
     if (ioctl( sd, SOUND_PCM_READ_BITS, &dBits) == -1) {
         perror("ioctl(\"SNDCTL_PCM_READ_BITS\")");
     }
    return dBits;
}

int Device::getDeviceChannels() {
    int dCh = 0;
    if (ioctl( sd, SOUND_PCM_READ_CHANNELS, &dCh) == -1) {
        perror("ioctl(\"SNDCTL_PCM_READ_CHANNELS\")");
    }
    return dCh;
}

int Device::getDeviceFragSize() {
    int frag_size;

    if (ioctl( sd, SNDCTL_DSP_GETBLKSIZE, &frag_size) == -1) {
      qDebug( "no fragsize" );
    } else {
      qDebug( "driver says frag size is %d",frag_size);
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
		Q_UNUSED(soundDescriptor);
		int number = 0;
		number = ::read(  sd /*soundDescriptor*/, (char *)buf, size);
		return number;
}

int Device::devWrite(int soundDescriptor, short * buf, int size) {
		Q_UNUSED(soundDescriptor);
		int bytesWritten = 0;
		bytesWritten = ::write( sd /*soundDescriptor*/, buf, size);
		return bytesWritten;
}
