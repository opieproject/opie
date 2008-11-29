
#ifndef DEVICE_H
#define DEVICE_H

#include <qstring.h>
#include <alsa/asoundlib.h>

class Device {
public:
    Device( QString deviceName );
    ~Device();
    bool openDevice(bool record = false);
    bool closeDevice(bool drop = false);
    unsigned int getChannels();
    snd_pcm_format_t getFormat();
    int getInVolume();
    int getOutVolume();
    unsigned int getRate();
    void changedInVolume(int);
    void changedOutVolume(int);
    bool setFragSize(int);
    bool setDeviceChannels(unsigned int);
    bool setDeviceRate(unsigned int);
    bool setDeviceFormat(snd_pcm_format_t);
    bool reset();
    int init();

    int devRead(char *buffer);
    int devWrite(char *buffer);

private:
    snd_pcm_format_t devForm; 
    unsigned int devCh, devRate;
    QString dspstr, mixstr;
    bool selectMicInput();

protected:
    snd_pcm_t *m_handle;
    snd_pcm_hw_params_t *m_hwparams;
    QString m_deviceName;
    snd_pcm_uframes_t m_frames;
};

#endif
