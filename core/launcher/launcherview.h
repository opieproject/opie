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
#include <qiconview.h>
#include <qtimer.h>
#include <qmap.h>

class CategorySelect;
class LauncherIconView;
class LauncherItem;
class QIconViewItem;
class QLabel;
class QWidgetStack;
class MenuButton;
class QComboBox;

enum BusyIndicatorType {
    BIT_Normal = 0,
    BIT_Animated
};

class LauncherView : public QVBox
{
    Q_OBJECT

public:
    LauncherView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~LauncherView();

    void hideIcons();

    bool removeLink(const QString& linkfile);
    void addItem(AppLnk* app, bool resort=TRUE);
    void changeItem(const AppLnk&old,AppLnk*nlink);

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

    void setColNumber( int );
    void relayout(void);
    LauncherIconView* iconView() { return icons; };

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

};

/* from opie-eye */
struct PixmapInfo {
    PixmapInfo() : width( -1 ), height( -1 ) {}
    bool operator==( const PixmapInfo& r ) {
        if ( width  != r.width  ) return false;
        if ( height != r.height ) return false;
        if ( file   != r.file   ) return false;
        return true;
    }
    int width, height;
    QString file;
    QPixmap pixmap;
};

class LauncherThumbReceiver:public QObject
{
    Q_OBJECT
    typedef QValueList<PixmapInfo> PixmapInfos;
public:
    LauncherThumbReceiver();
    ~LauncherThumbReceiver();
    void requestThumb(const QString&file,int width,int height);

public slots:
    void recieve( const QCString&, const QByteArray& );
protected slots:
    virtual void sendRequest();

signals:
    void sig_Thumbnail(const QPixmap&,const QString&,int);

protected:
    QTimer requestTimer;
    PixmapInfos m_inThumbNail;
};

class LauncherIconView : public QIconView {
    Q_OBJECT
public:
    LauncherIconView( QWidget* parent, const char* name=0 );
    ~LauncherIconView();
    QIconViewItem* busyItem() const;

#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    QPixmap busyPixmap() const { return busyPix; }
#endif
    void setBigIcons( bool bi );
    void updateCategoriesAndMimeTypes();
    void setBusyIndicatorType ( BusyIndicatorType t );
    void setStaticBackgroundPicture( bool enable );
    void doAutoScroll()
    {
    // We don't want rubberbanding (yet)
    }

    void setBusy(bool on);
    bool inKeyEvent() const { return ike; }

    void addItem(AppLnk* app, bool resort=TRUE);
    bool removeLink(const QString& linkfile,bool removeCache = true);
    void changeItem(const AppLnk&old,AppLnk*nlink);

    QStringList mimeTypes() const;
    QStringList categories() const;
    void clear();
    void addCatsAndMimes(AppLnk* app);

    void setBackgroundOrigin( QWidget::BackgroundOrigin ) {}

    void setBackgroundPixmap( const QPixmap &pm ) {
        bgPixmap = pm;
    }

    void setBackgroundColor( const QColor &c ) {
        bgColor = c;
    }

    void setColNumber( int );

    void drawBackground( QPainter *p, const QRect &r );
    void setItemTextPos( ItemTextPos pos );
    void hideOrShowItems(bool resort);

    void setTypeFilter(const QString& typefilter, bool resort);
    void setCategoryFilter( int catfilter, bool resort );

    enum SortMethod { Name, Date, Type };

    void setSortMethod( SortMethod m );
    int compare(const AppLnk* a, const AppLnk* b);
    void requestEyePix(const LauncherItem*which);

    static QMap<QString,QPixmap>* sm_EyeCache;

    virtual void setPalette(const QPalette & palette);
    virtual void unsetPalette();

protected:
    virtual void timerEvent( QTimerEvent *te );
    void styleChange( QStyle &old );
    void calculateGrid( ItemTextPos pos );
    void focusInEvent( QFocusEvent * ) {}
    void focusOutEvent( QFocusEvent * ) {}
    LauncherItem*findDocItem(const QString&);
    void addCheckItem(AppLnk* app);
    void checkCallback();
    virtual void keyPressEvent(QKeyEvent* e);

protected slots:
    void setEyePixmap(const QPixmap&,const QString&,int width);
    void stopEyeTimer();

private:
    QList<AppLnk> hidden;
    QDict<void> mimes;
    QDict<void> cats;
    SortMethod sortmeth;
    QRegExp tf;
    int cf;
    LauncherItem* bsy;
    int busyTimer;
    bool ike;
    bool bigIcns;
    QPixmap bgPixmap;
    QColor bgColor;
    LauncherThumbReceiver*m_EyeCallBack;
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    QPixmap busyPix;
#endif
    BusyIndicatorType busyType;
    QTimer m_eyeTimer;
    int numColumns;
    bool staticBackground;
};

#endif // LAUNCHERVIEW_H
