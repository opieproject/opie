#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qlistview.h>
#include <qaction.h>

#include <qtoolbar.h>
#include <qmenubar.h>

#include "accountview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );

public slots:
    void slotAdjustColumns();
    
protected slots:
    virtual void slotShowFolders( bool show );
    virtual void refreshMailView(Maillist*);
    virtual void displayMail(QListViewItem*);
    
protected:
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QPopupMenu *mailMenu, *settingsMenu;
    QAction *composeMail, *sendQueued, *showFolders, *searchMails, 
            *editSettings, *editAccounts, *syncFolders;
    AccountView *folderView;
    QListView *mailView;

};

class MailListViewItem:public QListViewItem
{
public:
    MailListViewItem(QListView * parent, MailListViewItem * after )
        :QListViewItem(parent,after),mail_data(){}
    virtual ~MailListViewItem(){}
    
    void storeData(const RecMail&data){mail_data = data;}
    const RecMail&data()const{return mail_data;}
    void showEntry();
    
protected:
    RecMail mail_data;
};

#endif
