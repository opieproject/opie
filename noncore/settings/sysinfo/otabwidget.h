/**********************************************************************
** OTabWidget
**
** Modified tab widget control
**
** Copyright (C) 2002, Dan Williams
**                    williamsdr@acm.org
**                    http://draknor.net
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef OTABWIDGET_H
#define OTABWIDGET_H

#include <qwidget.h>
#include <qlist.h>

class QComboBox;
class QPixmap;
class QTabBar;
class QWidgetStack;

class TabInfo
{
public:
    TabInfo() : i( -1 ), c( 0 ), p( 0 ), l( QString::null ) {}
    TabInfo( int id, QWidget *control, const QString &icon, const QString &label )
        : i( id ), c( control ), p( icon ), l( label ) {}
    int      id()      const { return i; }
    QString  label()   const { return l; }
    QWidget *control() const { return c; }
    QString  icon()    const { return p; }

private:
    int      i;
    QWidget *c;
    QString  p;
    QString  l;
};

typedef QList<TabInfo> TabInfoList;

class OTabWidget : public QWidget
{
    Q_OBJECT
public:
    enum TabStyle { Global, TextTab, IconTab, TextList, IconList };
    TabStyle tabStyle() const;
    void setTabStyle( TabStyle );

    enum TabPosition { Top, Bottom };
    TabPosition tabPosition() const;
    void setTabPosition( TabPosition );

    OTabWidget( QWidget *, const char *, TabStyle, TabPosition );
    ~OTabWidget();

    void addTab( QWidget *, const QString &, const QString & );
    void setCurrentTab( QWidget * );
    void setCurrentTab( QString );
    QSize sizeHint() const;


protected:
    void resizeEvent( QResizeEvent * );

private:
    TabInfoList  tabs;
    TabInfo     *currentTab;

    TabStyle    tabBarStyle;
    TabPosition tabBarPosition;

    QWidgetStack *tabBarStack;
    QTabBar      *tabBar;
    QComboBox    *tabList;

    QWidgetStack *widgetStack;

    QPixmap loadSmooth( const QString & );
    void    selectTab( TabInfo * );
    void    setUpLayout();

private slots:
    void slotTabBarSelected( int );
    void slotTabListSelected( int );
};

#endif
