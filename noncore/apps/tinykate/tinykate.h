/***************************************************************************
                             tinykate.h
			Tiny KATE mainwindow
                             -------------------
    begin                : November 2002
    copyright            : (C) 2002 by Joseph Wenninger <jowenn@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                         *
 *   ONLY VERSION 2 OF THE LICENSE IS APPLICABLE                           *
 *                                                                         *
 ***************************************************************************/

#ifndef __TINYKATE_H__
#define __TINYKATE_H__


#include <qmainwindow.h>
#include <opie/otabwidget.h>
#include <ktexteditor.h>

class QAction;
class QPopupMenu;

class TinyKate : public QMainWindow
{
Q_OBJECT
public:
    TinyKate( QWidget *parent=0, const char *name=0, WFlags f = 0);

public slots:
    void slotNew();

protected slots:
    void slotOpen();
    void slotClose();
    void slotCurrentChanged(QWidget *);

protected:
    void open(const QString&);
private:
    OTabWidget *tabwidget;
    KTextEditor::View *currentView;

    QAction *editCopy;
    QAction *editCut;
    QAction *editPaste;
    QAction *editUndo;
    QAction *editRedo;
    QAction *editFindReplace;
    QAction *viewIncFontSizes;
    QAction *viewDecFontSizes;
    QAction *utilSettings;

    QPopupMenu *hlmenu;
    uint nextUnnamed;
    uint viewCount;
};


#endif // __TINYKATE_H__
