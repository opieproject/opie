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
//#define QTOPIA_INTERNAL_MIMEEXT
#include "fileBrowser.h"
//#include "inputDialog.h"

#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/fileselector.h>
#include <qpe/qpeapplication.h>
#include <qpe/menubutton.h>
#include <qpe/mimetype.h>

#include <qdict.h>
#include <qwidgetstack.h>
#include <qlistview.h>
#include <qcombo.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <unistd.h>
#include <qpopupmenu.h>
#include <qlineedit.h>
#include <qstringlist.h>

#include <unistd.h>
#include <stdlib.h>

static int u_id = 1;
static int get_unique_id()
{
    return u_id++;
}

fileBrowser::fileBrowser( QWidget* parent,  const char* name, bool modal, WFlags fl , const QString mimeFilter )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
        setName( tr("fileBrowser") );
    setCaption(tr( name ) );
//     mimeType =  mimeFilter;
//     MimeType mt( mimeType);
//     if( mt.extension().isEmpty())
//     QStringList filterList;
//     filterList=QStringList::split(";",mimeFilter,FALSE);
//      for ( QStringList::Iterator it = filterList.begin(); it != filterList.end(); ++it ) {
//             printf( "%s \n", (*it).latin1() );
//         }

      filterStr = mimeFilter.right(mimeFilter.length() - mimeFilter.find("/",0,TRUE) - 1);// "*";
    
    qDebug(filterStr);
//     else
//         filterStr = "*."+ mt.extension();
//      qDebug("description "+mt.description());
//      qDebug( "id "+mt.id());
//      qDebug("extension "+mt.extension());

//      channel = new QCopChannel( "QPE/fileDialog", this );
//      connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
//         this, SLOT(receive(const QCString&, const QByteArray&)) );

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    dirPathCombo = new QComboBox( FALSE, this, "dirPathCombo" );
    dirPathCombo->setEditable(TRUE);

    connect( dirPathCombo, SIGNAL( activated( const QString & ) ),
              this, SLOT( dirPathComboActivated( const QString & ) ) );

    connect( dirPathCombo->lineEdit(), SIGNAL( returnPressed(  ) ),
              this, SLOT( dirPathEditPressed( ) ) );

    dirPathStringList << "/";
// we can get the storage here

    layout->addMultiCellWidget( dirPathCombo, 0, 0, 0, 4 );

    cdUpButton = new QPushButton(Resource::loadIconSet("up"),"",this,"cdUpButton");
    cdUpButton ->setMinimumSize( QSize( 20, 20 ) );
    cdUpButton ->setMaximumSize( QSize( 20, 20 ) );
    connect( cdUpButton ,SIGNAL(released()),this,SLOT( upDir()) );
    cdUpButton ->setFlat(TRUE);
    layout->addMultiCellWidget( cdUpButton, 0, 0, 5, 5 );

    docButton = new QPushButton(Resource::loadIconSet("DocsIcon"),"",this,"docsButton");
    docButton->setMinimumSize( QSize( 20, 20 ) );
    docButton->setMaximumSize( QSize( 20, 20 ) );
    connect( docButton,SIGNAL(released()),this,SLOT( docButtonPushed()) );
    docButton->setFlat(TRUE);
    layout->addMultiCellWidget( docButton, 0, 0, 6, 6 );

    homeButton = new QPushButton( Resource::loadIconSet("home"),"",this,"homeButton");
    homeButton->setMinimumSize( QSize( 20, 20 ) );
    homeButton->setMaximumSize( QSize( 20, 20 ) );
    connect(homeButton,SIGNAL(released()),this,SLOT(homeButtonPushed()) );
    homeButton->setFlat(TRUE);
    layout->addMultiCellWidget( homeButton, 0, 0, 7, 7 );

    FileStack = new QWidgetStack( this );

    ListView = new QListView( this, "ListView" );
//     ListView->setMinimumSize( QSize( 100, 25 ) );
    ListView->addColumn( tr( "Name" ) );
    ListView->setColumnWidth(0,120);
    ListView->setSorting( 2, FALSE);
    ListView->addColumn( tr( "Size" ) );
    ListView->setColumnWidth(1,-1);
    ListView->addColumn( tr("Date"),-1);

    ListView->setColumnWidthMode(0,QListView::Manual);
    ListView->setColumnAlignment(1,QListView::AlignRight);
    ListView->setColumnAlignment(2,QListView::AlignRight);
    ListView->setAllColumnsShowFocus( TRUE );

    QPEApplication::setStylusOperation( ListView->viewport(),QPEApplication::RightOnHold);
    connect( ListView, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
             this, SLOT( ListPressed(int, QListViewItem *, const QPoint&, int)) );

    connect( ListView, SIGNAL( clicked( QListViewItem*)), SLOT(listClicked(QListViewItem *)) );

    FileStack->addWidget( ListView, get_unique_id() );

    fileSelector = new FileSelector( mimeType, FileStack, "fileselector" , FALSE, FALSE); //buggy
//    connect( fileSelector, SIGNAL( closeMe() ), this, SLOT( showEditTools() ) );
//    connect( fileSelector, SIGNAL( newSelected( const DocLnk &) ), this, SLOT( newFile( const DocLnk & ) ) );
    connect( fileSelector, SIGNAL( fileSelected( const DocLnk &) ),
             this, SLOT( docOpen( const DocLnk & ) ) );
    layout->addMultiCellWidget( FileStack, 1, 1, 0, 7 );

    SelectionCombo = new QComboBox( FALSE, this, "SelectionCombo" );
    SelectionCombo->insertItem( tr( "Documents" ) );
    SelectionCombo->insertItem( tr( "All files" ) );
    SelectionCombo->insertItem( tr( "Hidden files" ) );
//      SelectionCombo->setMaximumWidth(120);
    layout->addMultiCellWidget( SelectionCombo, 2, 2, 0, 3 );

    connect( SelectionCombo, SIGNAL( activated( const QString & ) ),
            this, SLOT( selectionChanged( const QString & ) ) );

    typemb = new MenuButton(this);
    typemb->setLabel(tr("Type: %1"));
    typemb->setMinimumWidth(110);
    typemb->setFixedHeight(22);
    layout->addMultiCellWidget( typemb, 2, 2, 4, 7 );
    updateMimeTypeMenu() ;

    currentDir.setPath(QDir::currentDirPath());
    currentDir.setFilter( QDir::Files | QDir::Dirs/* | QDir::Hidden */| QDir::All);
    currentDir.setNameFilter(filterStr);

    populateList();
    move(0,15);
}

fileBrowser::~fileBrowser()
{
}

void fileBrowser::setFileView( int selection )
{
    SelectionCombo->setCurrentItem( selection );
    selectionChanged( SelectionCombo->currentText() );
}

void fileBrowser::populateList()
{
    ListView->clear();
    QListViewItem * item;
    bool isDir=FALSE;
//qDebug(currentDir.canonicalPath());
    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentDir.setMatchAllDirs(TRUE);

//    currentDir.setNameFilter("*.txt;*.etx");
    QString fileL, fileS, fileDate;
    const QFileInfoList *list = currentDir.entryInfoList( /*QDir::All*/ /*, QDir::SortByMask*/);
    QFileInfoListIterator it(*list);
    QFileInfo *fi;
    while ( (fi=it.current()) ) {
            if (fi->isSymLink() ){
                QString symLink=fi->readLink();
//         qDebug("Symlink detected "+symLink);
                QFileInfo sym( symLink);
                fileS.sprintf( "%10li", sym.size() );
                fileL.sprintf( "%s ->  %s",  sym.fileName().data(),sym.absFilePath().data() );
                fileDate = sym.lastModified().toString();
            } else {
//        qDebug("Not a dir: "+currentDir.canonicalPath()+fileL);
                fileS.sprintf( "%10li", fi->size() );
                fileL.sprintf( "%s",fi->fileName().data() );
                fileDate= fi->lastModified().toString();
                if( QDir(QDir::cleanDirPath(currentDir.canonicalPath()+"/"+fileL)).exists() ) {
                    fileL+="/";
                    isDir=TRUE;
//     qDebug( fileL);
                }
            }
            if(fileL !="./" && fi->exists()) {
                item= new QListViewItem( ListView,fileL,fileS , fileDate);
                QPixmap pm;
         
                if(isDir || fileL.find("/",0,TRUE) != -1) {
                    if( !QDir( fi->filePath() ).isReadable()) 
                        pm = Resource::loadPixmap( "lockedfolder" );
                    else 
                        pm= Resource::loadPixmap( "folder" );
                    item->setPixmap( 0,pm );
                } else {
                    if( !fi->isReadable() )
                        pm = Resource::loadPixmap( "locked" );
                    else {
                        MimeType mt(fi->filePath());
                        pm=mt.pixmap();// sets the pixmap for the mimetype
                        if(pm.isNull())
                            pm =  Resource::loadPixmap( "UnknownDocument-14" );
                        item->setPixmap( 0,pm);
                    }
                }
                if(  fileL.find("->",0,TRUE) != -1) {
                      // overlay link image
                    pm= Resource::loadPixmap( "folder" );
                    QPixmap lnk = Resource::loadPixmap( "symlink" );
                    QPainter painter( &pm );
                    painter.drawPixmap( pm.width()-lnk.width(), pm.height()-lnk.height(), lnk );
                    pm.setMask( pm.createHeuristicMask( FALSE ) );
                    item->setPixmap( 0, pm);
                }
            }
            isDir=FALSE;
            ++it;
//         }
    }
    ListView->setSorting( 3, FALSE);
    QString currentPath = currentDir.canonicalPath();

    fillCombo( (const QString &)currentPath);
//   dirPathCombo->lineEdit()->setText(currentPath);

//      if( dirPathStringList.grep(currentPath,TRUE).isEmpty() ) {
//          dirPathCombo->clear();
//          dirPathStringList.prepend(currentPath );
//          dirPathCombo->insertStringList( dirPathStringList,-1);
//      }
}

void fileBrowser::upDir()
{
    QString current = currentDir.canonicalPath();
    QDir dir(current);
    dir.cdUp();
    current = dir.canonicalPath();
    chdir( current.latin1() );
    currentDir.cd(  current, TRUE);
    populateList();
    update();
}

// you may want to switch these 2 functions. I like single clicks
void fileBrowser::listClicked(QListViewItem *selectedItem)
{
    if(selectedItem) {
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

void fileBrowser::homeButtonPushed() {
    QString current = QDir::homeDirPath();
    chdir( current.latin1() );
    currentDir.cd(  current, TRUE);
    populateList();
    update();
}

void fileBrowser::docButtonPushed() {
    QString current = QPEApplication::documentDir();
    chdir( current.latin1() );
    currentDir.cd( current, TRUE);
    populateList();
    update();

}

void fileBrowser::selectionChanged( const QString &select )
{
    if ( select == tr("Documents"))  {
        FileStack->raiseWidget( fileSelector );
        dirPathCombo->hide();
        cdUpButton->hide();
        docButton->hide();
        homeButton->hide();
    } else {
        if ( select == tr("All files") )
            currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::All);
        else
            currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);

        populateList();
        update();
        dirPathCombo->show();
        cdUpButton->show();
        docButton->show();
        homeButton->show();
        FileStack->raiseWidget( ListView );
    }
}

void fileBrowser::docOpen( const DocLnk &doc )
{
    fileList.append( doc.file().latin1() );
    accept();
}

void fileBrowser::ListPressed( int mouse, QListViewItem *item, const QPoint &point, int i)
{
    switch (mouse) {
      case 1:
          break;
      case 2:
          showListMenu(item);
    break;
    };
}

void fileBrowser::showListMenu(QListViewItem *item) {

    QPopupMenu  m;// = new QPopupMenu( Local_View );
    if(item) {
        if( item->text(0).find("/",0,TRUE))
            m.insertItem( tr( "Change Directory" ), this, SLOT( doCd() ));
        m.insertItem( tr( "Make Directory" ), this, SLOT( makDir() ));
        m.insertItem( tr( "Rescan" ), this, SLOT( populateList() ));
        m.insertItem( tr( "Rename" ), this, SLOT( localRename() ));
        m.insertSeparator();
        m.insertItem( tr( "Delete" ), this, SLOT( localDelete() ));
    } else {
        m.insertItem( tr( "Make Directory" ), this, SLOT( makDir() ));
        m.insertItem( tr( "Rescan" ), this, SLOT( populateList() ));
 
    }
    m.exec( QCursor::pos() );
}

void fileBrowser::doCd() {
    listClicked( ListView->currentItem());
}

void fileBrowser::makDir() {
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Make Directory"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
       QString  filename = fileDlg->LineEdit1->text();
       qDebug("Make dir");
       currentDir.mkdir( currentDir.canonicalPath()+"/"+filename);
    }
    populateList();
}

void fileBrowser::localRename() {
    QString curFile = ListView->currentItem()->text(0);
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Rename"),TRUE, 0);
    fileDlg->setTextEdit((const QString &) curFile);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        QString oldname =  currentDir.canonicalPath() + "/" + curFile;
        QString newName =  currentDir.canonicalPath() + "/" + fileDlg->LineEdit1->text();//+".playlist";
        if( rename(oldname.latin1(), newName.latin1())== -1)
            QMessageBox::message(tr("Note"),tr("Could not rename"));
    }
    populateList();
}

void fileBrowser::localDelete() {
    QString f = ListView->currentItem()->text(0);
    if(QDir(f).exists() ) {
        switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f+
                                      tr(" ?\nIt must be empty"),tr("Yes"),tr("No"),0,0,1) ) {
          case 0: {
              f=currentDir.canonicalPath()+"/"+f;
              QString cmd="rmdir "+f;
              system( cmd.latin1());
              populateList();
          }
              break;
          case 1:
                // exit
              break;
        };
    } else {
        switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+f
                                      +" ?",tr("Yes"),tr("No"),0,0,1) ) {
          case 0: {
              f=currentDir.canonicalPath()+"/"+f;
              QString cmd="rm "+f;
              system( cmd.latin1());
              populateList();
          }
              break;
          case 1:
                // exit
              break;
        };
    }
}

void fileBrowser::updateMimeTypeMenu() {

    disconnect( typemb, SIGNAL(selected(const QString&)),
          this, SLOT(showType(const QString&)) );

    QString prev;

    // Type filter
    QStringList types;
    types << tr("All");
    types << "--";
    types += getMimeTypes();
    prev = typemb->currentText();
    typemb->clear();
    typemb->insertItems(types);
      //  typemb->select(prev);

    connect(typemb, SIGNAL(selected(const QString&)), this, SLOT(showType(const QString&)));
}

void fileBrowser::showType(const QString &t) {

    qDebug(t);
    if(t.find(tr("All"),0,TRUE) != -1) {
        filterStr =  "*";
    } else {
        QStringList list =  mimetypes.grep( t,TRUE);
        QString ext;     
        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            mimeType =(*it);
            MimeType mt( mimeType);
//            qDebug("mime "+mimeType);
//         qDebug("description "+mt.description());
//         qDebug( "id "+mt.id());
//            qDebug("extension "+mt.extension());
//             if( mt.extension().isEmpty())
                filterStr =  "*";
//             else
//                 filterStr = "*."+ mt.extension()+" ";
//         printf( "%s \n", (*it).latin1() );
        }
    }
    currentDir.setNameFilter(filterStr);

    populateList();
    update();
//      if(fileSelector) {
//      disconnect( fileSelector, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( docOpen( const DocLnk & ) ) );
//          delete fileSelector;
      //  }
      //   fileSelector = new FileSelector( mimeType, FileStack, "fileselector" , FALSE, FALSE); //buggy
//    connect( fileSelector, SIGNAL( closeMe() ), this, SLOT( showEditTools() ) );
//    connect( fileSelector, SIGNAL( newSelected( const DocLnk &) ), this, SLOT( newFile( const DocLnk & ) ) );
      //   connect( fileSelector, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( docOpen( const DocLnk & ) ) );
//    fileSelector->reread();
//      if ( t == tr("All") ) {
//          icons->setTypeFilter("",TRUE);
//      } else {
//          icons->setTypeFilter(t+"/*",TRUE);
//      }

}

QStringList fileBrowser::getMimeTypes() {

    QStringList r;
    AppLnkSet apps( QPEApplication::qpeDir() + "apps" );
    QFile file( QPEApplication::qpeDir()+"etc/available.mime");
    file.open( IO_WriteOnly|IO_Truncate);//)
    for ( QListIterator<AppLnk> it( apps.children() ); it.current(); ++it ) {
        AppLnk* l;
        l = it.current();
        QStringList maj = l->mimeTypes();
        QStringList::ConstIterator f;
        for (  f = maj.begin(); f != maj.end(); f++ ) {
            QString  temp = *f;
            mimetypes << temp;
            int sl = temp.find('/');
            if (sl >= 0) {
                QString k = temp.left(sl);
                if( r.grep(k,TRUE).isEmpty() ) {
                    r << k;
                    k+="\n";
                    file.writeBlock(  k.latin1(), k.length());
                }
            }
        }
    }
    r.sort();
    file.close();
    return r;
}

void fileBrowser::receive( const QCString &msg, const QByteArray &data ) {
//    QDataStream stream( data, IO_ReadOnly );
//    if (msg == "keyRegister(int key, QString channel, QString message)")
//    {
//      int k;
//      QString c, m;
//      stream >> k;
//      stream >> c;
//      stream >> m;
}

void fileBrowser::dirPathComboActivated( const QString & current) {
    chdir( current.latin1() );
    currentDir.cd( current, TRUE);
    populateList();
    update();
}

void fileBrowser::dirPathEditPressed() {
    QString current = dirPathCombo->lineEdit()->text();
    chdir( current.latin1() );
    currentDir.cd( current, TRUE);
    populateList();
    update();
}

void fileBrowser::fillCombo(const QString &currentPath) {

    dirPathCombo->lineEdit()->setText(currentPath);

    if( dirPathStringList.grep(currentPath,TRUE).isEmpty() ) {
        dirPathCombo->clear();
        dirPathStringList.prepend(currentPath );
        dirPathCombo->insertStringList( dirPathStringList,-1);
    }
}


InputDialog::InputDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( tr("InputDialog") );
    resize( 234, 50 ); 
    setMaximumSize( QSize( 240, 50 ) );
    setCaption( tr(name ) );

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 10, 10, 216, 22 ) ); 
}

InputDialog::~InputDialog()
{
    inputText= LineEdit1->text();
  
}

void InputDialog::setTextEdit(const QString &string) {
    LineEdit1->setText(string);
}
