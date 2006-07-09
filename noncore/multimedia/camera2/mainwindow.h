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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "camerabase.h"
#include <qvaluelist.h>
#include <qmainwindow.h>
#include <qpe/applnk.h>

class LocationCombo;
class QAction;
class QTimer;
class CameraSettings;


class CameraMainWindow : public QMainWindow
{
    Q_OBJECT
public:
 static QString appName() { return QString::fromLatin1("camera2"); }

    CameraMainWindow( QWidget *parent=0, const char *name=0, WFlags fl=0 );
    ~CameraMainWindow();

public slots:
    void takePhoto();
    void toggleVideo();
    void selectThumb(int i);
    void thumbClicked(int i);

private slots:
    void viewPictures();
    void viewVideos();
    void doSettings();
    void appMessage(const QCString& msg, const QByteArray& data);
    void editThumb();
    void delThumb();
    void moveToContact();
    void takePhotoNow();
    void sendFile();
    void linkChanged(const QString&);

private:
    bool event(QEvent* e);
    void updateActions();
    void resizeEvent(QResizeEvent*);

    bool eventFilter(QObject*, QEvent*);
    QString nextFileName();
    void loadThumbs();
    void pushThumb(const DocLnk& f, const QImage& img);
    static const int nthumb = 5;
    QToolButton* thumb[nthumb];
    DocLnk picturefile[nthumb];
    int cur_thumb;
    void delThumb(int th);

    // Settings
    void confirmSettings();
    CameraSettings *settings;
    QString storagepath;
    int thumbw;
    int thumbh;
    int psize;
    int vsize;
    int pquality;
    int vquality;
    int vframerate;

    // Snap
    QCString snap_ch;
    QString snap_id;
    int snap_maxw, snap_maxh;
    bool inSnapMode() const;
    void setSnapMode(bool);

    CameraBase *camera;

    int namehint;
    QAction *a_pview, *a_vview, *a_settings;
    QAction *a_th_edit, *a_th_del, *a_th_add;
    QAction *a_send;
    QValueList<QSize> photo_size;
    QValueList<QSize> video_size;

    QTimer *refocusTimer;
    QString picfile;

    bool recording;
    void stopVideo();
    void startVideo();

    void preview();

    int camcat;
};

#endif

