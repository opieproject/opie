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

#include <qpe/storage.h>

#include <qvbox.h>

class AppLnk;
class AppLnkSet;
class CategorySelect;
class LauncherIconView;
class QIconView;
class QIconViewItem;
class MenuButton;
class QComboBox;

class LauncherView : public QVBox
{
    Q_OBJECT

public:
    LauncherView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~LauncherView();

    bool removeLink(const QString& linkfile);
    void addItem(AppLnk* app, bool resort=TRUE);
    void sort();

    void setFileSystems(const QList<FileSystem> &);
    void setToolsEnabled(bool);
    void updateTools();

    void setBusy(bool);

    QString getAllDocLinkInfo() const;
    enum ViewMode { Icon, List };
    void setViewMode( ViewMode m );
    ViewMode viewMode() const { return vmode; }

    enum BackgroundType { Ruled, SolidColor, Image };
    void setBackgroundType( BackgroundType t, const QString & );
    BackgroundType backgroundType() const { return bgType; }

    void setTextColor( const QColor & );
    QColor textColor() const { return textCol; }

    void setViewFont( const QFont & );
    void unsetViewFont ( );

    void setBusyIndicatorType ( const QString &type );

public slots:
    void populate( AppLnkSet *folder, const QString& categoryfilter );

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

protected:
    void internalPopulate( AppLnkSet *, const QString& categoryfilter );
    void paletteChange( const QPalette & );

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
    int busyType;
};

#endif // LAUNCHERVIEW_H
