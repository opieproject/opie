/* ----------------------------------------------------------------------- */
/*                                                                         */
/* [konsole.h]                      Konsole                                   */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole, an X terminal.                               */
/*                                                                            */
/* The material contained in here more or less directly orginates from        */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org>     */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                        */
/* Ported Konsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */

#ifndef KONSOLE_H
#define KONSOLE_H


#include <qmainwindow.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qstrlist.h>
#include <qintdict.h>
#include <qptrdict.h>
#include <qtabwidget.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qcolor.h>

#include "MyPty.h"
#include "TEWidget.h"
#include "TEmuVt102.h"
#include "session.h"

class EKNumTabWidget;

class Konsole : public QMainWindow
{
    Q_OBJECT

public:

    static QString appName()
    {
        return QString::fromLatin1("embeddedkonsole");
    }

    Konsole(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    Konsole(const char * name, const char* pgm, QStrList & _args, int histon);
    ~Konsole();
    void setColLin(int columns, int lines);
    QToolBar *secondToolBar;
    void show();
    void setColor(int);
    int lastSelectedMenu;
    int startUp;

public slots:
    void changeFontSize(int);
    void toggleFullScreen();
    void setFullScreen(bool);
    void changeSession(int);
    void cycleZoom();
    void newSession();

private slots:
    void setDocument(const QString &);
    void doneSession(TEWidget*,int);
    void changeTitle(TEWidget*,QString);
    void changeColumns(int);
    void setFont(int);
    //  void fontChanged(int);
    void configMenuSelected(int );
    void colorMenuSelected(int);
    void colorMenuIsSelected(int);
    void tabMenuSelected(int);
    void sessionListSelected(int);

    void enterCommand(int);
    void hitEnter();
    void hitSpace();
    void hitTab();
    void hitPaste();
    void hitUp();
    void hitDown();
    void switchSession(QWidget *);
    void changeCommand(const QString &, int);
    void initCommandList();
    void scrollMenuSelected(int);
    void editCommandListMenuSelected(int);
    void parseCommandLine();
    void changeForegroundColor(const QColor &);
    void changeBackgroundColor(const QColor &);

    void historyDialog();
    void fullscreenTimeout();

private:
    void doWrap();
    void init(const char* _pgm, QStrList & _args);
    void initSession(const char* _pgm, QStrList & _args);
    void runSession(TESession* s);
    void setColorPixmaps();
    void setHistory(bool);
    void setColors(QColor foreground, QColor background);
    int findFont(QString name, int size, bool exact = false);
    QSize calcSize(int columns, int lines);
    TEWidget* getTe();
    QStringList commands;
    QLabel * msgLabel;
    QColor foreground, background;
    bool fromMenu;

    bool fullscreen;

private:
    class VTFont
    {
    public:
        VTFont(QString name, QFont& font, QString family, int familyNum, int size)
        {
            this->name = name;
            this->font = font;
            this->family = family;
            this->size = size;
            this->familyNum = familyNum;
        }

        QFont& getFont()
        {
            return font;
        }
        QString getName()
        {
            return name;
        }
        int getSize()
        {
            return(size);
        }
        QString getFamily()
        {
            return(family);
        }
        int getFamilyNum()
        {
            return(familyNum);
        }

    private:
        QFont font;
        QString name;
        QString family;
        int familyNum;
        int size;
    };

    EKNumTabWidget* tab;
    int tabPos;
    int nsessions;
    QList<VTFont> fonts;
    int cfont;
    QCString se_pgm;
    QStrList se_args;

    QToolBar *menuToolBar;
    QToolBar *toolBar;
    QComboBox *commonCombo;

    QPopupMenu *fontList,*configMenu,*colorMenu,*scrollMenu,*editCommandListMenu;
    QPopupMenu *sessionList, *tabMenu;

    int sm_none, sm_left, sm_right;
    int cm_beep, cm_wrap;
    int cm_default;
    int cm_bw, cm_wb, cm_gb, cm_bt, cm_br, cm_rb, cm_gy, cm_bm, cm_mb, cm_cw, cm_wc, cm_bb, cm_ab;
    int tm_top, tm_bottom, tm_hidden;
    int ec_edit, ec_cmdlist, ec_quick;

    bool show_fullscreen_msg;
    QTimer *fullscreen_timer;
    QLabel *fullscreen_msg;


    // history scrolling I think
    bool        b_scroll;

    int         n_keytab;
    int         n_scroll;
    int         n_render;
    QString     pmPath; // pixmap path
    QString     dropText;
    QFont       defaultFont;
    QSize       defaultSize;

};

#endif

