/***************************************************************************
                          openetext.cpp  -  description
    begin                : Sat Dec 4 1999
    begin                : Tue Jul 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "gutenbrowser.h"
#include "openetext.h"
#include "editTitle.h"

/* OPIE */
#include <opie2/ofiledialog.h>
#include <opie2/odebug.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Ui;

/* QT */
#include <qtextstream.h>
#include <qfileinfo.h>

/* STD */
#if defined(_WS_WIN_)
#include <windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#endif

OpenEtext::OpenEtext(QWidget *parent, QString name) : QDialog(parent,name,true)
{
    local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
    local_index=local_library+"GUTINDEX.ALL";
    initDialog();
    connect( QListBox_1, SIGNAL(selected ( int)),this, SLOT(select_title(int)) );

    getTitles();
}

OpenEtext::~OpenEtext()
{
}

void OpenEtext::getTitles()
{
    QListBox_1->clear();
    Config config("Gutenbrowser");
    config.setGroup( "Files" );
    s_numofFiles = config.readEntry("NumberOfFiles", "0");
    QLabel_1->setText( tr(
        s_numofFiles+" etexts in your library."));
     int  i_numofFiles = s_numofFiles.toInt();
    for (int i = 1; i <= i_numofFiles+1; i++) {
        QString temp;
        temp.setNum(i);
        config.setGroup( "Files" );
        QString ramble = config.readEntry(temp, "" );
        config.setGroup( "Titles" );
        title = config.readEntry(ramble, "");
        if( !title.isEmpty()) {
            QListBox_1->insertItem ( QPixmap( QPEApplication::qpeDir()+"pics/gutenbrowser/gutenbrowser_sm.png"), title, -1);
        }
    }
}

/*
 opens selected title */
void OpenEtext::OpenTitle()
{
    if( QListBox_1->currentItem() != -1)
        select_title(  QListBox_1->currentItem() );
    else {
        QString sMsg;
        sMsg = "You must select an Etext to open";
        QMessageBox::message( "Error",sMsg);
    }
}

void OpenEtext::select_title(int index )
{
    OpenButton->setDown(TRUE);
    QString temp;
    temp.setNum( index + 1);
    const char * title;
    title = QListBox_1->text( index );
    openFileTitle = title;
    Config config("Gutenbrowser");
    config.setGroup( "Files" );
    file = config.readEntry(temp, "");
    OpenEtext::accept();
}

/*
 This function imports already existing etexts into the local library list*/
void OpenEtext::open()
{
OpenFileButton->setDown(TRUE);
 title = "";
    QString currDir="";
    QString filer;
    QString name_file;
    QString local ;
    local = (QDir::homeDirPath () +"Applications/gutenbrowser");
    QDir library( local);

//      fileBrowser *browseForFiles;
//      browseForFiles=new fileBrowser(this,"Browse for File", TRUE, 0, "guten/plain;text/plain");
//  //    browseForFiles=new fileBrowser(this,"fileBrowser",TRUE,0, "*");
//              browseForFiles->setFileView( 0);
//              browseForFiles->showMaximized();
//      browseForFiles->exec();
//      QString selFile= browseForFiles->selectedFileName;
//      fileList=browseForFiles->fileList;
    Config cfg("Gutenbrowser");
    cfg. setGroup ( "View" );
    QString dir = cfg.readEntry("LastOpenDirectory", QPEApplication::documentDir());
    QMap<QString, QStringList> map;
    map.insert(tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr("Text"), text );
    text << "*";
    map.insert(tr("All"), text );

    QString str = OFileDialog::getOpenFileName( 2, dir , QString::null, map);

    if( !str.isEmpty() && QFile(str).exists() && !QFileInfo(str).isDir() )  {
      filer = str;

    odebug << "Open file: "+str << oendl;

    //      QStringList::ConstIterator f;
//      QString fileTemp;
//      for (  f = fileList.begin(); f != fileList.end(); f++ ) {
//          fileTemp = *f;
//          fileTemp.right( fileTemp.length()-5);
//          fileName = fileTemp;
//          if( !fileName.isEmpty() ){
//              filer = fileName;
//          } else { //filename is empty
//  //              QString sMsg;
//  //              sMsg = "Error opening library filelist "+fileName;
//          }

        if( filer.right(4) == ".txt"
                        || filer.right(4) == ".TXT"
                        || filer.right(4) == ".etx"
                        || filer.right(4) == ".ETX"
                        || filer.right(4) == ".etx"
                        || filer.right(4) == ".ETX"
                        || filer.right(4) == ".zip"
                        || filer.right(4) == ".ZIP" ) {
            QFileInfo zipFile( filer);
            QString s_fileName = zipFile.fileName();
            QString cmd;

            if( s_fileName.right(4) == ".zip") { // unzip that sucker....
                s_fileName = s_fileName.left( s_fileName.length() - 4);
                if( chdir((const char*)local_library.latin1())!=0)
                    odebug << "chdir failed." << oendl; //                QString cmd = "gunzip -d " + filer + " -d " + local_library;
                cmd = "gunzip -S .zip " + filer;
                fileName = local_library + s_fileName + ".txt";
                system( cmd);
            }
// this renames the .txt to .etx!!
            else  /*if( s_fileName.right(4) == ".txt" ||  if( s_fileName.right(4) == ".TXT"))*/ {
//   odebug << "Filename is "+fileName << oendl;
                s_fileName = fileName;
                s_fileName.replace( s_fileName.length()-3,3,"gtn");//  s_fileName.replace( s_fileName.length()-3,3,"etx");
                rename(fileName.latin1(),s_fileName.latin1());
                fileName = s_fileName;
//  odebug << "Filename is now "+fileName << oendl;
            }
        } else
                        fileName = str;

        FindTitle(fileName);

        QFileInfo fi( fileName);
        name_file = fi.fileName();
        name_file = name_file.left(name_file.length() - 4);

        odebug << "Setting doclink" << oendl;
        DocLnk lnk;
        odebug << "name is " + name_file << oendl;
        lnk.setName(name_file); //sets file name
        odebug << "Title is "+title << oendl;
        lnk.setComment(title);

        QListBox_1->clear();
        getTitles();
        odebug << "Filename is "+fileName << oendl;
        lnk.setFile(fileName); //sets File property
        lnk.setType("guten/plain");// hey is this a REGISTERED mime type?!?!? ;D
        lnk.setExec(fileName);
        lnk.setIcon("gutenbrowser/Gutenbrowser");
        if(!lnk.writeLink())
            odebug << "Writing doclink did not work" << oendl;

   } // end of for each file name....
    OpenFileButton->setDown(FALSE);
}

/*
find the title in the config file */
bool OpenEtext::FindTitle( QString filename)
{
    Config config("Gutenbrowser");
    config.setGroup( "Files" );
    QString s_numofFiles = config.readEntry("NumberOfFiles", "0");
    int  i_numofFiles = s_numofFiles.toInt();

    QFileInfo fi(filename);
    name = fi.fileName();
    odebug << "filename to open is " + name << oendl;
    QFile indexLib( filename);
        bool findCheck = FALSE;
        //        int Titlenumber=0;

    if ( indexLib.open( IO_ReadOnly) ) {
                odebug << "file opened successfully" << oendl;
        QTextStream indexStream( &indexLib );
        QString target = "Project Gutenberg Etext of";
        QString target2 = "Project Gutenberg Etext";
        QString target3 = "Project Gutenberg's Etext of";
        QString target4 = "Project Gutenberg's Etext";
        QString indexLine;
        while ( !indexStream.eof() ) {
              // until end of file..
            indexLine = indexStream.readLine();
            if(  indexLine.find(target, 0, TRUE) > -1 && !findCheck) {findCheck = TRUE;
                title = indexLine.mid( indexLine.find(target, 0, TRUE) + (target.length()) , indexLine.find("\r", 0, TRUE));
                title = title.left( title.find( "*",0, TRUE));
                title = title.stripWhiteSpace ();
//                odebug << "Found the title 1 and it is " << title << "" << oendl;
//                              QListBox_1->insertItem ( title);
            }
            if( indexLine.find( target2, 0, TRUE) > -1 && !findCheck) {
                findCheck = TRUE;
                title = indexLine.mid(  indexLine.find( target2, 0, TRUE ) + ( target2.length()) , indexLine.find("\r", 0, TRUE) );
                title = title.left( title.find( "*",0, TRUE));
                title = title.stripWhiteSpace ();
//                    odebug << "Found the title 2 and it is " << title << "" << oendl;
//                              QListBox_1->insertItem ( title);
            }
            if( indexLine.find( target3, 0, TRUE) > -1 && !findCheck) {
                findCheck = TRUE;
                title = indexLine.mid( indexLine.find( target3, 0, TRUE) + ( target3.length()) , indexLine.find("\r", 0, TRUE));
                title = title.left( title.find( "*",0, TRUE));
                title = title.stripWhiteSpace ();
//                odebug << "Found the title 3 and it is " << title << "" << oendl;
            }
            if( indexLine.find( target4, 0, TRUE) > -1 && !findCheck) {
                findCheck = TRUE;
                title = indexLine.mid(  indexLine.find( target4, 0, TRUE) + ( target4.length()) , indexLine.find("\r", 0, TRUE));
                title = title.left( title.find( "*",0, TRUE));
                title = title.stripWhiteSpace ();
//                odebug << "Found the title 4 and it is " << title << "" << oendl;
            }
        } //endof file
                indexLib.close();

        if( !findCheck || title.length() < 2) {
            odebug << "Trying hard to find title from GUTINDEX.ALL" << oendl;
            title = titleFromLibrary( filename);
            findCheck = TRUE;
        }

        if ( checkConf() == false && findCheck == TRUE) {

            config.setGroup( "Files");
            config.writeEntry("NumberOfFiles",i_numofFiles + 1 );
            QString interger;
            interger.setNum( i_numofFiles +1);
            config.writeEntry(interger, filename);
            config.setGroup( "Titles" );
            for (int i = 1; i <= i_numofFiles; i++) { //find dup titles in config file
                QString temp;
                temp.setNum(i);
                config.setGroup( "Files" );
                QString ramble = config.readEntry(temp, "" );
                config.setGroup( "Titles" );
                QString title1 = config.readEntry(ramble, "");
                if(title1==title)
                    title+="1";
                i= i_numofFiles+1;
            }
            if(title.length()<3) {
//                odebug << "title is empty" << oendl;
                title="Unknown";
            }
            config.writeEntry( filename,title);
        }
    } else {
        QString sMsg;
        sMsg = "Error opening library file: "+filename;
        printf( sMsg+"\n");
    }
    return true;
}

QString OpenEtext::titleFromLibrary( QString fileName)
{
    QFile indexLib( local_index);
    QString target;
    int find1 = fileName.findRev("/",-1,TRUE)  + 1;
    int find2 = fileName.findRev(".gtn",-1,TRUE) - find1;
    if(find2==-1-find1)
    int find2 = fileName.findRev(".etx",-1,TRUE) - find1;
    else if(find2==-1-find1)
    int find2 = fileName.findRev(".txt",-1,TRUE) - find1;

    target = fileName.mid( find1,  find2 );
    QString checker = target.right(1);
    bool ok;
    checker.toInt( &ok,10);
    if( ok) {
        target = target.left( target.length()-1);
        checker = target.right(1);
        ok = FALSE;
        checker.toInt( &ok,10);
        if( ok) {
            target = target.left( target.length()-1);
        }
    }
    if ( indexLib.open( IO_ReadOnly) ) {
          // file opened successfully
        QTextStream indexStream( &indexLib );
        QString indexLine;
        bool findCheck = FALSE;
        while ( !indexStream.atEnd() ) {                  // until end of file..
            indexLine = indexStream.readLine();

            if( indexLine.find( target,0,FALSE) > 0 ) {
                findCheck = TRUE;
                title = indexLine.mid( 9, 50);
//                title = indexLine.mid( 26, indexLine.length() );
                title = title.stripWhiteSpace ();
//                odebug << "Finally Found the title and it is\n " << title << "" << oendl;
//                              QListBox_1->insertItem ( title);
            }
        } //end while loop
    }
    else
       odebug << "Error opening library index "+ local_index << oendl;
    return title;
}

bool OpenEtext::checkConf()
{
    QString file = fileName;
    Config config("Gutenbrowser");
    config.setGroup( "Files" );
    QString s_numofFiles = config.readEntry("NumberOfFiles", "0" );
    int  i_numofFiles = s_numofFiles.toInt();

    for (int i = 1; i <= i_numofFiles; i++) {
        QString temp;
        temp.setNum(i);
        config.setGroup( "Files");
        QString ramble = config.readEntry(temp, "" );

        if(ramble == file ) {
            return true;
        }
    }
    return false;
}

void OpenEtext::remove()
{
    title_text = QListBox_1->text( QListBox_1->currentItem() );
    title_text=title_text.stripWhiteSpace();
    QString msg ="<p>Do you really want to REMOVE\n" +title_text +"?\nThis will not delete the file.</P>";
    switch( QMessageBox::information( this, (tr("Remove Etext")),
                                      (tr(msg)),
                                      (tr("&Yes")), (tr("&Cancel")), 0 )){
        case 0: // Yes clicked,
            removeSelection();
            QListBox_1->clear();
            getTitles();
            break;
        case 1: // Cancel
            break;
    };
}

/*
 this removes selected title entry*/
void OpenEtext::removeSelection()
{
    Config config("Gutenbrowser");
    config.setGroup( "Files" );
    s_numofFiles = config.readEntry("NumberOfFiles", "0");
    int rem=0;
    int  i_numofFiles = s_numofFiles.toInt();
        QString fileNum2;
        QString fileNum;

    for (int i = 1; i <= i_numofFiles; i++) {
        fileNum.setNum(i);
        config.setGroup( "Files" );

        QString s_filename = config.readEntry(fileNum, "" );
        config.setGroup( "Titles" );

        QString file_title = config.readEntry( s_filename, "");
        if(title_text == file_title) {
            rem=i;
//odebug << "file title to remove is "+file_title << oendl;
            selFile = s_filename;
            config.removeEntry( s_filename); //removes file=title
        }
    }
    config.setGroup( "Files" );

    for(int fg = rem; fg < i_numofFiles ; fg++  ) { //this rewrites Files number entry
        fileNum.setNum(fg);
        fileNum2.setNum( fg + 1);

        QString s_filename2 = config.readEntry(fileNum2, "" );

        if (!s_filename2.isEmpty()) {
            config.writeEntry(fileNum, s_filename2 );
        }
    }
    config.writeEntry("NumberOfFiles", i_numofFiles - 1 );
    config.removeEntry(fileNum2);
//    remFile();
}

/*
 removes file title name from list and config file*/
void OpenEtext::remFile()
{
    Config config("Gutenbrowser");
    config.setGroup( "Files" );
    QString remFile;
    s_numofFiles = config.readEntry("NumberOfFiles", "0");
    int  i_numofFiles = s_numofFiles.toInt();
    for (int i = 1; i <= i_numofFiles; i++) {
        QString fileNum;
        fileNum.setNum(i);
        QString s_filename = config.readEntry(fileNum, "" );

        if( s_filename == selFile) {
            config.removeEntry(selFile);

            for(int fg = i_numofFiles - i; fg < i_numofFiles ; fg++  ) { //this rewrites Files number entry
                QString fileNum2;
                fileNum2.setNum( fg + 1);
                QString s_filename2 = config.readEntry(fileNum2, "" );

                if (!s_filename2.isEmpty()) {
                    config.writeEntry(fileNum, s_filename2 );
                }
            }
        }
    }
    config.writeEntry("NumberOfFiles", i_numofFiles - 1 );
    config.removeEntry(s_numofFiles);

    QListBox_1->triggerUpdate(true);
}

/*
sorts the list*/
void OpenEtext::scan() {

    QListBox_1->sort(TRUE);
    QListBox_1->triggerUpdate(true);
}

void  OpenEtext::editTitle() {

    int currentItem=QListBox_1->currentItem();
    QString title_text = QListBox_1->text( currentItem);
//odebug << "Selected "+title_text << oendl;

    Config config("Gutenbrowser");
    config.setGroup( "Files" );
    s_numofFiles = config.readEntry("NumberOfFiles", "0");
    int  i_numofFiles = s_numofFiles.toInt();
    QString fileNum;
    for (int i = 1; i <= i_numofFiles+1; i++) {

        fileNum.setNum(i);
        config.setGroup( "Files" );
        QString s_filename = config.readEntry(fileNum, "" );
        config.setGroup( "Titles" );
        QString file_title = config.readEntry( s_filename, "");
//odebug << "file_title is "+file_title << oendl;
        if(title_text == file_title ) {
            selFile = s_filename;
//odebug << "Edit: "+ file_title << oendl;
            i=i_numofFiles+1;
            Edit_Title *titleEdit;
            titleEdit = new Edit_Title(this,file_title ,TRUE);
            if(titleEdit->exec() !=0) {
//odebug << titleEdit->newTitle << oendl;
                config.writeEntry( s_filename, titleEdit->newTitle);
                QListBox_1->removeItem(currentItem);
                QListBox_1->insertItem ( QPixmap( QPEApplication::qpeDir()+"pics/gutenbrowser/gutenbrowser_sm.png"), titleEdit->newTitle, currentItem);
            }
        }
    }

//    getTitles();
    QListBox_1->triggerUpdate(true);
}

