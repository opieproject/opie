/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <qfile.h>
#include "wavplugin.h"

//#define debugMsg(a)     qDebug(a)
#define debugMsg(a)


struct RiffChunk {
    char id[4];
    Q_UINT32 size;
    char data[4];
};


struct ChunkData {
    Q_INT16 formatTag;
    Q_INT16 channels;
    Q_INT32 samplesPerSec;
    Q_INT32 avgBytesPerSec;
    Q_INT16 blockAlign;
    Q_INT16 wBitsPerSample;
};


const int sound_buffer_size = 4096;


class WavPluginData {
public:
    QFile *input;

    int wavedata_remaining;
    ChunkData chunkdata;
    RiffChunk chunk;
    uchar data[sound_buffer_size+32]; // +32 to handle badly aligned input data
    int out,max;
    int samples_due;
    int samples;
    int freq;
    int chan;
    int sampleRate;
    int resolution;
    
    WavPluginData() {
        max = out = sound_buffer_size;
        wavedata_remaining = 0;
        samples_due = 0;
        samples = -1;
    }

      // expands out samples to the frequency of 44kHz
    bool add( short *output, long count, long& done, bool stereo )
        {
            qDebug("add");
            done = 0;

            if ( input == 0 ) {
                qDebug("no input");
                return FALSE;
            }

            while ( count ) {
                int l,r;
                if ( getSample(l, r) == FALSE ) {
                    qDebug("didn't get sample");
                    return FALSE;
                }
                samples_due += 44100;
                while ( count && (samples_due > chunkdata.samplesPerSec) ) {
                    *output++ = l;
                    if ( stereo )
                        *output++ = r;
                    samples_due -= chunkdata.samplesPerSec;
                    count--;
                    done++;
                }
            }

            return TRUE;
        }

    bool initialise() {
        qDebug("initialize");
 if ( input == 0 )
            return FALSE;

        wavedata_remaining = -1;

        while ( wavedata_remaining == -1 ) {
              // Keep reading chunks...
            const int n = sizeof(chunk) - sizeof(chunk.data);
            int t = input->readBlock( (char*)&chunk, n );
            if ( t != n ) {
                if ( t == -1 )
                    return FALSE;
                return TRUE;
            }
            if ( qstrncmp(chunk.id,"data",4) == 0 ) {
                samples = wavedata_remaining = chunk.size;
            } else if ( qstrncmp(chunk.id,"RIFF",4) == 0 ) {
                char d[4];
                if ( input->readBlock(d,4) != 4 ) {
                    return FALSE;
                }
                if ( qstrncmp(d,"WAVE",4) != 0 ) {
                      // skip
                    if ( chunk.size > 1000000000 || !input->at(input->at()+chunk.size-4) ) {
                        return FALSE;
                    }
                }
            } else if ( qstrncmp(chunk.id,"fmt ",4) == 0 ) {
                if ( input->readBlock((char*)&chunkdata,sizeof(chunkdata)) != sizeof(chunkdata) ) {
                    return FALSE;
                }
#define WAVE_FORMAT_PCM 1
                if ( chunkdata.formatTag != WAVE_FORMAT_PCM ) {
                    qDebug("WAV file: UNSUPPORTED FORMAT %d",chunkdata.formatTag);
                      return FALSE;
                }
            }



            
            else {
                  // ignored chunk
                if ( chunk.size > 1000000000 || !input->at(input->at()+chunk.size) ) {
                    return FALSE;
                }
            }
        } // while

        return TRUE;
    }


      // gets a sample from the file
    bool getSample(int& l, int& r)
        {
            l = r = 0;

            if ( input == 0 )
                return FALSE;

            if ( (wavedata_remaining < 0) || !max )
                return FALSE;

            if ( out >= max ) {
                max = input->readBlock( (char*)data, (uint)QMIN(sound_buffer_size,wavedata_remaining) );

                wavedata_remaining -= max;

                out = 0;
                if ( max <= 0 ) {
                    max = 0;
                    return TRUE;
                }
            }
            if ( resolution == 8 ) {
                l = (data[out++] - 128) * 128;
            } else {
                l = ((short*)data)[out/2];
                out += 2;
            }
            if ( chan == 1 ) {
                r = l;
            } else {
                if ( resolution == 8 ) {
                    r = (data[out++] - 128) * 128;
                } else {
                    r = ((short*)data)[out/2];
                    out += 2;
                }
            }
            return TRUE;
        } // getSample

//////////////////////////////////////////////////////
int getWavSettings(int fd)
{ //this came from wmrecord

    char t1[4];
    unsigned long l1;
    int found;
    short fmt;
    unsigned short ch, brate;
    unsigned long srate;


      /* First read in the RIFF identifier. If this is missing then the
       * file is not a valid WAVE file.
       */
    if (read(fd, t1, 4)<4) {
        qDebug(" Could not read from sound file.\n");
        return -1;
    }
    if (strncmp(t1, "RIFF", 4)) {
        qDebug(" not a valid WAV file.\n");
        return -1;
    }
      /* Advance the file pointer to the next relevant field. */
    lseek(fd, 4, SEEK_CUR);
      /* Read in the WAVE identifier. */
    if (read(fd, t1, 4)<4) {
        qDebug("Could not read from sound file.\n");
        return -1;
    }
    if (strncmp(t1, "WAVE", 4)) {
        qDebug("not a valid WAV file.\n");
        return -1;
    }

      /* Search through the file for the format chunk. If the end of the
       * file is reached without finding the chunk, then the file is not a
       * valid WAVE file.
       */
    found = 0;
    while (!found) {
        if (read(fd, t1, 4)<4) {
            qDebug("Could not read from sound file.\n");
            return -1;
        }
        if (strncmp(t1, "fmt ", 4)) {
              /* Determine the length of the chunk found and skip to the next
               * chunk. The chunk length is always stored in the four bytes
               * following the chunk id.
               */
            if (read(fd, &l1, 4)<4) {
                qDebug("Could not read from sound file.\n");
                return -1;
            }
            lseek(fd, l1, SEEK_CUR);
        }
        else {
              /* This is the format chunk, which stores the playback settings
               * for the recording.
               */
              /* Skip the length field, since we don't really need it. */
            lseek(fd, 4, SEEK_CUR);
              /* Read in the format tag. If it has a value of 1, then there is
               * no compression and we can attempt to play the file
               * back. Otherwise, return.
               */
            if (read(fd, &fmt, 2)<2) {
                qDebug("Could not read from format chunk.\n");
                return -1;
            }
            if (fmt != 1) {
                qDebug("Wave file contains compressed data."
                       " Unable to continue.\n");
                return -1;
            }
              /* Get the stereo mode. */
            if (read(fd, &ch, 2)<2) {
                qDebug("Could not read from format chunk.\n");
                return -1;
            }
            else {
                chan = ch;
                qDebug("File has %d channels", chan);
            }
              /* Get the sample rate. */
            if (read(fd, &srate, 4)<4) {
                qDebug("Could not read from format chunk.\n");
                return -1;
            }
            else {
                sampleRate = srate;
                qDebug("File has samplerate of %d", sampleRate);
            }
              /* Get the bit rate. This is at the end of the format chunk. */
            lseek(fd, 6, SEEK_CUR);
            if (read(fd, &brate, 2)<2) {
                qDebug("Could not read from format chunk.\n");
                return -1;
            }
            else {
                resolution =  brate;
                qDebug("File has bitrate of %d", resolution);
            }

            found++;
        }
    }

      /* Search through the file for the data chunk. If the end of the
       * file is reached without finding the chunk, then the file is not a
       * valid WAVE file.
       */
    found = 0;
    while (!found) {
        if (read(fd, t1, 4)<4) {
            qDebug("Could not read from sound file.\n");
            return -1;
        }
        if (strncmp(t1, "data", 4)) {
              /* Determine the length of the chunk found and skip to the next
               * chunk. The chunk length is always stored in the four bytes
               * following the chunk id.
               */
            if (read(fd, &l1, 4)<4) {
                qDebug("Could not read from sound file.\n");
                return -1;
            }
            lseek(fd, l1, SEEK_CUR);
        }
        else {
              /* This is the data chunk, which stores the recording. */
              /* Get the length field. */
            if (read(fd, &l1, 4)<4) {
                qDebug("Could not read from sound file.\n");
                return -1;
            }
            else  {
                samples =l1;
                qDebug("file has length of %d\nlasting %d seconds",l1, (( l1 / sampleRate) / chan) / 2 ); // ????
                return l1;
            }
        }
    }

    return 0;
}
    
//////////////////////////////////////////////////   
};


WavPlugin::WavPlugin() {
    d = new WavPluginData;
    d->input = 0;
}


WavPlugin::~WavPlugin() {
    close();
    delete d;
}


bool WavPlugin::isFileSupported( const QString& path ) {
    debugMsg( "WavPlugin::isFileSupported" );

    char *ext = strrchr( path.latin1(), '.' );

    // Test file extension
    if ( ext ) {
  if ( strncasecmp(ext, ".raw", 4) == 0 )
      return TRUE;
  if ( strncasecmp(ext, ".wav", 4) == 0 )
      return TRUE;
  if ( strncasecmp(ext, ".wave", 4) == 0 )
      return TRUE;
    }

    return FALSE;
}


bool WavPlugin::open( const QString& path ) {
    qDebug( "WavPlugin::open" );

    d->max = d->out = sound_buffer_size;
    d->wavedata_remaining = 0;
    d->samples_due = 0;

    d->input = new QFile( path );
    if ( d->input->open(IO_ReadOnly) == FALSE ) {
        qDebug("couldn't open file");
        delete d->input;
        d->input = 0;
        return FALSE;
    }
   
//    d->getWavSettings( d->input.handle());
    d->initialise();

    return TRUE;
}


bool WavPlugin::close() {
    qDebug( "WavPlugin::close" );

    d->input->close();
    delete d->input;
    d->input = 0;
    return TRUE;
}


bool WavPlugin::isOpen() {
    qDebug( "WavPlugin::isOpen" );
    return ( d->input != 0 );
}


int WavPlugin::audioStreams() {
    qDebug( "WavPlugin::audioStreams" );
    return 1;
}


int WavPlugin::audioChannels( int ) {
    debugMsg( "WavPlugin::audioChannels" );
    return d->chan;
}


int WavPlugin::audioFrequency( int ) {
    qDebug( "WavPlugin::audioFrequency" );
    return d->freq;
}


int WavPlugin::audioSamples( int ) {
    qDebug( "WavPlugin::audioSamples" );
    return d->samples; 
//      return d->samples * 2 / d->chunkdata.channels; // ### Scaled samples will be made stereo,
//          // Therefore if source is mono we will double the number of samples
}


bool WavPlugin::audioSetSample( long, int ) {
    qDebug( "WavPlugin::audioSetSample" );
    return FALSE;
}


long WavPlugin::audioGetSample( int ) {
    qDebug( "WavPlugin::audioGetSample" );
    return 0;
}

/*
bool WavPlugin::audioReadSamples( short *, int, long, int ) {
    debugMsg( "WavPlugin::audioReadSamples" );
    return FALSE;
}


bool WavPlugin::audioReReadSamples( short *, int, long, int ) {
    debugMsg( "WavPlugin::audioReReadSamples" );
    return FALSE;
}


bool WavPlugin::audioReadMonoSamples( short *output, long samples, long& samplesMade, int ) {
    debugMsg( "WavPlugin::audioReadMonoSamples" );
    return !d->add( output, samples, samplesMade, FALSE );
}


bool WavPlugin::audioReadStereoSamples( short *output, long samples, long& samplesMade, int ) {
    debugMsg( "WavPlugin::audioReadStereoSamples" );
    return !d->add( output, samples, samplesMade, TRUE );
}
*/

bool WavPlugin::audioReadSamples( short *output, int channels, long samples, long& samplesMade, int ) {
    qDebug( "WavPlugin::audioReadSamples" );
    return d->add( output, samples, samplesMade, channels != 1 );
}

double WavPlugin::getTime() {
    qDebug( "WavPlugin::getTime" );
    return 0.0;
}


