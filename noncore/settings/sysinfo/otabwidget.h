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

#include <qtabwidget.h>

class TabInfo
{
public:
    TabInfo() : c( 0 ), i( 0 ), l( QString::null ) {}
    TabInfo( QWidget *control, const QString &icon, const QString &label )
        : c( control ), i( icon ), l( label ) {}
    QString  label()   const { return l; }
    QWidget *control() const { return c; }
    QString  icon()    const { return i; }

private:
    QWidget *c;
    QString  i;
    QString  l;
};

typedef QValueList<TabInfo> TabInfoList;

class OTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    OTabWidget( QWidget *, const char * );
    ~OTabWidget();

    void addTab( QWidget *, const QString &, const QString & );

private:
    TabInfoList Tabs;
    TabInfoList::Iterator CurrentTab;

    QPixmap loadSmooth( const QString & );

private slots:
    void tabChangedSlot( QWidget * );
};

#endif
