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
#ifndef LAUNCHERTAB_H
#define LAUNCHERTAB_H

#include <qcolor.h>
#include <qstring.h>
#include <qwidget.h>
#include <qrect.h>
#include <qiconset.h>
#include <qlist.h>
#include <qdict.h>
#include <qtabbar.h>

class LauncherView;

class LauncherTab : public QTab
{
public:
    LauncherTab( const QString &idStr, LauncherView *viewPtr,
		 const QIconSet &iconSet,
		 const QString &textStr=QString::null )
	: QTab( iconSet, textStr )
    {
	type = idStr;
	view = viewPtr;
    }

    QString type;
    LauncherView *view;
    QColor bgColor;
    QColor fgColor;
};

class LauncherTabBar : public QTabBar
{
    Q_OBJECT
public:
    LauncherTabBar( QWidget *parent=0, const char *name=0 );
    ~LauncherTabBar();

    void insertTab( LauncherTab *, int index = -1 );
    void removeTab( QTab * );

    LauncherView *currentView() {
	return currentLauncherTab()->view;
    }

    LauncherTab *currentLauncherTab() {
	return (LauncherTab*) tab( currentTab() );
    }

    LauncherTab *launcherTab( const QString &id ) const { return tabs[id]; }

    void nextTab();
    void prevTab();

public slots:
    void showTab(const QString&);

protected slots:
    virtual void layoutTabs();

protected:
    virtual void paint( QPainter *p, QTab *t, bool f ) const;
    void paintLabel( QPainter* p, const QRect& br, QTab* t, bool has_focus ) const;

private:
    QList<LauncherTab> items;
    QDict<LauncherTab> tabs;
};

#endif
