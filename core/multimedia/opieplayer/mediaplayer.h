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
#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

#include <qmainwindow.h>
#include <qframe.h>
#include <qpe/qlibrary.h>
#include <qpe/mediaplayerplugininterface.h>


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

protected:
    void timerEvent( QTimerEvent *e );

private:
    int			volumeDirection;
    const DocLnk	*currentFile;
};


#endif // MEDIA_PLAYER_H

