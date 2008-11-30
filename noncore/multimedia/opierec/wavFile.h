//wavFile.h
#ifndef WAVFILE_H
#define WAVFILE_H

#include <qobject.h>
#include <qfile.h>
#include <qstring.h>

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
    wavhdr hdr;
    wavhdrext_ima imaext;
    wavdatahdr datahdr;
    wavfactblk factblk;
    bool adjustHeaders(int fd, unsigned long total);
    QString currentFileName;
    QString trackName();
    
    QFile track;
    int wavHandle();
    int getFormat();
    int getResolution();
    int getSampleRate();
    int getNumberSamples();
    int getChannels();
    bool isTempFile();
    int openFile(const QString &);
    bool newFile();
    void closeFile();

private:
    int wavFormat, wavChannels, wavResolution, wavSampleRate, wavNumberSamples;
    unsigned short wavSamplesPerBlock;
    bool useTmpFile;
    bool setWavHeader(int fd);
    int parseWavHeader(int fd);
};

#endif
