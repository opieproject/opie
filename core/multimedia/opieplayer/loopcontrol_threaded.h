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
#ifndef MPEGVIEW_H
#define MPEGVIEW_H



#include <qwidget.h>
#include <qdatetime.h>


class LoopControl : public QObject {
    Q_OBJECT
public:
    LoopControl( QObject *parent, const char *name );
    ~LoopControl();

    bool init( const QString& filename );

    bool hasVideo() const { return hasVideoChannel; }
    bool hasAudio() const { return hasAudioChannel; }
    
    long totalPlaytime() { return (long)(hasVideoChannel ? total_video_frames / framerate : total_audio_samples / freq); }

    // These are public to run them from global functions needed to start threads
    // Otherwise they would be private
    void startAudio();
    void startVideo();
public slots:
    void play();
    void stop( bool willPlayAgainShortly = FALSE );

    void setMute( bool );
    void setPaused( bool );
    void setPosition( long );
    
signals:
    void positionChanged( long, long );
    void playFinished();

protected:
    void timerEvent(QTimerEvent*);

private:
    void startTimers();
    void killTimers();


    QTime playtime;
    int timerid;
    int audioSampleCounter;
    long current_frame;
    long total_video_frames;
    long total_audio_samples;
    
    float framerate;
    int freq;
    int stream;
    int framecount;
    int channels;
     
    bool moreAudio;
    bool moreVideo;

    bool hasVideoChannel;
    bool hasAudioChannel;
    bool isMuted;
    QString fileName;
};


#endif

