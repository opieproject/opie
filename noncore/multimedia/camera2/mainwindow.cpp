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

#define QTOPIA_INTERNAL_FILEOPERATIONS
#include "mainwindow.h"
#include "videocaptureview.h"
#include "camerasettings.h"
#include "thumbbutton.h"

#include "image.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
using namespace Opie::Core;

#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/locationcombo.h>
//#include <qtopia/contextmenu.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
//#include <qtopia/services.h>
#include <qpe/categories.h>
#include <qpe/qpetoolbar.h>
#include <qpe/global.h>

#include <qaction.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qsignalmapper.h>
#include <qdir.h>
#include <qimage.h>
#include <qslider.h>
#include <qtimer.h>
#include <qmessagebox.h>

#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

static const int thmarg = 2;
static const bool video_supported = FALSE;

CameraMainWindow::CameraMainWindow(QWidget * parent, const char *name, WFlags f)
:QMainWindow(parent, name, f)
{
    setCaption(tr("Camera"));

    picfile = Global::tempDir() + "image.jpg";
    camera = new CameraBase(this);
    setCentralWidget(camera);

    connect(qApp, SIGNAL(appMessage(const QCString &, const QByteArray &)),
            this, SLOT(appMessage(const QCString &, const QByteArray &)));
    connect(qApp, SIGNAL(linkChanged(const QString &)), this, SLOT(linkChanged(const QString &)));

    Categories cats;
    cats.load(categoryFileName());
    camcat = cats.id("Document View", "_Camera");       // No tr
    if (!camcat) {
        camcat = cats.addCategory("Document View", "_Camera");  // No tr
        cats.save(categoryFileName());
    }

    camera->photo->setFocus();

    connect(camera->photo, SIGNAL(clicked()), this, SLOT(takePhoto()));
    connect(camera->video, SIGNAL(clicked()), this, SLOT(toggleVideo()));

    refocusTimer = new QTimer(this);
    connect(refocusTimer, SIGNAL(timeout()), this, SLOT(takePhotoNow()));

    thumb[0] = camera->thumb1;
    thumb[1] = camera->thumb2;
    thumb[2] = camera->thumb3;
    thumb[3] = camera->thumb4;
    thumb[4] = camera->thumb5;
    cur_thumb = -1;

    QSignalMapper *sm = new QSignalMapper(this);
    for (int i = 0; i < nthumb; i++) {
        sm->setMapping(thumb[i], i);
        connect(thumb[i], SIGNAL(clicked()), sm, SLOT(map()));
        thumb[i]->installEventFilter(this);
    }
    connect(sm, SIGNAL(mapped(int)), this, SLOT(thumbClicked(int)));

#warning fixme
//    AppLnk picture_viewer = Service::appLnk("PhotoEdit");
//    AppLnk video_viewer = Service::appLnk("Open/video/mpeg");

    a_pview = new QAction(tr("%1...", "app name").arg("" /*picture_viewer.name() */ ),
                          Resource::loadPixmap(NULL /*picture_viewer.icon() */ ), QString::null, 0,
                          this, 0);
    connect(a_pview, SIGNAL(activated()), this, SLOT(viewPictures()));
    a_vview = new QAction(tr("%1...", "app name").arg("" /*video_viewer.name() */ ),
                          Resource::loadIconSet(NULL /*video_viewer.icon() */ ), QString::null, 0,
                          this, 0);
    connect(a_vview, SIGNAL(activated()), this, SLOT(viewVideos()));

    a_settings =
        new QAction(tr("Settings..."),  Opie::Core::OResource::loadPixmap("camera2/settings", Opie::Core::OResource::SmallIcon), 
                    QString::null, 0, this, 0);
    connect(a_settings, SIGNAL(activated()), this, SLOT(doSettings()));

    a_th_edit = new QAction(tr("Edit"),  Opie::Core::OResource::loadPixmap("edit", Opie::Core::OResource::SmallIcon), 
                            QString::null, 0, this, 0);
    connect(a_th_edit, SIGNAL(activated()), this, SLOT(editThumb()));
    a_th_del = new QAction(tr("Delete"),  Opie::Core::OResource::loadPixmap("trash", Opie::Core::OResource::SmallIcon), 
                           QString::null, 0, this, 0);
    connect(a_th_del, SIGNAL(activated()), this, SLOT(delThumb()));
    a_th_add =
        new QAction(tr("Move to Contact..."), Opie::Core::OResource::loadPixmap("addressbook/generic-contact", Opie::Core::OResource::SmallIcon),
                    QString::null, 0, this, 0);
    connect(a_th_add, SIGNAL(activated()), this, SLOT(moveToContact()));
    a_send =
        new QAction(tr("Beam to Contact..."),  Opie::Core::OResource::loadPixmap("beam", Opie::Core::OResource::SmallIcon), 
                    QString::null, 0, this, 0);
    connect(a_send, SIGNAL(activated()), this, SLOT(sendFile()));

    
#ifndef QTOPIA_PHONE
    // Create Toolbars
    QPEToolBar *bar = new QPEToolBar(this);
    bar->setHorizontalStretchable(TRUE);
    setToolBarsMovable(FALSE);

    a_pview->addTo(bar);
    if (video_supported)
        a_vview->addTo(bar);
    a_th_edit->addTo(bar);
    a_th_del->addTo(bar);
    a_th_add->addTo(bar);
    a_send->addTo(bar);
    a_settings->addTo(bar);
#else
    ContextMenu *contextMenu = new ContextMenu(this);
    a_pview->addTo(contextMenu);
    if (video_supported)
        a_vview->addTo(contextMenu);
    a_th_edit->addTo(contextMenu);
    a_th_del->addTo(contextMenu);
    a_th_add->addTo(contextMenu);
    a_send->addTo(contextMenu);
    a_settings->addTo(contextMenu);
#endif

    installEventFilter(camera->photo);
    installEventFilter(camera->video);
    camera->photo->installEventFilter(this);
    camera->video->installEventFilter(this);

    // Load the allowable sizes from the camera hardware.
    photo_size = camera->videocaptureview->photoSizes();
    video_size = camera->videocaptureview->videoSizes();

    settings = new CameraSettings(this, 0, TRUE);
    // load settings from config
    Config cfg("Camera");
    cfg.setGroup("General");
    QString l = cfg.readEntry("location");

#warning fixme
    /*if ( !l.isEmpty() )
       settings->location->setLocation(l);
     */

    storagepath = settings->location->documentPath();
    cfg.setGroup("Photo");
    int w;
    w = cfg.readNumEntry("width", camera->videocaptureview->recommendedPhotoSize().width());
    for (psize = 0; psize < (int) photo_size.count() - 1 && photo_size[psize].width() > w;)
        psize++;
    pquality = cfg.readNumEntry("quality", settings->photo_quality->value());
    cfg.setGroup("Video");
    w = cfg.readNumEntry("width", camera->videocaptureview->recommendedVideoSize().width());
    for (vsize = 0; vsize < (int) video_size.count() - 1 && video_size[vsize].width() > w;)
        vsize++;
    vquality = cfg.readNumEntry("quality", settings->video_quality->value());
    vframerate = cfg.readNumEntry("framerate", settings->video_framerate->value());

    for (int i = 0; i < (int) photo_size.count(); i++) {
        settings->photo_size->insertItem(tr("%1 x %2").arg(photo_size[i].width()).
                                         arg(photo_size[i].height()));
    }
    for (int i = 0; i < (int) video_size.count(); i++) {
        settings->video_size->insertItem(tr("%1 x %2").arg(video_size[i].width()).
                                         arg(video_size[i].height()));
    }

    namehint = 0;
    recording = FALSE;

    preview();

    if (!video_supported) {
        // Room for longer text
        camera->photo->setText(tr("Take Photo"));
        camera->video->setEnabled(FALSE);
        camera->video->hide();
    }
}

CameraMainWindow::~CameraMainWindow()
{
}

void CameraMainWindow::resizeEvent(QResizeEvent *)
{
    thumbw = width() / 5 - 4;
    thumbh = thumbw * 3 / 4;
    thumb[0]->setFixedHeight(thumbh + thmarg * 2);
    loadThumbs();
}

bool CameraMainWindow::event(QEvent * e)
{
    if (e->type() == QEvent::WindowActivate) {
        if (cur_thumb < 0)
            camera->videocaptureview->setLive();
    }
    else if (e->type() == QEvent::WindowDeactivate) {
        camera->videocaptureview->setLive(-1);
    }
    return QMainWindow::event(e);
}

bool CameraMainWindow::eventFilter(QObject * o, QEvent * e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = (QKeyEvent *) e;

#if QTOPIA_PHONE
        if (!ke->isAutoRepeat()) {
            if (ke->key() == Key_1) {
                takePhoto();
                return TRUE;
            }
            else if (ke->key() == Key_2) {
                toggleVideo();
                return TRUE;
            }
        }
#endif
        if (ke->key() == Key_Up) {
            camera->photo->setFocus();
            return TRUE;
        }
        else if (ke->key() == Key_Down) {
            thumb[0]->setFocus();
            return TRUE;
        }
        else if (ke->key() == Key_Left) {
            if (o == camera->video) {
                camera->photo->setFocus();
                return TRUE;
            }
            else {
                if (o == thumb[0])
                    return TRUE;
                for (int i = 1; i < nthumb; ++i) {
                    if (o == thumb[i]) {
                        thumb[i - 1]->setFocus();
                        return TRUE;
                    }
                }
            }
        }
        else if (ke->key() == Key_Right) {
            if (o == camera->photo) {
                camera->video->setFocus();
                return TRUE;
            }
            else {
                if (o == thumb[nthumb - 1])
                    return TRUE;
                for (int i = 0; i < nthumb - 1; ++i) {
                    if (o == thumb[i]) {
                        thumb[i + 1]->setFocus();
                        return TRUE;
                    }
                }
            }
        }
    }
    else if (e->type() == QEvent::FocusIn) {
        if (o == camera->photo)
            camera->photo->setText(tr("Take Photo"));
        updateActions();
    }
    else if (e->type() == QEvent::FocusOut) {
        if (o == camera->photo)
            camera->photo->setText(tr("Activate Camera"));
    }
    return QWidget::eventFilter(o, e);
}

void CameraMainWindow::updateActions()
{
    bool p = FALSE, v = FALSE;
    QWidget *foc = focusWidget();
    if (foc == camera->photo) {
        p = TRUE;
        v = FALSE;
    }
    else if (foc == camera->video) {
        v = TRUE;
        p = FALSE;
    }
    a_pview->setEnabled(p);
    a_vview->setEnabled(v);
    a_settings->setEnabled(p || v);
    bool th = !p && !v;
    if (th) {
        int i;
        for (i = 0; i < nthumb; i++) {
            if (thumb[i] == foc) {
                selectThumb(i);
                break;
            }
        }
        if (i == nthumb || !thumb[i]->pixmap())
            th = FALSE;
    }
    else {
        selectThumb(-1);
    }
}

void CameraMainWindow::viewPictures()
{
#warning fixme
//     ServiceRequest req("PhotoEdit","showCategory(int)");
//     req << camcat;
//     req.send();
}

void CameraMainWindow::viewVideos()
{
#warning fixme
//    Service::appLnk("Open/video/mpeg").execute();
}

void CameraMainWindow::doSettings()
{
    bool v = video_supported;
#ifdef QTOPIA_PHONE
    bool p;
    p = a_pview->isEnabled();
    v = v && a_vview->isEnabled();
    if (p)
        settings->photo->show();
    else
        settings->photo->hide();
#endif
    if (v)
        settings->video->show();
    else
        settings->video->hide();
    settings->photo_size->setCurrentItem(psize);
    settings->video_size->setCurrentItem(vsize);
    settings->photo_quality->setValue(pquality);
    settings->video_quality->setValue(vquality);
    settings->video_framerate->setValue(vframerate);
    settings->video_quality_n->setFixedWidth(fontMetrics().width("100"));
    settings->photo_quality_n->setFixedWidth(fontMetrics().width("100"));

    if (QPEApplication::execDialog(settings)) {
        confirmSettings();
    }
    else {
#warning fixme
//        settings->location->setLocation(storagepath);
    }
}

void CameraMainWindow::confirmSettings()
{
    storagepath = settings->location->documentPath();
    psize = settings->photo_size->currentItem();
    vsize = settings->video_size->currentItem();
    pquality = settings->photo_quality->value();
    vquality = settings->video_quality->value();
    vframerate = settings->video_framerate->value();

    // save settings
    Config cfg("Camera");
    cfg.setGroup("General");
    cfg.writeEntry("location", storagepath);
    cfg.setGroup("Photo");
    cfg.writeEntry("width", photo_size[psize].width());
    cfg.writeEntry("quality", pquality);
    cfg.setGroup("Video");
    cfg.writeEntry("width", video_size[vsize].width());
    cfg.writeEntry("quality", vquality);
    cfg.writeEntry("framerate", vframerate);

    loadThumbs();

    preview();
}

static int cmpDocLnk(const void *a, const void *b)
{
    const DocLnk *la = *(const DocLnk **) a;
    const DocLnk *lb = *(const DocLnk **) b;
    QFileInfo fa(la->linkFileKnown()? la->linkFile() : la->file());
    QFileInfo fb(lb->linkFileKnown()? lb->linkFile() : lb->file());
    return fa.lastModified().secsTo(fb.lastModified());
}

void CameraMainWindow::loadThumbs()
{
    if (storagepath.isEmpty()) {
        updateActions();
        return;
    }

    DocLnkSet set(storagepath, "image/jpeg");
    const QList < DocLnk > &l = set.children();

    DocLnk *lnk;
    DocLnk **array = new DocLnk *[l.count()];
    int n = 0;
    for (QListIterator < DocLnk > it(l); (lnk = it.current()); ++it) {
        if (lnk->categories().contains(camcat))
            array[n++] = lnk;
    }
    qsort(array, n, sizeof(array[0]), cmpDocLnk);
    for (int i = 0; i < nthumb; i++) {
        QPixmap pm;
        if (i < n) {
            picturefile[i] = *array[i];
            QImage img = Image::loadScaled(picturefile[i].file(), thumbw, thumbh);
            pm.convertFromImage(img);
        }
        if (pm.isNull()) {
            thumb[i]->setText("");
        }
        else {
            thumb[i]->setPixmap(pm);
        }
        thumb[i]->setEnabled(!pm.isNull());
    }
    if (cur_thumb >= 0)
        selectThumb(cur_thumb);

    if (!camera->videocaptureview->available()) {
        camera->photo->setEnabled(FALSE);
        camera->video->setEnabled(FALSE);
        if (!n) {
            thumb[0]->setEnabled(FALSE);
        }
        else {
            thumb[0]->setFocus();
            thumb[0]->setEnabled(TRUE);
        }
    }

    updateActions();
}

void CameraMainWindow::delThumb(int th)
{
    switch (QMessageBox::warning(0, tr("Confirmation"),
                                 tr("<qt>Delete '%1'?</qt>",
                                    "%1 = file name").arg(picturefile[th].name()), QMessageBox::Yes,
                                 QMessageBox::No)) {
    case QMessageBox::Yes:
        picturefile[th].removeFiles();

        // Rhys Hack - if we have just deleted the last image and there
        // is no camera connected, then exit the application.  This
        // avoids a focus problem where it is impossible to exit with
        // the back button due to the focus being in a stupid place.
        loadThumbs();
        if (!camera->videocaptureview->available() && !(thumb[0]->isEnabled())) {
            close();
        }
        break;
    default:
        //nothing
        break;
    }
}

void CameraMainWindow::pushThumb(const DocLnk & f, const QImage & img)
{
    for (int i = nthumb; --i;) {
        bool en = thumb[i - 1]->isEnabled();
        thumb[i]->setEnabled(en);
        picturefile[i] = picturefile[i - 1];
        const QPixmap *pm = thumb[i - 1]->pixmap();
        if (en && pm) {
            thumb[i]->setPixmap(*pm);
        }
        else {
            thumb[i]->setText("");
        }
    }
    QPixmap pm;
    QSize sz = Image::aspectScaleSize(img.width(), img.height(), thumbw, thumbh);
    QImage simg = img.smoothScale(sz.width(), sz.height());
    pm.convertFromImage(simg);
    thumb[0]->setPixmap(pm);
    thumb[0]->setEnabled(TRUE);
    picturefile[0] = f;
}

void CameraMainWindow::takePhoto()
{
    if (camera->photo != focusWidget()) {
        camera->photo->setFocus();
        return;
    }
    QSize size = photo_size[psize];
    if (size == camera->videocaptureview->captureSize() ||
        camera->videocaptureview->refocusDelay() == 0) {

        // We can grab the current image immediately.
        takePhotoNow();

    }
    else {

        // Change the camera size and then wait for the camera to refocus.
        camera->videocaptureview->setCaptureSize(size);
        refocusTimer->start(camera->videocaptureview->refocusDelay(), TRUE);

    }
}

void CameraMainWindow::takePhotoNow()
{
    QImage img = camera->videocaptureview->image();

    if (inSnapMode()) {
        QCopEnvelope e(snap_ch, "valueSupplied(QString,QImage)");
        QSize s = Image::aspectScaleSize(img.width(), img.height(), snap_maxw, snap_maxh);
        e << snap_id << img.smoothScale(s.width(), s.height());
        setSnapMode(FALSE);
        close();
    }
    else {
        DocLnk f;
        //f.setLocation(storagepath);
        f.setType("image/jpeg");
        QDateTime dt = QDateTime::currentDateTime();
        QString date = dt.toString();
        f.setName(tr("Photo, " + date));

        QArray < int >c(1);
        c[0] = camcat;
        f.setCategories(c);
        img.save(f.file(), "JPEG", pquality);   // Save the image in its original size.
        f.writeLink();
        pushThumb(f, img);
    }

    preview();
}

bool CameraMainWindow::inSnapMode() const
{
    return !snap_ch.isEmpty();
}

void CameraMainWindow::setSnapMode(bool y)
{
    if (y) {
        camera->thumbs->hide();
        camera->video->hide();
    }
    else {
        camera->thumbs->show();
        if (video_supported)
            camera->video->show();
        else
            camera->video->hide();
        snap_ch = QString::null;
    }
}

void CameraMainWindow::toggleVideo()
{
    if (recording)
        stopVideo();
    else
        startVideo();
    recording = !recording;
    camera->video->setText(recording ? tr("Stop") : tr("Video"));
    camera->photo->setEnabled(!recording);
    for (int i = 0; i < nthumb; i++)
        thumb[i]->setEnabled(!recording && thumb[i]->pixmap());
}

void CameraMainWindow::startVideo()
{
    // XXX eg. MJPG
}
void CameraMainWindow::stopVideo()
{
}

void CameraMainWindow::thumbClicked(int i)
{
    selectThumb(i);
}

void CameraMainWindow::editThumb()
{
    picturefile[cur_thumb].execute();
}

void CameraMainWindow::selectThumb(int i)
{
    cur_thumb = i;
    if (i >= 0) {
        QImage img(picturefile[i].file());
        camera->videocaptureview->setStill(img);
        thumb[i]->setFocus();
    }
    else {
        camera->videocaptureview->setLive();
    }
    a_th_edit->setEnabled(i >= 0);
    a_th_del->setEnabled(i >= 0);
    a_th_add->setEnabled(i >= 0);
    a_send->setEnabled(i >= 0);
}

void CameraMainWindow::moveToContact()
{
    if (cur_thumb >= 0) {
#warning fixme
//        QCopEnvelope e(Service::channel("Contacts"),"setContactImage(QImage)");
//        QImage img;
//        img.load(picturefile[cur_thumb].file());
//        e << img;
    }
}

void CameraMainWindow::delThumb()
{
    if (cur_thumb >= 0) {
        int d = cur_thumb;
        if (cur_thumb > 0)
            selectThumb(cur_thumb - 1);
        delThumb(d);
    }
}

void CameraMainWindow::linkChanged(const QString &)
{
    loadThumbs();
}

void CameraMainWindow::appMessage(const QCString & msg, const QByteArray & data)
{
    if (msg == "getImage(QCString,QString,int,int,QImage)") {
        QDataStream ds(data, IO_ReadOnly);
        ds >> snap_ch >> snap_id >> snap_maxw >> snap_maxh;
        setSnapMode(TRUE);
        QPEApplication::setKeepRunning();
    }
}

void CameraMainWindow::preview()
{
    if (camera->videocaptureview->refocusDelay() > 200) {
        camera->videocaptureview->setCaptureSize(photo_size[psize]);
    }
    else {
        camera->videocaptureview->setCaptureSize(camera->videocaptureview->
                                                 recommendedPreviewSize());
    }
}

void CameraMainWindow::sendFile()
{
    if (cur_thumb >= 0) {
        //copy file
        QFile input(picturefile[cur_thumb].file());
        if (!input.open(IO_ReadOnly)) {
            return;             //error
        }
        QFile output(picfile);
        if (!output.open(IO_WriteOnly)) {
            return;
        }

        const int BUFFER_SIZE = 1024;
        Q_INT8 buffer[BUFFER_SIZE];

        QDataStream srcStr(&input);
        QDataStream destStr(&output);

        while (!srcStr.atEnd()) {
            int i = 0;
            while (!srcStr.atEnd() && i < BUFFER_SIZE) {
                srcStr >> buffer[i];
                i++;
            }
            for (int k = 0; k < i; k++) {
                destStr << buffer[k];
            }
        }

#warning fixme
//        QCopEnvelope e(Service::channel("Email"),"writeMessage(QString,QString,QStringList,QStringList)");
//        e << QString() << QString() << QStringList() << QStringList( QString( picfile ) );
    }
}

void ThumbButton::drawButtonLabel(QPainter * p)
{
    const QPixmap *pm = pixmap();
    if (pm) {
        QSize s = (size() - pm->size()) / 2;
        p->drawPixmap(s.width(), s.height(), *pm);
    }
}

ThumbButton::ThumbButton(QWidget * parent, const char *name):QToolButton(parent, name)
{
}

