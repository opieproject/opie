/****************************************************************************
** $Id: helpwindow.h,v 1.3 2003-04-22 14:11:34 llornkcor Exp $
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
    HelpWindow( const QString& home_=0,  const QString& path=0, QWidget* parent=0, const char *name=0 );
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

