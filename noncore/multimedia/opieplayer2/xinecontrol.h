
#ifndef XINECONTROL_H
#define XINECONTROL_H

#include "lib.h"
#include <qobject.h>

class XineControl : public QObject  {
    Q_OBJECT
public:
    XineControl( QObject *parent, const char *name );
    ~XineControl();

private slots:
    void play( const QString& fileName );
    void stop();
    void pause( bool );

    int currentTime();

    // get length of media file and set it
    void length();

    int position();

private:
    XINE::Lib *libXine;
    int m_length;
    int m_currentTime;
    int m_position;

signals:
    void positionChanged( int position  );

};


#endif
