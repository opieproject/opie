#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

#include <qtoolbar.h>
#include <qmenubar.h>
#include <qaction.h>

#include "qlinphone.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    virtual ~MainWindow();

    static QString appName() {
        return QString::fromLatin1("olinphone");
    }

  QLinphoneMainWidget *mainWidget;
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QPopupMenu *prefMenu;

    QAction *settings;

public slots:
    void slotSettings();

};

#endif
