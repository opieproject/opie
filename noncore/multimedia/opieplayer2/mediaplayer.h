
#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

#include <qmainwindow.h>
#include <qframe.h>
#include <qpe/qlibrary.h>
#include <qpe/mediaplayerplugininterface.h>

#include "xinecontrol.h"

class DocLnk;


class MediaPlayer : public QObject {
    Q_OBJECT
public:
    MediaPlayer( QObject *parent, const char *name );
    ~MediaPlayer();

private slots:
    void setPlaying( bool );
    void pauseCheck( bool );
    void play();
    void next();
    void prev();
    void startIncreasingVolume();
    void startDecreasingVolume();
    void stopChangingVolume();
    void cleanUp();

protected:
    void timerEvent( QTimerEvent *e );
    void keyReleaseEvent( QKeyEvent *e);
    void doBlank();
    void doUnblank();
private:
    int     volumeDirection;
    const DocLnk  *currentFile;
    XineControl *xineControl;

};


#endif // MEDIA_PLAYER_H

