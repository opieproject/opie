/**************************************************************************************94x78**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************************************/

/*
 * $Id: vmemo.cpp,v 1.2 2002-02-14 02:13:19 llornkcor Exp $
 */

#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <string.h>
#include <stdlib.h>

typedef struct _waveheader {
  u_long  main_chunk; /* 'RIFF' */
  u_long  length;   /* filelen */
  u_long  chunk_type; /* 'WAVE' */

  u_long  sub_chunk;  /* 'fmt ' */
  u_long  sc_len;   /* length of sub_chunk, =16        (chunckSize)         */
  u_short format;   /* should be 1 for PCM-code        (formatTag)          */
  u_short modus;    /* 1 Mono, 2 Stereo                (channels)           */
  u_long  sample_fq;  /* samples per second              (samplesPerSecond)   */
  u_long  byte_p_sec; /* avg bytes per second            (avgBytePerSecond)   */
  u_short byte_p_spl; /* samplesize; 1 or 2 bytes        (blockAlign)         */
  u_short bit_p_spl;  /* 8, 12 or 16 bit                 (bitsPerSample)      */

  u_long  data_chunk; /* 'data' */
  u_long  data_length;  /* samplecount */
} WaveHeader;

#define RIFF    0x46464952
#define WAVE    0x45564157
#define FMT   0x20746D66
#define DATA    0x61746164
#define PCM_CODE  1
#define WAVE_MONO 1
#define WAVE_STEREO 2

#include "vmemo.h"

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qpainter.h>
#include <qdatetime.h>
#include <qsound.h>
#include <qfile.h>

int seq = 0;

/* XPM */
static char * vmemo_xpm[] = {
"14 14 47 1",
"   c None",
".  c #101010",
"+  c #0D0D0D",
"@  c #0B0B0B",
"#  c #393939",
"$  c #6C6C6C",
"%  c #5D5D5D",
"&  c #4E4E4E",
"*  c #1C1C1C",
"=  c #131313",
"-  c #7B7B7B",
";  c #303030",
">  c #383838",
",  c #3F3F3F",
"'  c #343434",
")  c #060606",
"!  c #444444",
"~  c #646464",
"{  c #505050",
"]  c #717171",
"^  c #1B1B1B",
"/  c #585858",
"(  c #4C4C4C",
"_  c #353535",
":  c #0E0E0E",
"<  c #080808",
"[  c #262626",
"}  c #121212",
"|  c #7F7F7F",
"1  c #464646",
"2  c #0C0C0C",
"3  c #727272",
"4  c #292929",
"5  c #656565",
"6  c #565656",
"7  c #434343",
"8  c #272727",
"9  c #0F0F0F",
"0  c #3A3A3A",
"a  c #090909",
"b  c #535353",
"c  c #545454",
"d  c #494949",
"e  c #7A7A7A",
"f  c #202020",
"g  c #3D3D3D",
"h  c #1F1F1F",
"         .+@  ",
"        #$%&* ",
"       =-;>,')",
"       .$;!~,)",
"        ;#{]!)",
"        ^~/(_)",
"       ./:@<[)",
"  }.  .|]1;;2 ",
" #-$;^/3&;;4@ ",
".$;;#5:67;89  ",
":%;0%&ab;8.   ",
"@cd%e!fg49    ",
" h0,!_;2@     ",
"  )))))       "};

VMemo::VMemo( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
  setFixedHeight( 18 );
  setFixedWidth( 14 );

  recording = FALSE;
  
  struct utsname name; //check for embedix kernel running on the zaurus, if lineo change string, this break
  if (uname(&name) != -1) {
      QString release=name.release;
      if(release.find("embedix",0,TRUE) !=-1) {
          systemZaurus=TRUE;
          printf("System release: %s\n", name.release);
      } else
          systemZaurus=FALSE;
  }

}

VMemo::~VMemo()
{
}

void VMemo::paintEvent( QPaintEvent* )
{
  QPainter p(this);
  p.drawPixmap( 0, 1,( const char** )  vmemo_xpm );
}

void VMemo::mousePressEvent( QMouseEvent * )
{
  qWarning("VMemo::mousePress()");
  QSound::play(Resource::findSound("vmemob"));

  recording = TRUE;
  qWarning("VMemo::mousePress() -> Starting to record");
  if (openDSP() == -1)
  {
    // ### Display an error box
    qWarning("VMemo::mousePress() -> DSP error");
    recording = FALSE;
    return;
  }

  Config vmCfg("VMemo");
  vmCfg.setGroup("Defaults");

  QDateTime dt = QDateTime::currentDateTime();
  QString fileName;
  if(systemZaurus)
      fileName=vmCfg.readEntry("Dir", "/mnt/cf/"); // zaurus does not have /mnt/ramfs
  else
      fileName=vmCfg.readEntry("Dir", "/mnt/ramfs/");

  fileName += "vm_";
  fileName += dt.toString();
  fileName += ".wav";

  // No spaces in the filename
  fileName.replace(QRegExp("'"),"");
  fileName.replace(QRegExp(" "),"_");
  fileName.replace(QRegExp(":"),".");
  fileName.replace(QRegExp(","),"");

  if(openWAV(fileName.latin1()) == -1)
  {
    // ### Display an error box
    qWarning("VMemo::mousePress() -> WAV error");
    close(dsp);
    return;
  }

  QArray<int> cats(1);
  cats[0] = vmCfg.readNumEntry("Category", 0);

  QString dlName("vm_");
  dlName += dt.toString();
  DocLnk l;
  l.setFile(fileName);
  l.setName(dlName);
  l.setType("audio/x-wav");
  l.setCategories(cats);
  l.writeLink();

  record();
}

void VMemo::mouseReleaseEvent( QMouseEvent * )
{
  qWarning("VMemo::mouseRelese() -> Done recording");
  recording = FALSE;
  QSound::play(Resource::findSound("vmemoe"));
}

int VMemo::openDSP()
{
  Config cfg("Sound");
  cfg.setGroup("Record");

  speed = cfg.readNumEntry("SampleRate", 11025);
  channels = cfg.readNumEntry("Stereo", 1) ? 2 : 1; // 1 = stereo(2), 0 = mono(1)
  if (cfg.readNumEntry("SixteenBit", 1))
  {
    format = AFMT_S16_LE;
    resolution = 16;
  }
  else
  {
    format = AFMT_U8;
    resolution = 8;
  }
  
  if(systemZaurus)
      dsp = open("/dev/dsp1", O_RDWR); //Zaurus needs /dev/dsp1
  else 
      dsp = open("/dev/dsp", O_RDWR);

  qWarning("speed = %i", speed);

  if(dsp == -1)
  {
      perror("open(\"/dev/dsp\")");
      return -1;
  }

  if(ioctl(dsp, SNDCTL_DSP_SETFMT , &format)==-1) {
    perror("ioctl(\"SNDCTL_DSP_SETFMT\")");
    return -1;
  }
  if(ioctl(dsp, SNDCTL_DSP_CHANNELS , &channels)==-1) {
      perror("ioctl(\"SNDCTL_DSP_CHANNELS\")");
      return -1;
  }
  if(ioctl(dsp, SNDCTL_DSP_SPEED , &speed)==-1) {
      perror("ioctl(\"SNDCTL_DSP_SPEED\")");
      return -1;
  }
  if(ioctl(dsp, SOUND_PCM_READ_RATE , &rate)==-1)  {
      perror("ioctl(\"SOUND_PCM_READ_RATE\")");
      return -1;
  }
  qWarning("speed = %i", speed);

  return 1;
}

int VMemo::openWAV(const char *filename)
{
  char buffer[256];

  qDebug("Creating %s ",filename);
  track.setName(filename);
  if(!track.open(IO_WriteOnly|IO_Truncate|IO_Raw))
  {
    qDebug("Could not open file");
    return -1;
  }
  wav=track.handle();

  WaveHeader wh;

  /*
  wh.main_chunk = RIFF;
  wh.length     = 0;
  wh.chunk_type = WAVE;
  wh.sub_chunk  = FMT;
  wh.sc_len     = 16;
  wh.format     = PCM_CODE;
  wh.modus      = channels;
  wh.sample_fq  = speed;
  wh.byte_p_spl = ((resolution == 8) ? 1 : 2) * (channels ? 2 : 1);
  wh.byte_p_sec = resolution; //speed * wh.modus * wh.byte_p_spl;
  wh.bit_p_spl  = resolution;
  wh.data_chunk = DATA;
  wh.data_length= 0;
  */

  wh.main_chunk = RIFF;
  wh.length     = 0;
  wh.chunk_type = WAVE;

  wh.sub_chunk  = FMT;
  wh.sc_len     = 16;
  wh.format     = PCM_CODE;
  wh.modus      = channels;
  wh.sample_fq  = speed;
  wh.bit_p_spl  = resolution;
  wh.byte_p_sec = wh.sample_fq * wh.bit_p_spl;
  wh.byte_p_spl = channels * (wh.bit_p_spl % 8);

  qWarning("channels = %i\n"
       "samplesPerSecond = %i\n"
       "avgBytesPerSecond = %i\n"
       "blockAlign = %i\n"
       "bitsPerSecond = %i\n", wh.modus, wh.sample_fq, wh.byte_p_sec, wh.byte_p_spl, wh.bit_p_spl);

  wh.data_chunk = DATA;
  wh.data_length= 0;

  write (wav, &wh, sizeof(WaveHeader));

  return 1;
}

void VMemo::record(void)
{
  int length=0, result, value;
  char sound[8192];

  qWarning("VMemo::record()");

  while(recording)
  {
    result = read(dsp, sound, 8192);
    write(wav, sound, result);
    length += result;
    qApp->processEvents();
  }

  qWarning("VMemo::record() -> Done recording");
  qWarning("VMemo::record() -> Closing dsp");

  value = length+36;
  lseek(wav, 4, SEEK_SET);
  write(wav, &value, 4);
  lseek(wav, 40, SEEK_SET);
  write(wav, &length, 4);
  track.close();

  if( ioctl( dsp, SNDCTL_DSP_RESET,0) == -1)// ); //tell driver to stop for a while
  perror("ioctl(\"SNDCTL_DSP_RESET\")");
  ::close(dsp);

  qWarning("VMemo::record() -> terminating");
}
