/****************************************************************************
* libtremorplugin.cpp
*
* Copyright (C) 2002 Latchesar Ionkov <lucho@ionkov.net>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/
// fixed and adapted for opieplayer 2003 ljp <llornkcor@handhelds.org>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <locale.h>
#include <math.h>
#include <assert.h>

#include <qmap.h>

#include "libtremorplugin.h"


extern "C" {
#include "tremor/ivorbiscodec.h"
#include "tremor/ivorbisfile.h"
}


#define MPEG_BUFFER_SIZE    65536
//#define debugMsg(a) qDebug(a)
#define debugMsg(a)


class LibTremorPluginData {
public:
  char* filename;
  FILE* f;
  OggVorbis_File vf;
  vorbis_info* vi;
  vorbis_comment* vc;
  bool bos;
  int csection;
  QString finfo;
};


LibTremorPlugin::LibTremorPlugin() {
qDebug("<<<<<<<<<<<<<TREMOR!!!!!>>>>>>>>>>>>>>>>>>");
 d = new LibTremorPluginData;
  d->f = 0;
  d->vi = 0;
  d->vc = 0;
  d->bos = 0;
  d->csection = 0;
  d->finfo = "";
}


LibTremorPlugin::~LibTremorPlugin() {
  close();
  delete d;
}


bool LibTremorPlugin::isFileSupported( const QString& path ) {
    debugMsg( "LibTremorPlugin::isFileSupported" );

    // Mpeg file extensions
    //  "mp2","mp3","m1v","m2v","m2s","mpg","vob","mpeg",ac3"
    // Other media extensions
    //  "wav","mid","mod","s3m","ogg","avi","mov","sid"

    char *ext = strrchr( path.latin1(), '.' );

    // Test file extension
    if ( ext ) {
  if ( strncasecmp(ext, ".ogg", 4) == 0 )
      return TRUE;
    }

    return FALSE;
}


bool LibTremorPlugin::open( const QString& path ) {
  debugMsg( "LibTremorPlugin::open" );

  d->filename = (char*) path.latin1();
  d->f = fopen( d->filename, "r" );
  if (d->f == 0) {
    qDebug("error opening %s", d->filename );
    return FALSE;
  }

  if (ov_open(d->f, &d->vf, NULL, 0) < 0) {
    qDebug("error opening %s", d->filename);
    return FALSE;
  }

  d->vc = ov_comment(&d->vf, -1);
  d->vi = ov_info(&d->vf, -1);
  d->bos = false;

  QString comments[] = { "title", "artist", "album", "year", "tracknumber", "" };
  QString cdescr[] = { "Title", "Artist", "Album", "Year", "Track", "" };


  QMap<QString, QString> cmap;
  char** cptr = d->vc->user_comments;

  while (*cptr != 0) {
    QString s(*cptr);
    int n = s.find('=');

    if (n < 0) {
      continue;
    }

    QString key = s.left(n).lower();
    QString value = s.mid(n+1);

    cmap[key] = value;
    cptr++;
  }

  d->finfo = "";
  for(int i = 0; !comments[i].isEmpty(); i++) {
    QString v = cmap[comments[i].lower()];

    if (!v.isEmpty()) {
      if (!d->finfo.isEmpty()) {
        d->finfo += ", ";
      }

      d->finfo += cdescr[i] + ": " + v;
    }
  }

  qDebug("finfo: " + d->finfo);

  return TRUE;
}


bool LibTremorPlugin::close() {
    debugMsg( "LibTremorPlugin::close" );

    int result = TRUE;

    if (fclose(d->f) == -1) {
        qDebug("error closing file %s", d->filename);
  result = FALSE;
    }

    d->f = 0;
    d->finfo = "";

    return result;
}


bool LibTremorPlugin::isOpen() {
  debugMsg( "LibTremorPlugin::isOpen" );
  return ( d->f != 0 );
}


const QString &LibTremorPlugin::fileInfo() {
  return d->finfo;
}

int LibTremorPlugin::audioStreams() {
  debugMsg( "LibTremorPlugin::audioStreams" );
  return 1;
}


int LibTremorPlugin::audioChannels( int ) {
  qDebug( "LibTremorPlugin::audioChannels: %i", d->vi->channels );
  return d->vi->channels;
}


int LibTremorPlugin::audioFrequency( int ) {
  qDebug( "LibTremorPlugin::audioFrequency: %ld", d->vi->rate );
  return d->vi->rate;
}


int LibTremorPlugin::audioSamples( int ) {
    debugMsg( "LibTremorPlugin::audioSamples" );
    return (int) ov_pcm_total(&d->vf,-1);
}


bool LibTremorPlugin::audioSetSample( long, int ) {
    debugMsg( "LibTremorPlugin::audioSetSample" );
    return FALSE;
}


long LibTremorPlugin::audioGetSample( int ) {
    debugMsg( "LibTremorPlugin::audioGetSample" );
    return 0;
}


bool LibTremorPlugin::audioReadSamples( short *output, int, long samples, long& samplesMade, int ) {
//  qDebug( "<<<<<<<<<<<<LibTremorPlugin::audioReadStereoSamples %d", samples );

  int old_section = d->csection;

  char* buf = (char*) output;
  int length = samples * 4;

  if ( samples == 0 )
    return false;

  while (length > 0) {
    if (d->bos) {
      d->vi = ov_info(&d->vf, -1);
      d->vc = ov_comment(&d->vf, -1);
    }

    int n = 4096;
    if (length < n) {
      n = length;
    }

    long ret = ov_read(&d->vf, buf, n, &d->csection);
//    qDebug("%d", ret);
    if (ret == 0) {
      break;
    } else if (ret < 0) {
      return true;
    }

    if (old_section != d->csection) {
      d->bos = true;
    }

    buf += ret;
    length -= ret;

  }

  samplesMade = samples;

  return true;
}

double LibTremorPlugin::getTime() {
    debugMsg( "LibTremorPlugin::getTime" );
    return 0.0;
}

