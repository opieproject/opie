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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qdatetime.h>
#include <qimage.h>
#include <qpixmap.h>

class QIconSet;
class QToolButton;
class QLabel;
class MainWindowBase;
class QCopChannel;
class PreviewWidget;

class CameraMainWindow: public QMainWindow
{
  Q_OBJECT

  public:
    CameraMainWindow( QWidget * parent = 0, const char * name = "mainwindow", WFlags f = 0 );
    virtual ~CameraMainWindow();

  public slots:
    void changeZoom( int );
    void systemMessage( const QCString&, const QByteArray& );

  protected:

  private:
    PreviewWidget* preview;
    int _rotation;
    QCopChannel* _sysChannel;
};

#endif
