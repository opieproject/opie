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
#ifndef LAUNCHERVIEW_H
#define LAUNCHERVIEW_H

#include <qtopia/storage.h>
#include <qtopia/applnk.h>

#include <qvbox.h>

class CategorySelect;
class LauncherIconView;
class QIconView;
class QIconViewItem;
class QLabel;
class QWidgetStack;
class MenuButton;
class QComboBox;

class LauncherView : public QVBox
{
    Q_OBJECT

public:
    LauncherView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~LauncherView();

    void hideIcons();

    bool removeLink(const QString& linkfile);
    void addItem(AppLnk* app, bool resort=TRUE);
    void removeAllItems();
    void setSortEnabled(bool);
    void setUpdatesEnabled(bool);
    void sort();

    void setToolsEnabled(bool);
    void updateTools();

    void setBusy(bool);
    void setBusyIndicatorType( const QString& );

    enum ViewMode { Icon, List };
    void setViewMode( ViewMode m );
    ViewMode viewMode() const { return vmode; }

    enum BackgroundType { Ruled, SolidColor, Image };
    void setBackgroundType( BackgroundType t, const QString & );
    BackgroundType backgroundType() const { return bgType; }

    void setTextColor( const QColor & );
    QColor textColor() const { return textCol; }

    void setViewFont( const QFont & );
    void clearViewFont();

    void relayout(void);

signals:
    void clicked( const AppLnk * );
    void rightPressed( AppLnk * );

protected slots:
    void selectionChanged();
    void returnPressed( QIconViewItem *item );
    void itemClicked( int, QIconViewItem * );
    void itemPressed( int, QIconViewItem * );
    void sortBy(int);
    void showType(int);
    void showCategory( int );
    void resizeEvent(QResizeEvent *);
    void flushBgCache();

protected:
    void paletteChange( const QPalette & );

    void fontChanged(const QFont &);

private:
    static bool bsy;
    QWidget* tools;
    LauncherIconView* icons;
    QComboBox *typemb;
    QStringList typelist;
    CategorySelect *catmb;
    ViewMode vmode;
    BackgroundType bgType;
    QString bgName;
    QColor textCol;

    QImage loadBackgroundImage(QString &fname);
    int m_busyType;
};

#endif // LAUNCHERVIEW_H
