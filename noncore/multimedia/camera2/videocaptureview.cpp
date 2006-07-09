/**********************************************************************
** Copyright (C) 2000-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
**
** A copy of the GNU GPL license version 2 is included in this package as
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "videocaptureview.h"
#include <qimage.h>
#include <qpainter.h>
#ifdef Q_WS_QWS
#include <qgfx_qws.h>
#include <qdirectpainter_qws.h>
#endif

#ifdef __linux__
#define	HAVE_VIDEO4LINUX 1
#endif

#ifdef HAVE_VIDEO4LINUX

#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#endif /* HAVE_VIDEO4LINUX */

class VideoCapture {
  public:
    VideoCapture();
    ~VideoCapture();

    bool hasCamera() const;
    void getCameraImage(QImage & img, bool copy = FALSE);

      QValueList < QSize > photoSizes() const;
      QValueList < QSize > videoSizes() const;

    QSize recommendedPhotoSize() const;
    QSize recommendedVideoSize() const;
    QSize recommendedPreviewSize() const;

    QSize captureSize() const;
    void setCaptureSize(QSize size);

    uint refocusDelay() const;
    int minimumFramePeriod() const;

  private:
#ifdef HAVE_VIDEO4LINUX
    int fd;
    int width, height;
    struct video_capability caps;
    struct video_mbuf mbuf;
    unsigned char *frames;
    int currentFrame;

    void setupCamera(QSize size);
    void shutdown();
#endif
};

#ifdef HAVE_VIDEO4LINUX

#define	VIDEO_DEVICE	    "/dev/video"

bool VideoCapture::hasCamera() const 
{
    return (fd != -1);
}

QSize VideoCapture::captureSize() const
{
    return QSize(width, height);
}

uint VideoCapture::refocusDelay() const 
{
    return 250;
}

int VideoCapture::minimumFramePeriod() const 
{
    return 40;                  // milliseconds
}

VideoCapture::VideoCapture()
{
    setupCamera(QSize(0, 0));
}

VideoCapture::~VideoCapture()
{
    shutdown();
}

void VideoCapture::setupCamera(QSize size)
{
    qWarning(" VideoCapture::setupCamera");
    // Clear important variables.
    frames = 0;
    currentFrame = 0;
    width = 640;
    height = 480;
    caps.minwidth = width;
    caps.minheight = height;
    caps.maxwidth = width;
    caps.maxheight = height;

    // Open the video device.
    fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd == -1) {
        qWarning("%s: %s", VIDEO_DEVICE, strerror(errno));
        return;
    }

    // Get the device's current capabilities.
    memset(&caps, 0, sizeof(caps));
    if (ioctl(fd, VIDIOCGCAP, &caps) < 0) {
        qWarning("%s: could not retrieve the video capabilities", VIDEO_DEVICE);
        close(fd);
        fd = -1;
        return;
    }

    // Change the channel to the first-connected camera, skipping TV inputs.
    // If there are multiple cameras, this may need to be modified.
    int chan;
    struct video_channel chanInfo;
    qWarning("available video capture inputs:");
    for (chan = 0; chan < caps.channels; ++chan) {
        chanInfo.channel = chan;
        if (ioctl(fd, VIDIOCGCHAN, &chanInfo) >= 0) {
            if (chanInfo.type == VIDEO_TYPE_CAMERA)
                qWarning("    %s (camera)", chanInfo.name);
            else if (chanInfo.type == VIDEO_TYPE_TV)
                qWarning("    %s (tv)", chanInfo.name);
            else
                qWarning("    %s (unknown)", chanInfo.name);
        }
    }
    for (chan = 0; chan < caps.channels; ++chan) {
        chanInfo.channel = chan;
        if (ioctl(fd, VIDIOCGCHAN, &chanInfo) >= 0) {
            if (chanInfo.type == VIDEO_TYPE_CAMERA) {
                qWarning("selecting camera on input %s", chanInfo.name);
                if (ioctl(fd, VIDIOCSCHAN, &chan) < 0) {
                    qWarning("%s: could not set the channel", VIDEO_DEVICE);
                }
                break;
            }
        }
    }

    // Set the desired picture mode to RGB32.
    struct video_picture pict;
    memset(&pict, 0, sizeof(pict));
    ioctl(fd, VIDIOCGPICT, &pict);
    pict.palette = VIDEO_PALETTE_RGB32;
    if (ioctl(fd, VIDIOCSPICT, &pict) < 0) {
        qWarning("%s: could not set the picture mode", VIDEO_DEVICE);
        close(fd);
        fd = -1;
        return;
    }

    // Determine the capture size to use.  Zero indicates "preview mode".
    if (size.width() == 0) {
        size = QSize(caps.minwidth, caps.minheight);
    }

    // Get the current capture window.
    struct video_window wind;
    memset(&wind, 0, sizeof(wind));
    ioctl(fd, VIDIOCGWIN, &wind);

    // Adjust the capture size to match the camera's aspect ratio.
    if (caps.maxwidth > 0 && caps.maxheight > 0) {
        if (size.width() > size.height()) {
            size = QSize(size.height() * caps.maxwidth / caps.maxheight, size.height());
        }
        else {
            size = QSize(size.width(), size.width() * caps.maxheight / caps.maxwidth);
        }
    }

    // Set the new capture window.
    wind.x = 0;
    wind.y = 0;
    wind.width = size.width();
    wind.height = size.height();
    if (ioctl(fd, VIDIOCSWIN, &wind) < 0) {
        qWarning("%s: could not set the capture window", VIDEO_DEVICE);
    }

    // Re-read the capture window, to see what it was adjusted to.
    ioctl(fd, VIDIOCGWIN, &wind);
    width = wind.width;
    height = wind.height;

    // Enable mmap-based access to the camera.
    memset(&mbuf, 0, sizeof(mbuf));
    if (ioctl(fd, VIDIOCGMBUF, &mbuf) < 0) {
        qWarning("%s: mmap-based camera access is not available", VIDEO_DEVICE);
        close(fd);
        fd = -1;
        return;
    }

    // Mmap the designated memory region.
    frames = (unsigned char *) mmap(0, mbuf.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!frames || frames == (unsigned char *) (long) (-1)) {
        qWarning("%s: could not mmap the device", VIDEO_DEVICE);
        close(fd);
        fd = -1;
        return;
    }

    // Start capturing of the first frame.
    struct video_mmap capture;
    currentFrame = 0;
    capture.frame = currentFrame;
    capture.width = width;
    capture.height = height;
    capture.format = VIDEO_PALETTE_RGB32;
    ioctl(fd, VIDIOCMCAPTURE, &capture);
}

void VideoCapture::shutdown()
{
    if (frames != 0) {
        munmap(frames, mbuf.size);
        frames = 0;
    }
    if (fd != -1) {
        int flag = 0;
        ioctl(fd, VIDIOCSYNC, 0);
        ioctl(fd, VIDIOCCAPTURE, &flag);
        close(fd);
        fd = -1;
    }
}

void VideoCapture::getCameraImage(QImage & img, bool copy)
{
    if (fd == -1) {
        if (img.isNull()) {
            img.create(width, height, 32);
        }
        return;
    }

    // Start capturing the next frame (we alternate between 0 and 1).
    int frame = currentFrame;
    struct video_mmap capture;
    if (mbuf.frames > 1) {
        currentFrame = !currentFrame;
        capture.frame = currentFrame;
        capture.width = width;
        capture.height = height;
        capture.format = VIDEO_PALETTE_RGB32;
        ioctl(fd, VIDIOCMCAPTURE, &capture);
    }

    // Wait for the current frame to complete.
    ioctl(fd, VIDIOCSYNC, &frame);

    // Create an image that refers directly to the kernel's
    // frame buffer, to avoid having to copy the data.
    if (!copy) {
        img = QImage(frames + mbuf.offsets[frame], width, height, 32, 0, 0, QImage::IgnoreEndian);
    }
    else {
        img.create(width, height, 32);
        memcpy(img.bits(), frames + mbuf.offsets[frame], width * height * 4);
    }

    // Queue up another frame if the device only supports one at a time.
    if (mbuf.frames <= 1) {
        capture.frame = currentFrame;
        capture.width = width;
        capture.height = height;
        capture.format = VIDEO_PALETTE_RGB32;
        ioctl(fd, VIDIOCMCAPTURE, &capture);
    }
}

QValueList < QSize > VideoCapture::photoSizes() const
{
    QValueList < QSize > list;
    list.append(QSize(caps.maxwidth, caps.maxheight));
    if (caps.maxwidth != caps.minwidth || caps.maxheight != caps.minheight)
        list.append(QSize(caps.minwidth, caps.minheight));
    return list;
}

QValueList < QSize > VideoCapture::videoSizes() const
{
    // We use the same sizes for both.
    return photoSizes();
}

QSize VideoCapture::recommendedPhotoSize() const
{
    return QSize(caps.maxwidth, caps.maxheight);
}

QSize VideoCapture::recommendedVideoSize() const
{
    return QSize(caps.minwidth, caps.minheight);
}

QSize VideoCapture::recommendedPreviewSize() const
{
    return QSize(caps.minwidth, caps.minheight);
}

void VideoCapture::setCaptureSize(QSize size)
{
    if (size.width() != width || size.height() != height) {
        shutdown();
        setupCamera(size);
    }
}

#else /* !HAVE_VIDEO4LINUX */

// Dummy implementation for systems without video.

VideoCapture::VideoCapture()
{
}

VideoCapture::~VideoCapture()
{
}

bool VideoCapture::hasCamera() const 
{
    return TRUE;
}

QSize VideoCapture::captureSize() const 
{
    return QSize(640, 480);
}

uint VideoCapture::refocusDelay() const 
{
    return 0;
}

int VideoCapture::minimumFramePeriod() const 
{
    return 100;
}

static unsigned int nextrand()
{
#define A 16807
#define M 2147483647
#define Q 127773
#define R 2836
    static unsigned int rnd = 1;
    unsigned long hi = rnd / Q;
    unsigned long lo = rnd % Q;
    unsigned long test = A * lo - R * hi;
    if (test > 0)
        rnd = test;
    else
        rnd = test + M;
    return rnd;
}

void VideoCapture::getCameraImage(QImage & img, bool)
{
    // Just generate something dynamic (rectangles)
    static QImage cimg;
    int x, y, w, h;
    if (cimg.isNull()) {
        x = y = 0;
        w = 640;
        h = 480;
        cimg.create(w, h, 32);
    }
    else {
        w = nextrand() % (cimg.width() - 10) + 10;
        h = nextrand() % (cimg.height() - 10) + 10;
        x = nextrand() % (cimg.width() - w);
        y = nextrand() % (cimg.height() - h);
    }
    QRgb c = qRgb(nextrand() % 255, nextrand() % 255, nextrand() % 255);
    for (int j = 0; j < h; j++) {
        QRgb *l = (QRgb *) cimg.scanLine(y + j) + x;
        for (int i = 0; i < w; i++)
            l[i] = c;
    }
    img = cimg;
}

QValueList < QSize > VideoCapture::photoSizes() constconst
{
    QValueList < QSize > list;
    list.append(QSize(640, 480));
    list.append(QSize(320, 240));
    return list;
}

QValueList < QSize > VideoCapture::videoSizes() constconst
{
    QValueList < QSize > list;
    list.append(QSize(640, 480));
    list.append(QSize(320, 240));
    return list;
}

QSize VideoCapture::recommendedPhotoSize() const 
{
    return QSize(640, 480);
}

QSize VideoCapture::recommendedVideoSize() const 
{
    return QSize(320, 240);
}

QSize VideoCapture::recommendedPreviewSize() const 
{
    return QSize(320, 240);
}

void VideoCapture::setCaptureSize(QSize size)
{
}

#endif /* !HAVE_VIDEO4LINUX */

VideoCaptureView::VideoCaptureView(QWidget * parent, const char *name, WFlags fl):QWidget(parent,
                                                                                          name, fl)
{
    capture = new VideoCapture();
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sp);
    tid_update = 0;
    setLive();
}

VideoCaptureView::~VideoCaptureView()
{
    delete capture;
}

void VideoCaptureView::setLive(int period)
{
    if (tid_update)
        killTimer(tid_update);
    if (period == 0)
        tid_update = startTimer(capture->minimumFramePeriod());
    else if (period > 0)
        tid_update = startTimer(period);
    else
        tid_update = 0;
}

void VideoCaptureView::setStill(const QImage & i)
{
    setLive(-1);
    img = i;
    repaint(TRUE);
}

QValueList < QSize > VideoCaptureView::photoSizes() const
{
    return capture->photoSizes();
}

QValueList < QSize > VideoCaptureView::videoSizes() const
{
    return capture->videoSizes();
}

QSize VideoCaptureView::recommendedPhotoSize() const 
{
    return capture->recommendedPhotoSize();
}

QSize VideoCaptureView::recommendedVideoSize() const 
{
    return capture->recommendedVideoSize();
}

QSize VideoCaptureView::recommendedPreviewSize() const 
{
    return capture->recommendedPreviewSize();
}

QSize VideoCaptureView::captureSize() const 
{
    return capture->captureSize();
}

void VideoCaptureView::setCaptureSize(QSize size)
{
    capture->setCaptureSize(size);
}

uint VideoCaptureView::refocusDelay() const 
{
    return capture->refocusDelay();
}

bool VideoCaptureView::available() const 
{
    return capture->hasCamera();
}

void VideoCaptureView::paintEvent(QPaintEvent *)
{
    if (tid_update && !capture->hasCamera()) {
        QPainter p(this);
        p.drawText(rect(), AlignCenter, tr("No Camera"));
        return;
    }

    if (tid_update)
        capture->getCameraImage(img);
    int w = img.width();
    int h = img.height();

    if (!w || !h)
        return;

    if (width() * w > height() * h) {
        w = w * height() / h;
        h = height();
    }
    else {
        h = h * width() / w;
        w = width();
    }

    if (qt_screen->transformOrientation() == 0) {
        // Stretch and draw the image.
        QDirectPainter p(this);
        QGfx *gfx = p.internalGfx();
        if (gfx) {
            gfx->setSource(&img);
            gfx->setAlphaType(QGfx::IgnoreAlpha);
            gfx->stretchBlt((width() - w) / 2, (height() - h) / 2, w, h, img.width(), img.height());
        }
    }
    else {
        // This code is nowhere near efficient enough (hence the above).
        // TODO - handle rotations during direct painting.
        QImage scimg = img.smoothScale(w, h);
        QPainter p(this);
        p.drawImage((width() - w) / 2, (height() - h) / 2, scimg);
    }
}

void VideoCaptureView::timerEvent(QTimerEvent *)
{
    repaint(FALSE);
}

