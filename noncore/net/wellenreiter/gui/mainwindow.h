/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
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

class Wellenreiter;
class WellenreiterConfigWindow;
class QIconSet;
class QToolButton;

class WellenreiterMainWindow: public QMainWindow
{
  Q_OBJECT

  public:
    WellenreiterMainWindow( QWidget * parent = 0, const char * name = "mainwindow", WFlags f = 0 );
    ~WellenreiterMainWindow();
    QString getFileName( bool save );

  protected:
    Wellenreiter* mw;
    WellenreiterConfigWindow* cw;

    QToolButton* startButton;
    QToolButton* stopButton;
    QToolButton* uploadButton;
    int startID;
    int stopID;
    int uploadID;

  protected:
    virtual void closeEvent( QCloseEvent* );
    void updateToolButtonState();

  public slots:
    void showConfigure();
    void demoAddStations();
    void fileSaveLog();
    void fileSaveHex();
    void fileSaveSession();
    void fileLoadSession();
    void fileNew();
    void uploadSession();
    void changedSniffingState();
};

#endif
