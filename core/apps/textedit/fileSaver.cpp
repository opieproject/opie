/****************************************************************************
** copyright 2001 ljp ljp@llornkcor.com
** Created: Fri Dec 14 08:16:46 2001 fileSaver.cpp
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "fileSaver.h"
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qlistview.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <qlineedit.h>

fileSaver::fileSaver( QWidget* parent,  const char* name, bool modal, WFlags fl , const QString currentFileName )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "fileSaver" );
    resize( 236, 280 );
    setCaption(tr( "Save file" ) );
    QFileInfo fi(currentFileName);
    QString tmpFileName=fi.fileName();
//    qDebug( tmpFileName);

    dirLabel = new QLabel(this, "DirLabel");
    dirLabel->setText(currentDir.canonicalPath());
    dirLabel->setGeometry(10,4,230,30);

    ListView = new QListView( this, "ListView" );
    ListView->addColumn( tr( "Name" ) );
    ListView->setColumnWidth(0,140);
    ListView->setSorting( 2, FALSE);
    ListView->addColumn( tr( "Size" ) );
    ListView->setColumnWidth(1,59);
    ListView->setColumnWidthMode(0,QListView::Manual);
    ListView->setColumnAlignment(1,QListView::AlignRight);
//      ListView->setMultiSelection(true);
//      ListView->setSelectionMode(QListView::Extended);

    ListView->setAllColumnsShowFocus( TRUE );
    ListView->setGeometry( QRect( 10, 35, 220, 160 ) );

    fileEdit= new QLineEdit(this);
    fileEdit->setGeometry( QRect( 10, 200, 200, 22));

    fileEdit->setText( tmpFileName);

      // signals and slots connections
    connect( ListView, SIGNAL(doubleClicked( QListViewItem*)), SLOT(listDoubleClicked(QListViewItem *)) );
    connect( ListView, SIGNAL(pressed( QListViewItem*)), SLOT(listClicked(QListViewItem *)) );

//      tmpFileName=fi.FilePath();
//      qDebug( tmpFileName);
    currentDir.setPath( QDir::currentDirPath() );
    populateList();
}

fileSaver::~fileSaver()
{
}


void fileSaver::populateList()
{
    ListView->clear();
    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );
    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentDir.setMatchAllDirs(TRUE);

    currentDir.setNameFilter("*");
    QString fileL, fileS;
    const QFileInfoList *list = currentDir.entryInfoList(QDir::All /*, QDir::SortByMask*/);
    QFileInfoListIterator it(*list);
    QFileInfo *fi;
    while ( (fi=it.current()) ) {

        if (fi->isSymLink() ){
            QString symLink=fi->readLink();
//         qDebug("Symlink detected "+symLink);
         QFileInfo sym( symLink);
            fileS.sprintf( "%10li", sym.size() );
            fileL.sprintf( "%s ->  %s",  sym.fileName().data(),sym.absFilePath().data() );

        } else {
//  //        qDebug("Not a dir: "+currentDir.canonicalPath()+fileL);
            fileS.sprintf( "%10li", fi->size() );
            fileL.sprintf( "%s",fi->fileName().data() );
            if( QDir(QDir::cleanDirPath(currentDir.canonicalPath()+"/"+fileL)).exists() ) {
                fileL+="/";
//     qDebug(currentDir.canonicalPath()+fileL);
            }
        }
        item= new QListViewItem( ListView,fileL,fileS );
        ++it;
    }
    ListView->setSorting( 2, FALSE);
    dirLabel->setText("Current Directory:\n"+currentDir.canonicalPath());


}

void fileSaver::upDir()
{
//    qDebug(currentDir.canonicalPath());
}

void fileSaver::listDoubleClicked(QListViewItem *selectedItem)
{
}

void fileSaver::listClicked(QListViewItem *selectedItem)
{
    QString strItem=selectedItem->text(0);
    QString strSize=selectedItem->text(1);
//  qDebug("strItem is "+strItem);
    strSize.stripWhiteSpace();
//  qDebug(strSize);

        if(strItem.find("@",0,TRUE) !=-1 || strItem.find("->",0,TRUE) !=-1 ) { //if symlink
            QString strItem2=strItem.right( (strItem.length()-strItem.find("->",0,TRUE)) -4);
//            qDebug("strItem symlink is "+strItem2);
            if(QDir(strItem2).exists() ) {
                currentDir.cd(strItem2, TRUE);
                populateList();
            }
        } else { // not a symlink
            if(strItem.find(". .",0,TRUE) && strItem.find("/",0,TRUE)!=-1 ) {
                if(QDir(QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem)).exists() ) {
                    strItem=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem);
                    currentDir.cd(strItem,FALSE);
//       qDebug("Path is "+strItem);
                    populateList();
                } else {
                    currentDir.cdUp();
                    populateList();
                }
                if(QDir(strItem).exists()){
                    currentDir.cd(strItem, TRUE);
                    populateList();
                }
            } // else
//                    if( QFile::exists(strItem ) ) {
//                        qDebug("We found our files!!");

//                      OnOK();
                } //end not symlink
        chdir(strItem.latin1());


}


void fileSaver::closeEvent( QCloseEvent *e )
{
    if(e->isAccepted()) {
    e->accept();
    } else {
    qDebug("not accepted");
    done(-1);
    }
}

void fileSaver::accept() {
    selectedFileName =  fileEdit->text();
    QString path =  currentDir.canonicalPath()+"/" + selectedFileName;
    if( path.find("//",0,TRUE) ==-1 ) {
        selectedFileName = path;
    } else {
        selectedFileName = currentDir.canonicalPath()+selectedFileName;
    }
    qDebug("going to save "+selectedFileName);
    done(1);
}
