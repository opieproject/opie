/****************************************************************************
** copyright 2001 ljp ljp@llornkcor.com
** Created: Fri Dec 14 08:16:46 2001
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
#include "fileBrowser.h"
#include <qpe/config.h>

#include <qlistview.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <unistd.h>

fileBrowser::fileBrowser( QWidget* parent,  const char* name, bool modal, WFlags fl , const QString filter )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "fileBrowser" );
    resize( 236, 280 );
    setCaption(tr( "Browse for file" ) );
    filterStr=filter;
    dirLabel = new QLabel(this, "DirLabel");
    dirLabel->setText(currentDir.canonicalPath());
    dirLabel->setGeometry(10,4,230,30);
    ListView = new QListView( this, "ListView" );
    ListView->addColumn( tr( "Name" ) );
    ListView->setColumnWidth(0,140);
    ListView->setSorting( 2, FALSE);
    ListView->addColumn( tr( "Size" ) );
    ListView->setColumnWidth(1,59);
//      ListView->addColumn( tr( "" ) );
//      ListView->setColumnWidth(1,59);
//      ListView->setMultiSelection(true);
//      ListView->setSelectionMode(QListView::Extended);

    ListView->setAllColumnsShowFocus( TRUE );
    ListView->setGeometry( QRect( 10, 35, 220, 240 ) );

    // signals and slots connections
    connect( ListView, SIGNAL(doubleClicked( QListViewItem*)), SLOT(listDoubleClicked(QListViewItem *)) );
    connect( ListView, SIGNAL(pressed( QListViewItem*)), SLOT(listClicked(QListViewItem *)) );
    currentDir.setPath(QDir::currentDirPath());
    populateList();
}

fileBrowser::~fileBrowser()
{
}


void fileBrowser::populateList()
{
    ListView->clear();
//qDebug(currentDir.canonicalPath());
    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );
    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentDir.setMatchAllDirs(TRUE);

    currentDir.setNameFilter(filterStr);
//    currentDir.setNameFilter("*.txt;*.etx");
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
//        qDebug("Not a dir: "+currentDir.canonicalPath()+fileL);
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

void fileBrowser::upDir()
{
//    qDebug(currentDir.canonicalPath());
}

void fileBrowser::listDoubleClicked(QListViewItem *selectedItem)
{
}

// you may want to switch these 2 functions. I like single clicks
void fileBrowser::listClicked(QListViewItem *selectedItem)
{
    QString strItem=selectedItem->text(0);
    QString strSize=selectedItem->text(1);
//    qDebug("strItem is "+strItem);
    strSize.stripWhiteSpace();
//  qDebug(strSize);

        if(strItem.find("@",0,TRUE) !=-1 || strItem.find("->",0,TRUE) !=-1 ) { //if symlink
              // is symlink
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
            } else {
                strItem=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem);
                  if( QFile::exists(strItem ) ) {
//currentDir.canonicalPath()
                      qDebug("We found our files!!"+strItem);
                    OnOK();
                  }
                } //end not symlink
        chdir(strItem.latin1());
      }
}

void fileBrowser::OnOK()
{
  QListViewItemIterator it1( ListView);
    for ( ; it1.current(); ++it1 ) {
      if ( it1.current()->isSelected() ) {
         selectedFileName=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+it1.current()->text(0));
         qDebug("selected filename is "+selectedFileName);
       fileList.append( selectedFileName );
      }
    }
  accept();
}
