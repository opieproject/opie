/***************************************************************************
                          LibraryDialog.h  -  description
                             -------------------
    begin                : Sat Aug 19 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <qpe/config.h>

#include "SearchDialog.h"
#include "NetworkDialog.h"
#include <qstringlist.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qheader.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qlabel.h>
#include <stdlib.h>
#include <qtabwidget.h>

class QVBoxLayout;
class QHBoxLayout; 
class QGridLayout; 
class QListView;
class QListViewItem;
class QPushButton;
//class QTabWidget;
class QWidget;
//class Gutenbrowser;

class LibraryDialog : public QDialog { 
    Q_OBJECT

public:
    LibraryDialog( QWidget* parent = 0, const char* name = 0 , bool modal = TRUE, WFlags fl = 0 );
    ~LibraryDialog();

    QTabWidget *tabWidget;
    QListView *ListView1,*ListView2,*ListView3,*ListView4,*ListView5;
    QWidget *widget_1,*widget_2,*widget_3,*widget_4,*widget_5,*widget_6;
    QString ftp_host,ftp_base_dir;
    QLabel *statusLabel;
    QListViewItem *QList_Item1,*QList_Item2,*QList_Item3,*QList_Item4,*QList_Item5;
    QStringList list,ItemStrlist,Searchlist,resultLs;
//    QStringList list1,list2,list3,list4;

    QPushButton *buttonSearch,*buttonCancel,*buttonDownload,*buttonLibrary,*buttonNewList,*moreInfoButton;
    QString new_index,old_index;
    QFile newindexLib,indexLib;

    int checked;
    int i_binary;
    bool useSmallInterface;
bool indexLoaded;
    QPushButton *cancel,*DownloadAll;     
    QCheckBox *checkBox,*httpBox,*authBox;    
//    QCheckBox * httpBox;    
    
    QString filename;
    QString downDir;
    int ftpNumb;
    int i;
    QString year,title,number,file,author;   
    QString DlglistItemNumber,DlglistItemFile,DlglistItemYear,DlglistItemTitle,NewlistItemFile;
    QString m_getFilePath;        
    QString Edir;     
            
    QString index;
    QString IDontKnowWhy;
    QString local_library;
    QString local_index;
    QString File_Name;
    QString proxy_http;
    int doitAll;      
    QString texter;
//    CConfigFile *config;
    void parseSearchResults( QString resultStr);

public slots:
    bool getItem(QListViewItem* );
    void doListView();
    void FindLibrary();
    void newList();
    void Newlibrary();
    void Library();
    bool getAuthor();
    void select_title(QListViewItem*);
    void cancelIt();
    void sort();
    bool moreInfo();
//    void DownloadEmAll();
    bool httpDownload();
    bool setTitle();
    void saveConfig();
    bool download_Etext();
    void onButtonSearch();
    bool onButtonDownload();

//    bool UnzipIt( QString fileName);
   void comboSelect(int index);        
protected slots:
    
    
protected:

 void initDialog();
 QHBoxLayout *hbox,*hbox1,*hbox2;
 QVBoxLayout *vbox;
// void search_slot();

};

#endif // LIBRARYDIALOG_H
