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
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <qtopia/config.h>
#include <qtopia/storage.h>
#include <qtopia/applnk.h>
#include <qmainwindow.h>
#include <qstringlist.h>
#include <qprogressbar.h>
#include <qvbox.h>
#include <qlist.h>
#include <qdict.h>
#include "launcherview.h"
#include "launchertab.h"
#include "serverinterface.h"

class QWidgetStack;
class TaskBar;
class Launcher;

class LauncherTabWidget : public QVBox {
    // can't use a QTabWidget, since it won't let us set the frame style.
    Q_OBJECT
public:
    LauncherTabWidget( Launcher* parent );

    void updateDocs(AppLnkSet* docFolder);
    void setBusy(bool on);
    LauncherView *currentView(void);

    LauncherView* newView( const QString&, const QPixmap& pm, const QString& label );
    void deleteView( const QString& );
    void setTabViewAppearance( LauncherView *v, Config &cfg );
    void setTabAppearance( LauncherTab *, Config &cfg );

    LauncherView *view( const QString & );
    LauncherView *docView();

    void createDocLoadingWidget();
    void setLoadingWidgetEnabled( bool v );
    void setLoadingProgress( int percent );

    LauncherTabBar* categoryBar;

signals:
    void selected(const QString&);
    void clicked(const AppLnk*);
    void rightPressed(AppLnk*);

protected slots:
    void raiseTabWidget();
    void tabProperties();
    void initLayout();

private slots:
    void launcherMessage( const QCString &, const QByteArray &);
    void appMessage( const QCString &, const QByteArray &);
    void setProgressStyle();

protected:
    void paletteChange( const QPalette &p );
    void styleChange( QStyle & );

private:
    Launcher *launcher;
    LauncherView *docview;

    QWidgetStack *stack;
    LauncherView *docLoadingWidget;
    QProgressBar *docLoadingWidgetProgress;
    bool docLoadingWidgetEnabled;
};

class Launcher : public QMainWindow, public ServerInterface
{
    Q_OBJECT
public:
    Launcher();
    ~Launcher();

    // implementing ServerInterface
    void createGUI();
    void destroyGUI();
    void typeAdded( const QString& type, const QString& name, const QPixmap& pixmap, const QPixmap& bgPixmap );
    void typeRemoved( const QString& type );
    void applicationAdded( const QString& type, const AppLnk& doc );
    void applicationRemoved( const QString& type, const AppLnk& doc );
    void allApplicationsRemoved();
    void applicationStateChanged( const QString& name, ApplicationState state );
    void documentAdded( const DocLnk& doc );
    void documentRemoved( const DocLnk& doc );
    void allDocumentsRemoved();
    void documentChanged( const DocLnk& oldDoc, const DocLnk& newDoc );
    void storageChanged( const QList<FileSystem> & );
    void applicationScanningProgress( int percent );
    void documentScanningProgress( int percent );
    bool requiresApplications() const { return TRUE; }
    bool requiresDocuments() const { return TRUE; }
    void showLoadingDocs();
    void showDocTab();

    QStringList idList() const { return ids; }

public slots:
    void viewSelected(const QString&);
    void showTab(const QString&);
    void select( const AppLnk * );
    void properties( AppLnk * );
    void makeVisible();

signals:
    void executing( const AppLnk * );

private slots:
    void systemMessage( const QCString &, const QByteArray &);
    void toggleSymbolInput();
    void toggleNumLockState();
    void toggleCapsLockState();

protected:
    bool eventFilter( QObject *o, QEvent *ev );

private:
    void updateApps();
    void loadDocs();
    void updateDocs();
    void updateTabs();

    LauncherTabWidget *tabs;
    QStringList ids;
    TaskBar *tb;
};

#endif // LAUNCHERVIEW_H
