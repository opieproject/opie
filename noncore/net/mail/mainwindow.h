#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qlistview.h>
#include <qaction.h>

#include <qtoolbar.h>
#include <qmenubar.h>

#include "accountview.h"

class RecMail;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    static QString appName() { return QString::fromLatin1("opiemail"); }

public slots:
    void slotAdjustColumns();
    
protected slots:
    virtual void slotShowFolders( bool show );
    virtual void refreshMailView(QList<RecMail>*);
    virtual void displayMail(QListViewItem*);
    void slotAdjustLayout();
    
protected:
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QPopupMenu *mailMenu, *settingsMenu;
    QAction *composeMail, *sendQueued, *showFolders, *searchMails, 
            *editSettings, *editAccounts, *syncFolders;
    AccountView *folderView;
    QListView *mailView;
    QBoxLayout *layout;
};

#endif
