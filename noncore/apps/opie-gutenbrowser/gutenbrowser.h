/***************************************************************************
                          gutenbrowser.h  -  description
                             -------------------
    begin                : Mon Jul 24 22:33:12 MDT 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef GUTENBROWSER_H
#define GUTENBROWSER_H

class QPainter;
class QPushButton;
class QListView;
class QString;
class QTextView;
class QPopupMenu;
class QMenuBar;
class QToolBar;
class QLayout;
class QToolTip;
class QPixMap;
class QFile;

#include <qtoolbutton.h>

#include <qpe/qpeapplication.h>
#include <qpe/qpemenubar.h>
#include <qpe/menubutton.h>

#include <qmainwindow.h>
#include <qapplication.h>
#include <qwhatsthis.h>
#include <qfontmetrics.h>
#include <qfontinfo.h>
#include <qpoint.h>

#include "LibraryDialog.h"
#include "multiline_ex.h"
#include "resource.h"

#include <qvaluelist.h>

class Gutenbrowser : public QMainWindow {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("gutenbrowser"); }
#ifdef NOQUICKLAUNCH
		Gutenbrowser();
#else
		Gutenbrowser(QWidget *p = 0, const char* name = 0, WFlags fl = 0);
#endif
    ~Gutenbrowser();
    LibraryDialog* LibraryDlg;
    QButton* InfoBar;
    QFile newindexLib, indexLib;
    QListBox *mainList;
    QPrinter *printer;
    QString brow;
    QString file_name;
    QString fontStr, pointSizeStr, weightStr, italicStr, boldStr;
    QString ftp_base_dir;
    QString ftp_host;
    QString iniFile;
    QString listItemFile;
    QString listItemNumber;
    QString listItemTitle;
    QString listItemYear;
    QString local_index;
    QString local_library;
    QString networkUrl;
    QString pattern;
    QString prefBrowser;
    QString s_Wrap;
    QString status;
    QString statusTop;
    QString title;
    QPushButton *DownloadButton, *LibraryButton, *OpenButton, *ForwardButton, *BackButton, *PrintButton, *SearchButton, *dictionaryButton, *beginButton, *ExitButton, *OptionsButton, *FontButton, *ClearButton, *HelpButton, *setBookmarkButton;
    MenuButton* lastBmkButton;
    SearchDialog* searchDlg;

    bool buttonsHidden;
    bool italic, bold;
    bool loadCheck;
    bool useSplitter;
    char host[1024];
    char login[1024];
    char password[1024];
    int check;
    int direction;
    int i_pageNum;
    int lastScrollRow;
    int lineCheck;
    int pages;
    int pointSize, weight;
//    int styleI;

public slots:

private:

/*     typedef QValueList<int> pageStop; */
/*     pageStop pageStopArray; */
//     QVector<int>  pageStopArray;
    QArray<int>  pageStopArray;

    MultiLine_Ex* Lview;
    QBoxLayout *buttons2;
    QBoxLayout *buttons;
    QBoxLayout *edits;
    QBoxLayout *menu;
    QBoxLayout *topLayout;
    QFile f;
    QPEMenuBar* menubar;
    QPopupMenu *fileMenu, *editMenu, *viewMenu, *DownloadMenu, *optionsMenu, *bookmarksMenu, *donateMenu, *helpMenu;
    bool useWrap;
    int fileHandle, currentFilePos, currentLine; 
    int last_search;

    bool getTitle( const char *file);
    bool setTitle();
    int repeatSearch();
    void  Search();
    void OnNetworkDialog( const QString &networkUrl,const QString & output);
    void hideButtons();

private slots:

    bool load( const char *fileName );
    bool setStatus();
    void BeginBtn();
    void TopBtn();
    void cleanUp();
    void doBeginBtn();
    void goGetit(const QString &url, bool showMsg);
    void infoGutenbrowser();
    void listClickedSlot( QListBoxItem *);
    void mainListPressed(int, QListBoxItem *, const QPoint &);
    void menuEditTitle();
    void search_slot();
    void searchdone_slot();
    void setDocument(const QString&);
    void slotFilePrint();
//    void setStyle(int styleInt);


protected:

    bool UnZipIt(const QString &zipFile);
    bool b_queryExit;
    bool queryExit();
    bool showMainList;
    bool useIcons;
    bool working;


protected slots:
    
    void BackBtn();
    void Bookmark( int);
    void ByeBye();
    void ChangeFont();
    void ClearEdit();
    void DownloadIndex();
    void ForwardBtn();
    void HelpBtn();
    void InfoBarClick();
    void LibraryBtn();
    void LookupBtn();
    void OpenBtn();
    void PrintBtn();
    void SearchBtn();
    void annotations();
    void doOptions();
    void donateByteMonkie();
    void donateGutenberg();
    void downloadFtpList();
    void downloadLibIndex();
    void enableButtons(bool);
    void fillWithTitles();
    void fixKeys();
    void initButtonBar();
    void initConfig();
    void initMenuBar();
    void initSlots();
    void initStatusBar();
    void initView();
    void menuGoogle();
    void menuOpen();
    void searchGoogle(const QString &);
    void setBookmark();
    void toggleButtonIcons( bool usingIcons);
//    virtual void setBrowser();
//      void stylePlatinum();
//      void styleWindows();
//      void defaultStyle();
/*      void styleMotif(); */
/*      void styleMotifPlus(); */
/*      void styleWood(); */
/*      void stylePlatinumPlus(); */
/*      void styleCDE(); */
//      void styleMetal();
/*      void styleSgi();     */
/*      void styleLace(); */
/*      void styleLacePlus(); */
/*  #endif */
protected:

    int doSearch(const QString & s_pattern , bool case_sensitive, bool forward, int line, int col );
    void focusInEvent( QFocusEvent* );
    void focusOutEvent( QFocusEvent* );
    void keyPressEvent( QKeyEvent * );
    void keyReleaseEvent( QKeyEvent *e);
    void resizeEvent( QResizeEvent * );
    void writeConfig();
//    void moveSplitter( int pos);
};
#endif

