
#ifndef MEDIA_PLAYER_STATE_H
#define MEDIA_PLAYER_STATE_H


#include <qobject.h>


class MediaPlayerDecoder;
class Config;


class MediaPlayerState : public QObject {
Q_OBJECT
public:
    MediaPlayerState( QObject *parent, const char *name );
    ~MediaPlayerState();

    bool isStreaming;
    bool fullscreen();
    bool scaled();
    bool looping();
    bool shuffled();
    bool playlist();
    bool paused();
    bool playing();
    long position();
    long length();
    char view();

public slots:
    void setFullscreen( bool b );
    void setScaled( bool b );
    void setLooping( bool b );
    void setShuffled( bool b );
    void setPlaylist( bool b );
    void setPaused( bool b );
    void setPlaying( bool b );
    void setPosition( long p );
    void updatePosition( long p );
    void setLength( long l );
    void setView( char v );

    void setPrev();
    void setNext();
    void setList();
    void setVideo();
    void setAudio();

    void toggleFullscreen();
    void toggleScaled();
    void toggleLooping();
    void toggleShuffled();
    void togglePlaylist();
    void togglePaused();
    void togglePlaying();

signals:
    void fullscreenToggled( bool );
    void scaledToggled( bool );
    void loopingToggled( bool );
    void shuffledToggled( bool );
    void playlistToggled( bool );
    void pausedToggled( bool );
    void playingToggled( bool );
    void positionChanged( long ); // When the slider is moved
    void positionUpdated( long ); // When the media file progresses
    void lengthChanged( long );
    void viewChanged( char );

    void prev();
    void next();

private:
    bool isFullscreen;
    bool isScaled;
    bool isLooping;
    bool isShuffled;
    bool usePlaylist;
    bool isPaused;
    bool isPlaying;
    long curPosition;
    long curLength;
    char curView;

    void readConfig( Config& cfg );
    void writeConfig( Config& cfg ) const;


};


#endif // MEDIA_PLAYER_STATE_H

