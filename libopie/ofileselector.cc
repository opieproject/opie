/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-        
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB. 
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qnamespace.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qwidgetstack.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qheader.h>
#include <qdir.h>
#include <qpainter.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qstringlist.h>
#include <qmessagebox.h>

#include <qpe/qpeapplication.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/mimetype.h>
#include <qpe/resource.h>
#include <qpe/storage.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "ofileselector.h"

QMap<QString,QPixmap> *OFileSelector::m_pixmaps = 0;

namespace {

  int indexByString( const QComboBox *box, const QString &str ){
    int index= -1;
    for(int i= 0; i < box->count(); i++ ){
      qWarning("str T%sT  boxT%sT", str.latin1(), box->text(i).latin1() );
      if( str == box->text(i ) ){
    index= i;
    break;
      }
    }
    return index;
  }

};


OFileSelector::OFileSelector(QWidget *wid, int mode, int selector, const QString &dirName,
           const QString &fileName, const QStringList &mimetypes ) : QWidget( wid )
{
  if(wid!=0)
    resize(wid->width(),wid->height());
  m_selector = selector;
  m_currentDir = dirName;
  m_name = fileName;
   m_mimetypes = mimetypes;
//   if( mimetypes.isEmpty() )
//     m_autoMime = true;

  m_mode = mode;
  m_shTool = true;
  m_shPerm = true;
  m_shLne = true;
  m_shChooser = true;
  m_shYesNo = true;
  // for FILESELECTOR only view is interesting
  m_location = 0;
  m_homeButton = 0;
  m_docButton = 0;
  m_hideButton = 0;
  m_ok = 0;
  m_cancel = 0;
  m_reread = 0;
  m_up = 0;
  m_View = 0;
  m_select = 0;
  m_stack = 0;

  m_select = 0;
  m_stack = 0;
  m_lay = 0;
  m_boxToolbar = 0;
  m_boxOk = 0;
  m_edit = 0;

  m_fnLabel = 0;
  m_checkPerm = 0;
  m_mimeCheck = 0;
  m_viewCheck = 0;

  m_pseudo = 0;
  m_pseudoLayout = 0;

  m_dir = true;
  m_files = true;
  m_custom = 0;
  m_showPopup = true;

  if(m_pixmaps == 0 ) // init the pixmaps
    initPics();

  m_lay = new QVBoxLayout(this);
  init();
  m_edit->setText( fileName );
}

void OFileSelector::initPics()
{
  qWarning("init pics" );
  m_pixmaps = new QMap<QString,QPixmap>;
  QPixmap pm  = Resource::loadPixmap( "folder"  );
  QPixmap lnk = Resource::loadPixmap( "opie/symlink"  );
  QPainter painter( &pm );
  painter.drawPixmap( pm.width()-lnk.width(), pm.height()-lnk.height(), lnk );
  pm.setMask( pm.createHeuristicMask( FALSE ) );
  m_pixmaps->insert("dirsymlink", pm );

  QPixmap pm2 = Resource::loadPixmap( "lockedfolder" );
  QPainter pen(&pm2 );
  pen.drawPixmap(pm2.width()-lnk.width(), pm2.height()-lnk.height(), lnk );
  pm2.setMask( pm2.createHeuristicMask( FALSE ) );
  m_pixmaps->insert("symlinkedlocked", pm2 );

}

// let's initialize the gui
/**
   --------------------
   | cmbBox   Button  |
   --------------------
   | FileSlector      |
   |       or         |
   | OSelector        |
   |                  |
   |                  |
   ____________________
   | LineEdit         |
   ____________________
   | Permission Bar   |
   ____________________
   | ViewChoose       |
   ____________________
   | Save       Cancel|
   ____________________
 */
void OFileSelector::delItems()
{
  QLayoutIterator it = m_lay->iterator();
  while ( it.current() != 0 ){
    it.deleteCurrent();
  }
}

void OFileSelector::init()
{

  m_stack = new QWidgetStack(this, "wstack" );
  if( m_selector == NORMAL ){
    QString currMime;
    if( m_mimeCheck != 0 )
      currMime = m_mimeCheck->currentText();

    updateMimes();
    m_select = new FileSelector( currMime == "All" ? QString::null : currMime , m_stack, "fileselector", FALSE, FALSE );
    m_stack->addWidget(m_select, NORMAL );
    m_lay->addWidget(m_stack );
    m_stack->raiseWidget(NORMAL );
    connect(m_select, SIGNAL(fileSelected( const DocLnk &) ), this, SLOT(slotFileBridgeSelected(const DocLnk &) ) );
    m_pseudoLayout = 0l;
  }else {
    initializeListView();
  }

  if(m_shLne ){ 
    initializeName();
  }

  if(m_shPerm ){
    m_checkPerm = new QCheckBox(tr("Set Permission"), this, "Permission" );
    m_checkPerm->setChecked( false );
    m_lay->addWidget(m_checkPerm ); 
  }

  if( m_shChooser )
    initializeChooser();

  if(m_shYesNo )
    initializeYes();
 
  
}

void OFileSelector::setYesCancelVisible( bool show )
{
  if ( show == m_shYesNo )
    return;
  m_shYesNo = show;
  if( !show ){
    delete m_ok;
    delete m_cancel; 
    m_ok = 0;
    m_cancel = 0;
    // delete m_boxOk; all ready deleted in delItems
  }
  updateLay(); // recreate it and save the other states
}

void OFileSelector::setToolbarVisible( bool show )
{
  if ( m_shTool == show )
    return;
  if(!m_shTool ){
    delete m_boxToolbar;
    delete m_homeButton;
    delete m_docButton;
    delete m_location;
    delete m_up;
    m_boxToolbar = 0;
    m_homeButton = 0;
    m_docButton = 0;
    m_location = 0;
    m_up = 0;
  };
  updateLay();// overkill fix it
}

void OFileSelector::setPermissionBarVisible( bool show )
{
  if( show == m_shPerm )
    return;

  m_shPerm = show;

  updateLay();
}

void OFileSelector::setLineEditVisible( bool show )
{
  if( show == m_shLne )
    return;

  m_shLne = show;
  if( !show ){
    delete m_edit;
    delete m_fnLabel;
    m_edit = 0;
    m_fnLabel = 0;
    //delete m_boxName; will be deleted 
  }
  updateLay();
}

void OFileSelector::setChooserVisible( bool show )
{
  if( show = m_shChooser )
    return;
  m_shChooser = show;
  if( !show ){
    delete m_mimeCheck;
    delete m_viewCheck;
    m_mimeCheck = 0;
    m_viewCheck = 0;
  }
  updateLay();
}

QCheckBox* OFileSelector::permissionCheckbox( )
{
  return m_checkPerm;
}

void OFileSelector::setCaseSensetive( bool caSe )
{
  m_case = caSe;
  reparse();
}

void OFileSelector::setShowFiles(bool files ){
  m_files = files;
  reparse();
}

void OFileSelector::setPopupMenu(QPopupMenu *pop )
{
  //delete oldpopup;
  m_custom = pop;
}

bool OFileSelector::setPermission( ) const
{
  if( m_checkPerm == 0 )
    return false;
  else
    return m_checkPerm->isChecked(); 
}

void OFileSelector::setPermissionChecked( bool check )
{
  if( m_checkPerm == 0 )
    return;
  m_checkPerm->setChecked( check );
}

QString OFileSelector::selectedName( )const
{
  QString string;
  if( m_selector == NORMAL ){
    const DocLnk *lnk = m_select->selected();
    string = lnk->file();
  }else if(m_selector == EXTENDED || m_selector == EXTENDED_ALL ) {
    QListViewItem *item = m_View->currentItem();
    if(item != 0 ){
      string = m_currentDir + "/" + item->text( 1 );
    }
  }
  return string;
}

QStringList OFileSelector::selectedNames()const
{
  QStringList list;
  return list;
}

DocLnk OFileSelector::selectedDocument( )const
{
  DocLnk lnk;
  return lnk;
}
void OFileSelector::updateLay()
{
  /* if( m_shTool )
    //
  else
    // hide
  */
  // save the state
  bool check = false;
  if( m_checkPerm != 0 )
    check = m_checkPerm->isChecked();
  QString text;

  if( m_edit != 0 )
    text = m_edit->text();
  // save current mimetype

  delItems();
  delete m_checkPerm;
  m_checkPerm = 0;
  delete m_edit;
  m_edit = 0;
  delete m_fnLabel;
  m_fnLabel = 0;
  delete m_ok;
  m_ok = 0;
  delete m_cancel;
  m_cancel = 0;
  delete m_mimeCheck;
  m_mimeCheck = 0;
  delete m_viewCheck;
  m_viewCheck = 0;
  delete m_select; // test
  delete m_stack;
  //delete m_list;
  init();
  if( m_shLne )
    m_edit->setText(text );
  if( m_shPerm )
    m_checkPerm->setChecked(check );
}

// let's update the mimetypes. Use the current mimefilter for the 2nd QDir retrieve
// insert QListViewItems with the right options
bool OFileSelector::compliesMime(const QString &path, const QString &mime )
{
  if( mime == "All" )
    return true;
  MimeType type( path );
  if( type.id() == mime )
    return true;
  return false;
}

void OFileSelector::reparse()
{
  if(m_View== 0 || m_selector == NORMAL)
    return;

  m_View->clear();
 
  
  QString currMime =m_mimeCheck->currentText();
  // update the mimetype now
  if( m_autoMime ) {
    QDir dir( m_currentDir );
    m_mimetypes.clear();
    m_mimeCheck->clear();
    dir.setFilter( QDir::Files | QDir::Readable );
    dir.setSorting(QDir::Size ); 
    const QFileInfoList *list = dir.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while( (fi=it.current())  ){
      if(fi->extension() == QString::fromLatin1("desktop") ){
  ++it;
  continue;
      }
      MimeType type(fi->filePath() );
      if( !m_mimetypes.contains( type.id() ) )
  m_mimetypes.append( type.id() );

      ++it;
    }
    m_mimetypes.prepend("All" );
    m_mimeCheck->insertStringList(m_mimetypes );
    // set it to the current mimetype
    m_mimeCheck->setCurrentItem( indexByString( m_mimeCheck, currMime ) ); 
  };
  
  QDir dir( m_currentDir );
  //dir.setFilter(-1 );
  int sort = QDir::Name | QDir::DirsFirst | QDir::Reversed;
  if( m_case )
    sort = QDir::IgnoreCase;
  dir.setSorting( sort  );
  
  int filter;
  /*  if( m_dir && !m_files)
      filter |= QDir::Dirs;
      else if( !m_dir && m_files )
      filter |= QDir::Files;
      else
      filter |= QDir::All;
  */
  if( m_selector == EXTENDED_ALL )
    filter = QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All;
  else
    filter = QDir::Files | QDir::Dirs | QDir::All;
  dir.setFilter( filter );

  const QFileInfoList *list = dir.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  while( (fi=it.current())  ){
    if(fi->fileName() == ".." || fi->fileName() == "." ){
      ++it;
      continue;
    }
    qWarning("Test:  %s", fi->fileName().latin1() );
    if(fi->isSymLink() ){
      qWarning("Symlink %s", fi->fileName().latin1() );
      QString file = fi->dirPath(true)+"/"+ fi->readLink();
      qWarning("File ->%s", file.latin1() );
      for(int i=0; i<=4; i++ ){ // prepend from dos
  QFileInfo info( file );
  if( !info.exists() ){
    qWarning("does not exist" );
    addSymlink(currMime,  fi, TRUE );
    break;
  }else if( info.isDir() ){ 
    qWarning("isDir" );
    addDir(currMime, fi, TRUE  );
    break;
  }else if( info.isFile() ){
    qWarning("isFile" );
    addFile(currMime, fi, TRUE );
    break;
  }else if( info.isSymLink() ){
    file = info.dirPath(true)+ "/"+ info.readLink();
    qWarning("isSymlink again %s", file.latin1() );
  }else if( i == 4 ){ // just insert it and have the symlink symbol
    addSymlink(currMime, fi );
    qWarning("level too deep" );
  }
      }
    }else if( fi->isDir() ){
      addDir(currMime, fi );
    }else if( fi->isFile() ) { // file ?
      addFile(currMime, fi );
    }
    ++it;  
  }
  m_View->sort();
}

QString OFileSelector::directory()const
{
    QDir d( m_currentDir);
  return d.absPath();
}

int OFileSelector::fileCount()
{
  return 0;
}

void OFileSelector::slotOk( )
{
  emit ok();
}

void OFileSelector::slotCancel( )
{
  emit cancel();
}

void OFileSelector::initializeName()
{
  m_boxName = new QHBoxLayout(this );
  m_edit = new QLineEdit(this );
  m_fnLabel = new QLabel(this );
  m_fnLabel->setText(tr("Name:") );
  m_boxName->addWidget(m_fnLabel );
  m_boxName->insertSpacing(1, 8 );
  m_boxName->addWidget(m_edit, 100 );

  m_lay->addLayout(m_boxName);
}

void OFileSelector::initializeYes()
{
  m_ok = new QPushButton("&Save", this, "save" );
  m_cancel = new QPushButton("C&ancel", this, "cancel" );
  m_boxOk = new QHBoxLayout(this );
  m_boxOk->addWidget( m_ok, Qt::AlignHCenter );
  m_boxOk->insertSpacing(1, 8 );
  m_boxOk->addWidget( m_cancel, Qt::AlignHCenter);
  m_lay->addLayout(m_boxOk );
  connect(m_ok, SIGNAL(clicked() ),
    this, SLOT(slotOk() )  );
  connect(m_cancel, SIGNAL(clicked() ),
    this, SLOT(slotCancel() )  );

}

void OFileSelector::initializeChooser()
{
  m_boxView = new QHBoxLayout(this );

  m_mimeCheck = new QComboBox(this, "mime check");
  m_viewCheck = new QComboBox(this, "view check");
  m_boxView->addWidget(m_viewCheck, 0 );
  m_boxView->insertSpacing(2, 8 );
  m_boxView->addWidget(m_mimeCheck, 0 );
  m_lay->addLayout(m_boxView );
  m_lay->insertSpacing( 4, 8);

  m_viewCheck->insertItem(tr("Documents") );
  m_viewCheck->insertItem(tr("Files") );
  m_viewCheck->insertItem(tr("All Files") );

//   if(!m_autoMime )
//     m_mimeCheck->insertItem(m_mimetypes.join("," ) );
//   else{ // check
    updateMimes();
    m_mimeCheck->insertStringList( m_mimetypes );
//   }

  connect( m_viewCheck, SIGNAL(activated(const QString &) ),
     this, SLOT(slotViewCheck(const QString & ) ) );

  connect( m_mimeCheck, SIGNAL(activated(const QString &) ),
     this, SLOT(slotMimeCheck(const QString & ) ) );
}

void OFileSelector::slotMimeCheck(const QString &view ){
  if(m_selector == NORMAL ){
    delete m_select;
    m_select = new FileSelector(view == "All" ? QString::null : view 
        , m_stack, "fileselector", FALSE, FALSE );
    m_stack->addWidget( m_select, NORMAL );
    m_stack->raiseWidget( NORMAL );
  }else{
    reparse();
  }
}

void OFileSelector::slotViewCheck(const QString &view ){
  qWarning("changed: show %s", view.latin1() );
  // if the current view is the one
  QString currMime = m_mimeCheck->currentText();
  if( view == QString::fromLatin1("Documents") ){
    // get the mimetype now
    // check if we're the current widget and return
    if( m_View != 0) { // delete 0 shouldn't crash but it did :( 
      delete m_View;
      delete m_boxToolbar;
      delete m_homeButton;
      delete m_docButton;
      delete m_location;
      delete m_up;
      delete m_pseudo;
      //if(m_pseudoLayout!=0 )
//  delete m_pseudoLayout;
    }
    m_View = 0;
    m_boxToolbar = 0;
    m_homeButton = 0;
    m_docButton = 0;
    m_location = 0;
    m_up = 0;
    m_pseudo = 0;
    m_pseudoLayout = 0;

    delete m_select;
    m_select = new FileSelector( currMime == "All" ? QString::null : currMime,
         m_stack,"fileselector", FALSE, FALSE );
    m_stack->addWidget( m_select, NORMAL );
    m_mimeCheck->clear();
    m_selector = NORMAL;
    updateMimes();
    m_mimeCheck->insertStringList( m_mimetypes );
    m_stack->raiseWidget( NORMAL );
    connect(m_select, SIGNAL(fileSelected( const DocLnk &) ), this, SLOT(slotFileBridgeSelected(const DocLnk &) ) );    

  }else if(view == QString::fromLatin1("Files") ){
    // remove from the stack
    delete m_select;
    m_select = 0;
    delete m_View;
    m_View = 0;

    

    m_selector = EXTENDED;
    // create the ListView or IconView
    initializeListView();

    reparse();
  }else if(view == QString::fromLatin1("All Files") ) {
    // remove from the stack
    delete m_select;
    m_select = 0;
    delete m_View;
    m_View = 0;

    m_selector = EXTENDED_ALL;
    initializeListView();
    reparse();
  };
}


void OFileSelector::updateMimes() // lets check which mode is active
  // check the current dir for items then
{
  m_mimetypes.clear();
  m_mimetypes.append("All" );
  if( m_selector == NORMAL ){
    DocLnkSet set;
    Global::findDocuments(&set, QString::null );
    QListIterator<DocLnk> dit( set.children() );
    for ( ; dit.current(); ++dit ) {
      if( !m_mimetypes.contains((*dit)->type()  ) )
      m_mimetypes.append( (*dit)->type() );
    }
  }else{
    // should be allreday updatet
    ;
  }
}

void OFileSelector::initializeListView()
{ 
  // just to make sure but clean it up better FIXME
  delete m_View;
  m_View = 0;
  delete m_boxToolbar;
  delete m_homeButton;
  delete m_docButton;
  delete m_location;
  delete m_up;
  //delete m_pseudo;
  //if(m_pseudoLayout!=0 ) // why did you overload malloc
    //delete m_pseudoLayout;
  m_boxToolbar = 0;
  m_homeButton = 0;
  m_docButton = 0;
  m_location = 0;
  m_up = 0;
  m_pseudo = 0;
  m_pseudoLayout = 0;
  // time for the toolbar 
  m_pseudo = new QWidget(m_stack, "Pseudo Widget");
  m_pseudoLayout = new QVBoxLayout(m_pseudo );
  if(m_shTool ){
    m_boxToolbar = new QHBoxLayout( );
    m_boxToolbar->setAutoAdd( true );
    m_location = new QComboBox(m_pseudo );
    connect( m_location, SIGNAL(activated(const QString &) ), this, SLOT( locationComboActivated(const QString & ) ) );

    m_up = new QPushButton(Resource::loadIconSet("up"),"", m_pseudo,"cdUpButton");
    m_up->setFixedSize( QSize( 20, 20 ) );
    connect(m_up ,SIGNAL(clicked()),this,SLOT(cdUP()  ) );
    m_up->setFlat(TRUE);

    m_homeButton = new QPushButton(Resource::loadIconSet("home") , "", m_pseudo);
    m_homeButton->setFixedSize( QSize( 20, 20 ) );
    connect(m_homeButton,SIGNAL(clicked()),this,SLOT(slotHome() ) );
    m_homeButton->setFlat(TRUE);

    m_docButton = new QPushButton(Resource::loadIconSet("DocsIcon"),"", m_pseudo,"docsButton");
    m_docButton->setFixedSize( QSize( 20, 20 ) );
    connect(m_homeButton,SIGNAL(clicked()),this,SLOT(slotDoc() ) );
    m_docButton->setFlat(TRUE);

    m_boxToolbar->addWidget(m_location );
    m_boxToolbar->addWidget(m_up );
    m_boxToolbar->addWidget(m_homeButton );
    m_boxToolbar->addWidget(m_docButton );
    m_pseudoLayout->addLayout(m_boxToolbar );
    // lets fill the combobox
    StorageInfo storage;
    const QList<FileSystem> &fs = storage.fileSystems();
    QListIterator<FileSystem> it ( fs );
    for( ; it.current(); ++it ){
      const QString disk = (*it)->name();
      const QString path = (*it)->path();
      m_location->insertItem(path+ "<-"+disk );
    }
    int count = m_location->count();
    m_location->insertItem(m_currentDir );
    m_location->setCurrentItem( count );
  };
  m_View = new QListView(m_pseudo, "Extended view" );
  m_stack->addWidget( m_pseudo, EXTENDED );
  m_stack->raiseWidget( EXTENDED );
  m_pseudoLayout->addWidget(m_View );
  QPEApplication::setStylusOperation( m_View->viewport(),QPEApplication::RightOnHold);
  // set up the stuff
  // Pixmap Name Date Size mime
  //(m_View->header() )->hide();
  //m_View->setRootIsDecorated(false);
  m_View->addColumn(" ");
  m_View->addColumn(tr("Name"),135 );
  m_View->addColumn(tr("Size"),-1 );
  m_View->addColumn(tr("Date"), 60 );
  m_View->addColumn(tr("Mime Type"),-1 );
  QHeader *header = m_View->header();
  header->hide();
  m_View->setSorting(1 );
  m_View->setAllColumnsShowFocus( TRUE);
  // connect now
  connect(m_View, SIGNAL(selectionChanged() ), this, SLOT(slotSelectionChanged() ) );
  connect(m_View, SIGNAL(currentChanged(QListViewItem *) ), this, SLOT(slotCurrentChanged(QListViewItem * ) ) );
  connect(m_View, SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint &, int) ), 
    this, SLOT(slotClicked( int, QListViewItem *, const QPoint &, int) ) );
  connect(m_View, SIGNAL(mouseButtonPressed(int, QListViewItem *, const QPoint &, int )), 
    this, SLOT(slotRightButton(int, QListViewItem *, const QPoint &, int  ) ) );

 
};
/* If a item is locked  depends on the mode
   if we're in OPEN !isReadable is locked
   if we're in SAVE !isWriteable is locked


 */


void OFileSelector::addFile(const QString &mime, QFileInfo *info, bool symlink ){
  qWarning("Add Files" );
  if( !m_files ){
    qWarning("not mfiles" );
    return;
  }

  MimeType type( info->filePath() );
  QString name;
  QString dir;
  bool locked= false;
  if(mime == "All" ){
    ;
  }else if( type.id() != mime ) {
    return;
  }
  QPixmap pix = type.pixmap(); 
  if(pix.isNull() )
    pix = Resource::loadPixmap( "UnknownDocument-14" );
  dir = info->dirPath( true );
  if( symlink ) { // check if the readLink is readable
    // do it right later
    name = info->fileName() + " -> " + info->dirPath() + "/" + info->readLink();
  }else{ // keep track of the icons
    name = info->fileName();
    if( m_mode == OPEN ){
      if( !info->isReadable() ){
  locked = true;
  pix = Resource::loadPixmap("locked" );
      }
    }else if( m_mode == SAVE ){
      if( !info->isWritable() ){
  locked = true;
  pix = Resource::loadPixmap("locked" );
      }
    } 
  }
  new OFileSelectorItem( m_View, pix, name,
       info->lastModified().toString(),
       QString::number( info->size() ),
       dir, locked ); 
}

void OFileSelector::addDir(const QString &mime, QFileInfo *info, bool symlink  )
{
  if(!m_dir )
    return;
  //if( showDirs )
  {
    bool locked=false;
    QString name;
    QPixmap pix;
    if( ( m_mode == OPEN && !info->isReadable() ) || ( m_mode == SAVE && !info->isWritable()  ) ){
      locked = true;
      if( symlink ){
  pix = (*m_pixmaps)["symlinkedlocked"];
      }else{
  pix = Resource::loadPixmap("lockedfolder"  );
      }
    }else{
      if( symlink ){
  pix = (*m_pixmaps)["dirsymlink" ];
      }else{
  pix = Resource::loadPixmap("folder"  );
      }
    }
    if( symlink){
      name = info->fileName()+ "->"+ info->dirPath(true) +"/" +info->readLink();

    }else{
      //if(info->isReadable() )
      name = info->fileName();
    }

      new OFileSelectorItem(m_View, pix,
          name, info->lastModified().toString(),
          QString::number(info->size() ),info->dirPath(true),  locked, true );

  }
}

void OFileSelector::setShowDirs(bool dir )
{
  m_dir = dir;
  reparse();
}

void OFileSelector::slotFileSelected(const QString &string )
{
  if(m_shLne )
    m_edit->setText( string );

  emit fileSelected( string );
  // do AppLnk stuff
} 

void OFileSelector::slotFileBridgeSelected( const DocLnk &lnk )
{
  slotFileSelected(lnk.name() );
  emit fileSelected( lnk );
}

void OFileSelector::slotSelectionChanged() // get the current items
  // fixme
{
  qWarning("selection changed" );
}

void OFileSelector::slotCurrentChanged(QListViewItem *item )
{
  qWarning("current changed" );
  if( item == 0 )
    return;

  if( m_selector == EXTENDED || m_selector == EXTENDED_ALL ){
    OFileSelectorItem *sel = (OFileSelectorItem*)item;
    if(!sel->isDir() ){
      qWarning("is not dir" );
      if(m_shLne ){
  m_edit->setText(sel->text(1) );
  qWarning("setTexy" );
      }
    }
  }else {
    qWarning("mode not extended" );
  }
}

// either select or change dir
void OFileSelector::slotClicked( int button, QListViewItem *item, const QPoint &point, int )
{
  if( item == 0 )
    return;

  if( button != Qt::LeftButton )
    return; 

 qWarning("clicked" );
  if(m_selector == EXTENDED || m_selector == EXTENDED_ALL ){
    qWarning("inside" );
    OFileSelectorItem *sel = (OFileSelectorItem*)item;
    if(!sel->isLocked() ){ // not locked either changedir or open
      QStringList str = QStringList::split("->", sel->text(1) );
      if(sel->isDir() ){
        cd( sel->directory() + "/" + str[0] );
      } else {
          qWarning("file" );
          if(m_shLne )
              m_edit->setText(str[0] );
          emit fileSelected(str[0] );
            // emit DocLnk need to do it
      }
    } else {
      qWarning( "locked" );
    }
  };
}

void OFileSelector::slotRightButton(int button, QListViewItem *item, const QPoint &, int )
{
  if (item == 0 )
    return;

  if( button != Qt::RightButton )
    return; 
  qWarning("right button" );
  slotContextMenu(item);
}

void OFileSelector::slotContextMenu(QListViewItem *item)
{
  qWarning("context menu" );
  if( item ==0 || !m_showPopup )
    return;
 
  if( m_custom !=0){
    m_custom->exec();
  }else{
    QPopupMenu menu;
    QAction up;
    up.setText("cd up");
    up.addTo( &menu );
    connect(&up, SIGNAL(activated() ),
      this, SLOT(cdUP()  ) );

    QAction act;
    OFileSelectorItem *sel = (OFileSelectorItem*)item;
    if(sel->isDir() ){
      act.setText( tr("Change Directory") );
      act.addTo(&menu );
      connect(&act, SIGNAL(activated() ),
        this, SLOT(slotChangedDir() ) );
    }else{
      act.setText( tr("Open file" )  );
      act.addTo( &menu );
      connect(&act, SIGNAL(activated() ),
        this, SLOT(slotOpen() ) );
    }
    QAction rescan;
    rescan.setText( tr("Rescan")  );
    rescan.addTo( &menu );
    connect(&rescan, SIGNAL(activated() ),
      this, SLOT(slotRescan() ) );

    QAction rename;
    rename.setText( tr("Rename") );
    rename.addTo( &menu );
    connect(&rename, SIGNAL(activated() ),
      this, SLOT(slotRename() ) );

    menu.insertSeparator();
    QAction delItem;
    delItem.setText( tr("Delete")  );
    delItem.addTo(&menu );
    connect(&delItem, SIGNAL(activated() ),
      this, SLOT(slotDelete() ) );

    menu.exec(QCursor::pos() );
  }
}

bool OFileSelector::cd(const QString &str )
{
  qWarning(" dir %s", str.latin1() );
  QDir dir( str);
  if(dir.exists() ){
    m_currentDir = dir.absPath();
    reparse();
    if(m_shTool ){
        int count = m_location->count();
        insertLocationPath( str ,count );
      m_location->setCurrentItem( count );
    }
    return true;
  }
  return false;
}

void  OFileSelector::insertLocationPath(const QString &currentPath, int count) {
    QStringList pathList;
    for(int i=0;i<count;i++) {
        pathList << m_location->text(i);
    }
    if( pathList.grep( currentPath,TRUE).isEmpty() ) {
        m_location->clear();
        if( currentPath.left(2)=="//")
            pathList.append( currentPath.right(currentPath.length()-1) );
        else
            pathList.append( currentPath );
        m_location->insertStringList( pathList,-1);
    }
}

void OFileSelector::slotChangedDir()
{
  OFileSelectorItem *sel = (OFileSelectorItem*)m_View->currentItem();
  if(sel->isDir() ){
    QStringList str = QStringList::split("->", sel->text(1) );
    cd( sel->directory() + "/" + str[0] );
        
  }
}

void OFileSelector::slotOpen()
{
  OFileSelectorItem *sel = (OFileSelectorItem*)m_View->currentItem();
  if(!sel->isDir() ){
    QStringList str = QStringList::split("->", sel->text(1) );
    slotFileSelected( str[0] );
  }
}

void OFileSelector::slotRescan()
{
  reparse();
}

void OFileSelector::slotRename()
{
  // rename inline
}

void OFileSelector::slotDelete()
{
  qWarning("delete slot" );
  OFileSelectorItem *sel = (OFileSelectorItem*)m_View->currentItem();
  QStringList list = QStringList::split("->", sel->text(1) );
  if( sel->isDir() ){
      QString str = QString::fromLatin1("rm -rf ") + list[0]; //better safe than sorry
    switch ( QMessageBox::warning(this,tr("Delete"),tr("Do you really want to delete\n")+list[0],
                                  tr("Yes"),tr("No"),0,1,1) ) {
          case 0:
              ::system(str.utf8().data() );
          break;
    }
  } else {
    QFile::remove( list[0] );
  }
  m_View->takeItem( sel );
  delete sel;
}

void OFileSelector::cdUP()
{
  QDir dir( m_currentDir );
  dir.cdUp();
  if(dir.exists() ){
    m_currentDir = dir.absPath();
    reparse();
    int count = m_location->count();
    insertLocationPath( m_currentDir,count );
    m_location->setCurrentItem( indexFromString(m_currentDir ));
//this wont work in all instances
      // FIXME
  }
}

int OFileSelector::indexFromString(const QString &str) {

    for(int i=0;i<  m_location->count();i++) {
        if(str == m_location->text(i))
            return i;
    }
    return 0;
}

void OFileSelector::slotHome()
{
  cd(QDir::homeDirPath() );
}

void OFileSelector::slotDoc()
{
  cd(QDir::homeDirPath() + "/Documents" );
}

void OFileSelector::slotNavigate()
{

}

void OFileSelector::locationComboActivated(const QString & file ) {
    cd(file.left(file.find("<-",0,TRUE)));
    reparse();
}
