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

#include <qstring.h>
#include <qlist.h>
#include <qintdict.h>
#include <qlabel.h>
#include <qpopupmenu.h>

#include <qpe/menuappletinterface.h>

class AppLnkSet;
class AppLnk;

class StartPopupMenu : public QPopupMenu
{
public:
    StartPopupMenu( QWidget *parent ) : QPopupMenu( parent ) {}
protected:
    void keyPressEvent( QKeyEvent *e );
};

class QLibrary;

struct MenuApplet
{
#ifndef QT_NO_COMPONENT
    QLibrary *library;
#endif
    MenuAppletInterface *iface;
    int id;
    QPopupMenu *popup;
};
            

class StartMenu : public QLabel {
    Q_OBJECT
public:
    StartMenu( QWidget * );
    ~StartMenu();

    const AppLnk* execToLink(const QString& appname);

public:
    StartPopupMenu *launchMenu;

public slots:
    void launch( );
    void loadOptions( );
    void createMenu( );
    void reloadApps( );
    void reloadApplets( );

protected slots:
    void itemSelected( int id );

protected:
    virtual void mousePressEvent( QMouseEvent * );
    virtual void timerEvent ( QTimerEvent * );
    
private:
    bool loadMenu( AppLnkSet *folder, QPopupMenu *menu );
    void loadApplets( );
    void clearApplets( );

private:
    bool useWidePopupMenu;
    QString popupMenuSidePixmap;

    bool startButtonIsFlat;
    QString startButtonPixmap;

    AppLnkSet *apps;

	QIntDict<MenuApplet> applets;
	QIntDict<QPopupMenu> tabdict;
	
//    QValueList<MenuApplet> appletList;
    int safety_tid;
    int sepId;
};

#endif // __START_MENU_H__
