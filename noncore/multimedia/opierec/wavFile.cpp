//wavFile.cpp
#include "wavFile.h"
#include "qtrec.h"

#include <qmessagebox.h>
#include <qdir.h>

#include <qpe/timestring.h>
#include <qpe/config.h>

#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <fcntl.h>
#include <math.h>
#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

WavFile::WavFile( QObject * parent,const QString &fileName, bool makeNwFile, int sampleRate,
                  int channels, int resolution, int format )
        : QObject( parent)
{
//qDebug("new wave file");
    bool b =  makeNwFile;
    wavSampleRate=sampleRate;
    wavFormat=format;
    wavChannels=channels;
    wavResolution=resolution;
    useTmpFile=false;
    if( b) {
        newFile();
    } else {
        openFile(fileName);
    }
}

bool WavFile::newFile() {

//  qDebug("Set up new file");
  Config cfg("OpieRec");
  cfg.setGroup("Settings");

  currentFileName=cfg.readEntry("directory",QDir::homeDirPath());
  QString date;
  QDateTime dt = QDateTime::currentDateTime();
  date = dt.toString();//TimeString::dateString( QDateTime::currentDateTime(),false,true);
  date.replace(QRegExp("'"),"");
  date.replace(QRegExp(" "),"_");
  date.replace(QRegExp(":"),".");
  date.replace(QRegExp(","),"");

  QString currentFile=date;
  if(currentFileName.right(1).find("/",0,true) == -1)
    currentFileName += "/" + date;
  else
    currentFileName += date;
  currentFileName+=".wav";

//  qDebug("set up file for recording: "+currentFileName);
  char pointer[] = "/tmp/opierec-XXXXXX";
  int fd = 0;

  if( currentFileName.find("/mnt",0,true) == -1
      && currentFileName.find("/tmp",0,true) == -1 ) {
    // if destination file is most likely in flash (assuming jffs2)
    // we have to write to a different filesystem first
        
    useTmpFile = true;
    if(( fd = mkstemp( pointer)) < 0 ) {
       perror("mkstemp failed");
       return false;
    }
    
//    qDebug("Opening tmp file %s",pointer);
    track.setName( pointer);

  } else { //just use regular file.. no moving
      
    useTmpFile = false;
    track.setName( currentFileName);
  }
  if(!track.open( IO_ReadWrite | IO_Truncate)) {
    QString errorMsg=(QString)strerror(errno);
    qDebug(errorMsg);
    QMessageBox::message("Note", "Error opening file.\n" +errorMsg);
  
    return false;
  } else {
    setWavHeader( track.handle() , &hdr);
  }
return true;
}

WavFile::~WavFile() {

    closeFile();
}

void WavFile::closeFile() {
    if(track.isOpen()) 
        track.close();
}

int WavFile::openFile(const QString &currentFileName) {
//    qDebug("open play file "+currentFileName);
    closeFile();
    
  track.setName(currentFileName);

  if(!track.open(IO_ReadOnly)) {
    QString errorMsg=(QString)strerror(errno);
    qDebug("<<<<<<<<<<< "+errorMsg+currentFileName);
    QMessageBox::message("Note", "Error opening file.\n" +errorMsg);
    return -1;
  } else {
    parseWavHeader( track.handle());
  }
  return track.handle();
}
                  
bool WavFile::setWavHeader(int fd, wavhdr *hdr) {

  strncpy((*hdr).riffID, "RIFF", 4); // RIFF
  strncpy((*hdr).wavID, "WAVE", 4); //WAVE
  strncpy((*hdr).fmtID, "fmt ", 4); // fmt
  (*hdr).fmtLen = 16; // format length = 16

  if( wavFormat == WAVE_FORMAT_PCM) {
    (*hdr).fmtTag = 1; // PCM
//    qDebug("set header  WAVE_FORMAT_PCM");
  }
  else {
    (*hdr).fmtTag = WAVE_FORMAT_DVI_ADPCM; //intel ADPCM
 //    qDebug("set header  WAVE_FORMAT_DVI_ADPCM");
  }

  //  (*hdr).nChannels = 1;//filePara.channels;// ? 2 : 1*/; // channels
  (*hdr).nChannels = wavChannels;// ? 2 : 1*/; // channels

  (*hdr).sampleRate = wavSampleRate; //samples per second
  (*hdr).avgBytesPerSec = (wavSampleRate)*( wavChannels*(wavResolution/8)); // bytes per second
  (*hdr).nBlockAlign = wavChannels*( wavResolution/8); //block align
  (*hdr).bitsPerSample = wavResolution; //bits per sample 8, or 16

  strncpy((*hdr).dataID, "data", 4);

  write( fd,hdr, sizeof(*hdr));
   qDebug("writing header: bitrate%d, samplerate %d,  channels %d",
          wavResolution, wavSampleRate, wavChannels);
  return true;
}

bool WavFile::adjustHeaders(int fd, int total) {
  lseek(fd, 4, SEEK_SET);
  int i = total + 36;
  write( fd, &i, sizeof(i));
  lseek( fd, 40, SEEK_SET);
  write( fd, &total, sizeof(total));
  qDebug("adjusting header %d", total);
  return true;
}

int WavFile::parseWavHeader(int fd) {
  qDebug("Parsing wav header");
  char string[4];
  int found;
  short fmt;
  unsigned short ch, bitrate;
  unsigned long samplerrate, longdata;

  if (read(fd, string, 4) < 4) {
    qDebug(" Could not read from sound file.\n");
    return -1;
  }
  if (strncmp(string, "RIFF", 4)) {
    qDebug(" not a valid WAV file.\n");
    return -1;
  }
  lseek(fd, 4, SEEK_CUR);
  if (read(fd, string, 4) < 4) {
    qDebug("Could not read from sound file.\n");
    return -1;
  }
  if (strncmp(string, "WAVE", 4)) {
    qDebug("not a valid WAV file.\n");
    return -1;
  }
  found = 0;

  while (!found) {
    if (read(fd, string, 4) < 4) {
      qDebug("Could not read from sound file.\n");
      return -1;
    }
    if (strncmp(string, "fmt ", 4)) {
      if (read(fd, &longdata, 4) < 4) {
        qDebug("Could not read from sound file.\n");
        return -1;
      }
      lseek(fd, longdata, SEEK_CUR);
    } else {
      lseek(fd, 4, SEEK_CUR);
      if (read(fd, &fmt, 2) < 2) {
        qDebug("Could not read format chunk.\n");
        return -1;
      }
      if (fmt != WAVE_FORMAT_PCM && fmt != WAVE_FORMAT_DVI_ADPCM) {
        qDebug("Wave file contains unknown format."
               " Unable to continue.\n");
        return -1;
      }
      wavFormat = fmt;
      //      compressionFormat=fmt;
      qDebug("compressionFormat is %d", fmt);
      if (read(fd, &ch, 2) < 2) {
        qDebug("Could not read format chunk.\n");
        return -1;
      } else {
        wavChannels = ch;
        qDebug("File has %d channels", ch);
      }
      if (read(fd, &samplerrate, 4) < 4) {
        qDebug("Could not read from format chunk.\n");
        return -1;
      } else {
        wavSampleRate = samplerrate;
        //                sampleRate = samplerrate;
        qDebug("File has samplerate of %d",(int) samplerrate);
      }
      lseek(fd, 6, SEEK_CUR);
      if (read(fd, &bitrate, 2) < 2) {
        qDebug("Could not read format chunk.\n");
        return -1;
      }  else {
        wavResolution=bitrate;
        //                 resolution =  bitrate;
        qDebug("File has bitrate of %d", bitrate);
      }
      found++;
    }
  }
  found = 0;
  while (!found) {
    if (read(fd, string, 4) < 4) {
      qDebug("Could not read from sound file.\n");
      return -1;
    }

    if (strncmp(string, "data", 4)) {
      if (read(fd, &longdata, 4)<4) {
        qDebug("Could not read from sound file.\n");
        return -1;
      }

      lseek(fd, longdata, SEEK_CUR);
    } else {
      if (read(fd, &longdata, 4) < 4) {
        qDebug("Could not read from sound file.\n");
        return -1;
      } else {
        wavNumberSamples =  longdata;
         qDebug("file has length of %d \nlasting %d seconds", (int)longdata,
        (int)(( longdata / wavSampleRate) / wavChannels) / ( wavChannels*( wavResolution/8)) );
//        wavSeconds = (( longdata / wavSampleRate) / wavChannels) / ( wavChannels*( wavResolution/8));
        
  return longdata;
      }
    }
  }

  lseek(fd, 0, SEEK_SET);

  return 0;
}

QString WavFile::trackName() {
    return track.name();
}

int WavFile::wavHandle(){
  return track.handle();
}

int WavFile::getFormat() {
return wavFormat;
}

int WavFile::getResolution() {
return wavResolution;
}

int WavFile::getSampleRate() {
 return wavSampleRate;
}

int WavFile::getNumberSamples() {
 return wavNumberSamples;
}

bool WavFile::isTempFile() {
return useTmpFile;
}

int WavFile::getChannels() {

   return wavChannels;
}
