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

#ifndef __START_MENU_H__
#define __START_MENU_H__

#include <qtopia/applnk.h>
#include <qintdict.h>
#include <qstring.h>
#include <qlist.h>
#include <qmap.h>
#include <qdir.h>
#include <qlabel.h>
#include <qpopupmenu.h>

#include <qtopia/menuappletinterface.h>

class StartPopupMenu : public QPopupMenu
{

public:
    StartPopupMenu( QWidget *parent ) : QPopupMenu( parent ) {}
    
protected:
    void keyPressEvent( QKeyEvent *e );

};

class QLibrary;


struct MenuApplet {
    QLibrary *library;
    MenuAppletInterface *iface;
    int id;
    QPopupMenu *popup;
};

class StartMenu : public QLabel {
    
    Q_OBJECT
        
public:
    StartMenu( QWidget * );
    ~StartMenu();

    void refreshMenu();

public:
    StartPopupMenu *launchMenu;

signals:
    void tabSelected(const QString&);

public slots:
    void launch( );
    void createMenu( );

protected slots:
    void itemSelected( int id );

protected:
    virtual void mousePressEvent( QMouseEvent * );

private:
    void loadApplets();
    void clearApplets();
    void addApplets( QPopupMenu* menu );
    bool loadMenu( QPopupMenu *menu );
    void createMenuEntries( QPopupMenu *menu, QDir dir, bool ltabs, bool lot );
    void createDirEntry( QPopupMenu *menu, QDir dir, QString file, bool lot );
    void createAppEntry( QPopupMenu *menu, QDir dir, QString file );

private:
    bool useWidePopupMenu;
    QString popupMenuSidePixmap;

    bool startButtonIsFlat;
    QString startButtonPixmap;

    int currentItem;

    QIntDict<AppLnk> appLnks;
    QIntDict<QString> tabNames;
    QIntDict<MenuApplet> menuApplets;
    
};

#endif // __START_MENU_H__
