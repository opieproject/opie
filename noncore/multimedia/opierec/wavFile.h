//wavFile.h
#ifndef WAVFILE_H
#define WAVFILE_H

#include <qobject.h>
#include <qfile.h>
#include <qstring.h>

#define WAVE_FORMAT_DVI_ADPCM (0x0011)
#define WAVE_FORMAT_PCM (0x0001)

typedef struct { 
    char           riffID[4];
    unsigned long  riffLen;
    char           wavID[4];
    char           fmtID[4];
    unsigned long  fmtLen;
    unsigned short fmtTag;
    unsigned short nChannels;
    unsigned long  sampleRate;
    unsigned long  avgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short bitsPerSample;
} wavhdr;

typedef struct {
    unsigned short wExtSize;
    unsigned short wSamplesPerBlock;
} wavhdrext_ima;

typedef struct {
    char           factID[4];
    unsigned long  dwFactSize;
    unsigned long  dwSamplesWritten;
} wavfactblk;

typedef struct {
    char           dataID[4];
    unsigned long  dataLen;
} wavdatahdr;

class WavFile : public QObject {
Q_OBJECT
public:
    WavFile( QObject * parent=0,const QString &fileName=0, bool newFile=0, int sampleRate=0,
             int channels=0 , int resolution=0, int format=0, unsigned short samplesPerBlock=0);
    ~WavFile();
    bool adjustHeaders(int fd, unsigned long total);

    int wavHandle();
    int getFormat();
    int getResolution();
    int getSampleRate();
    int getNumberSamples();
    int getChannels();
    QString getFileName();
    int openFile();
    int createFile();
    void closeFile();
    bool isOpen();

private:
    int wavFormat, wavChannels, wavResolution, wavSampleRate, wavNumberSamples;
    unsigned short wavSamplesPerBlock;
    wavhdr hdr;
    wavhdrext_ima imaext;
    wavdatahdr datahdr;
    wavfactblk factblk;
    QFile track;

    bool setWavHeader(int fd);
    int parseWavHeader(int fd);
};

#endif
