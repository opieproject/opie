/****************************************************************************
** $Id: helpwindow.h,v 1.2 2003-04-21 23:58:24 llornkcor Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <qmainwindow.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
#include <qevent.h>

class QComboBox;
class QPopupMenu;

class HelpWindow : public QMainWindow
{
    Q_OBJECT
public:
    HelpWindow( const QString& home_,  const QString& path, QWidget* parent, const char *name );
    ~HelpWindow();

private slots:

    void addBookmark();
    void bookmChosen( int );
    void histChosen( int );
    void newWindow();
    void openFile();
    void pathSelected( const QString & );
    void print();
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );
    void textChanged();
private:
    QTextBrowser* browser;
    QComboBox *pathCombo;
    int backwardId, forwardId;
    QString selectedURL;
    QStringList history, bookmarks;
    QMap<int, QString> mHistory, mBookmarks;
    QPopupMenu *hist, *bookm;
    
    void readHistory();
    void readBookmarks();
    
};





#endif

