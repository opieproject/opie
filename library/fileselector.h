/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <qhbox.h>
#include <qvbox.h>
#include <qlistview.h>
#include <qtoolbutton.h>

#include "filemanager.h"
#include "applnk.h"

class QPopupMenu;
class QPushButton;

class FileSelectorItem : public QListViewItem
{
public:
    FileSelectorItem( QListView *parent, const DocLnk& f );
    ~FileSelectorItem();

    DocLnk file() const { return fl; }

private:
    DocLnk fl;
};

class CategoryMenu;
class FileSelectorViewPrivate;
class FileSelectorView : public QListView
{
    Q_OBJECT

public:
    FileSelectorView( const QString &mimefilter, QWidget *parent, const char *name );
    ~FileSelectorView();
    void reread();
    int fileCount() { return count; }

    void setCategoryFilter(CategoryMenu *);
protected:
    void keyPressEvent( QKeyEvent *e );

protected slots:
    void cardMessage( const QCString &, const QByteArray &);

    void categoryChanged();
    
private:
    QString filter;
    FileManager *fileManager;
    int count;
    FileSelectorViewPrivate *d;
};

class FileSelectorPrivate;
class FileSelector : public QVBox
{
    Q_OBJECT

public:
    FileSelector( const QString &mimefilter, QWidget *parent, const char *name, bool newVisible = TRUE, bool closeVisible = TRUE );
    ~FileSelector();
    void setNewVisible( bool b );
    void setCloseVisible( bool b );
    void setCategoriesVisible( bool b );
    void reread();
    int fileCount();
    const DocLnk *selected();

signals:
    void fileSelected( const DocLnk & );
    void newSelected( const DocLnk & );
    void closeMe();

private slots:
    void createNew();
    void fileClicked( int, QListViewItem *, const QPoint &, int );
    // pressed to get 'right down'
    void filePressed( int, QListViewItem *, const QPoint &, int );
    void fileClicked( QListViewItem *);

private:
    FileSelectorView *view;
    QString filter;
    QToolButton *buttonNew, *buttonClose;
    FileSelectorPrivate *d;
};

#endif
