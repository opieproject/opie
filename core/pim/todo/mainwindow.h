/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class TodoTable;
class QAction;
class QPopupMenu;
class Ir;
class CategorySelect;

class TodoWindow : public QMainWindow
{
    Q_OBJECT

public:
    TodoWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~TodoWindow();

public slots:
    void flush();
    void reload();

protected slots:
    void slotNew();
    void slotDelete();
    void slotEdit();
    void slotShowPopup( const QPoint & );
    void setShowCompleted( int );
    void currentEntryChanged( int r, int c );
    void slotFind();
    void setDocument( const QString & );
    void slotBeam();
    void beamDone( Ir * );
    void catSelected(int);
    
protected:
    void closeEvent( QCloseEvent *e );

private:
    TodoTable *table;
    QAction *editAction,
	    *deleteAction,
	    *findAction;
    QPopupMenu *contextMenu, *catMenu;
    CategorySelect *catSelect;

    bool syncing;
};

#endif
