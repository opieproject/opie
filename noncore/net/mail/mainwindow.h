#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "accountview.h"
#include <libmailwrapper/mailtypes.h>

#include <opie2/osmartpointer.h>

#include <qmainwindow.h>
#include <qlistview.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qmap.h>

class RecMail;
class QCopChannel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    virtual ~MainWindow();

public slots:
    virtual void slotAdjustColumns();
    virtual void appMessage(const QCString &msg, const QByteArray &data);
    virtual void slotComposeMail();
    virtual void systemMessage( const QCString&, const QByteArray& );

protected slots:
    virtual void slotSendQueued();
    virtual void slotEditAccounts();
    virtual void slotShowFolders( bool show );
    virtual void refreshMailView(const QValueList<RecMailP>&);
    virtual void displayMail();
    virtual void slotDeleteMail();
    virtual void mailHold(int, QListViewItem *,const QPoint&,int);
    virtual void slotAdjustLayout();
    virtual void slotEditSettings();
    virtual void mailLeftClicked( int, QListViewItem *,const QPoint&,int  );
    virtual void serverSelected(int);

protected:
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QPopupMenu *mailMenu, *settingsMenu,*serverMenu,*folderMenu;

    QAction *composeMail, *sendQueued, *showFolders, *searchMails, *deleteMails,
            *editSettings, *editAccounts, *syncFolders;
    AccountView *folderView;
    QListView *mailView;
    QBoxLayout *layout;
    int m_Rotate;
    QCopChannel* m_sysChannel;
    int m_ServerMenuId,m_FolderMenuId;
};

#endif
