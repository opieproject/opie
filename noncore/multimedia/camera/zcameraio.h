/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef ZCAMERAIO_H
#define ZCAMERAIO_H

#include <qobject.h>

class QImage;
class QTime;

class ZCameraIO : public QObject
{
  Q_OBJECT

  public:
    virtual ~ZCameraIO();

    enum ReadMode
    {
        IMAGE = 0, STATUS = 1,
        FASTER = 0, BETTER = 2,
        XNOFLIP = 0, XFLIP = 4,
        YNOFLIP = 0, YFLIP = 8
    };

    // low level interface

    bool setCaptureFrame( int w, int h, int zoom = 256, bool rot = true );
    bool setZoom( int zoom = 0 );
    void setReadMode( int = IMAGE | XFLIP | YFLIP );

    bool isShutterPressed(); // not const, because it calls clearShutterLatch
    bool isAvailable() const;
    bool isCapturing() const;
    bool isFinderReversed() const;

    bool snapshot( unsigned char* );
    bool snapshot( QImage* );

    // high level interface
    bool isOpen() const;
    static ZCameraIO* instance();
    void captureFrame( int w, int h, int zoom, QImage* image );

  protected:
    ZCameraIO();
    void clearShutterLatch();
    void init();
    bool read( char*, int );
    bool write( char*, int = 0 );

  signals:
    void shutterClicked();

  private:
    int _driver;
    char _status[4];
    static ZCameraIO* _instance;
    int _height;
    int _width;
    int _zoom;
    bool _rot;
    int _readlen;

    QTime* _timer;
};

#endif
