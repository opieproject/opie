/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <qwidget.h>
#include <qpixmap.h>

#include <alsa/asoundlib.h>


class QLabel;


class Waveform : public QWidget
{
public:
    Waveform( QWidget *parent=0, const char *name=0, WFlags fl=0 );
    ~Waveform();

public:
    void changeSettings( int frequency, int channels, snd_pcm_format_t format );
    void reset();
    void newSamples( const char *buf, int len );

private:

    void makePixmap();
    void draw();

protected:

    void paintEvent( QPaintEvent *event );

private:
    int samplesPerPixel;
    int currentValue;
    int numSamples;
    snd_pcm_format_t sndFormat;
    short *window;
    int windowPosn;
    int windowSize;
    QPixmap *pixmap;

};


#endif

