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
#ifndef FILESELECTOR_P_H
#define FILESELECTOR_P_H

#include <qlistview.h>

class FileManager;
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


#endif
