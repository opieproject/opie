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

class QToolButton;
class QAction;
class QPopupMenu;

class TinyKate : public QMainWindow
{
Q_OBJECT
public:
    TinyKate( QWidget *parent=0, const char *name=0, WFlags f = 0);
    ~TinyKate( );
    static QString appName() { return QString::fromLatin1( "kate" ); };


public slots:
    void slotNew();
 void setDocument(const QString& fileref);

protected slots:
    void slotOpen();
    void slotClose();
    void slotCurrentChanged(QWidget *);
    void slotSave();
    void slotSaveAs();
protected:
    void open(const QString&);
private:
    QString currentFileName;
    OTabWidget *tabwidget;
    KTextEditor::View *currentView;
    bool shutDown;

    QToolButton *editCopy, *editCut, *editPaste, *editUndo, *editRedo,  *editFindReplace;
    QAction *viewIncFontSizes, *viewDecFontSizes, *utilSettings;

    QPopupMenu *hlmenu;
    uint nextUnnamed;
    uint viewCount;
};


#endif // __TINYKATE_H__
