

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qabstractlayout.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qpopupmenu.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include <qpe/fileselector.h>
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
    int index= 0;
    for(int i= 0; i < box->count(); i++ ){
      if( str == box->text(i ) ){
	index= i;
	break;
      }
    }
    return index;
  }
  class OFileSelectorItem : public QListViewItem {
  public:
    OFileSelectorItem(QListView *view, const QPixmap &pixmap, const QString &path,
		      const QString &date, const QString &size, const QString &mDir,
		      bool isLocked=false, bool isDir=false ): QListViewItem(view) {
      setPixmap(0, pixmap );
      setText(1, path );
      setText(2, size );
      setText(3, date );
      //setText(4, mDir );
      m_dir = mDir;
      dir = isDir;
      mLocked = isLocked;
    }
    bool isLocked() const{
      return mLocked;
    }
    QString directory()const{
      return m_dir;
    }
    bool isDir()const{
      return dir;
    }
    QString path()const{
      return text(1 );
    }
    QString key(int id, bool )const {
      QString ke;
      if( id == 0 || id == 1 ){ // name
	if( dir ){
	  ke.append("0" );
	  ke.append( text(1) );
	}else{
	  ke.append("1" );
	  ke.append( text(1) );
	}
      }else if( id == 2 ){ // size
	return text(2);
      }else if( id == 3 ){ // date
	return text(3);
      }
      return ke;
    };
  private:
    bool mLocked:1;
    bool dir:1;
    QString m_dir;
  };
};


OFileSelector::OFileSelector( QWidget *wid, int mode, int selector,
			      const QString &dirName,
			      const QString &fileName,
			      const QStringList &mimeTypes )
  : QWidget( wid, "OFileSelector")
{
  m_mimetypes = mimeTypes;
  initVars();
  m_mode = mode;
  m_selector = selector;
  m_currentDir =  dirName;
  init();
QTimer::singleShot(6*1000, this, SLOT( slotTest() ) );
}

OFileSelector::OFileSelector(const QString &mimeFilter, QWidget *parent,
			     const char *name, bool newVisible = TRUE,
			     bool closeVisible = FALSE )
  : QWidget( parent, name )
{
  m_mimetypes = QStringList::split(";", mimeFilter );
  initVars();
  m_currentDir = QPEApplication::documentDir(); 
  m_mode = OPEN;
  m_selector = NORMAL;
  m_shClose = closeVisible;
  m_shNew = newVisible;
  m_shLne = false;
  m_shPerm = false;
  m_shYesNo = false;
  init();

  
}

OFileSelector::~OFileSelector()
{

}

void OFileSelector::setNewVisible( bool visible )
{
  m_shNew = visible;
  if( m_selector == NORMAL ){
    delete m_select;
    // we need to initialize but keep the selected mimetype
    QString mime = m_mimeCheck == 0 ? QString::null : m_mimeCheck->currentText() ;
    m_select = new FileSelector( m_autoMime ? mime : m_mimetypes.join(";") ,
				 m_stack, "fileselector",
				 m_shNew, m_shClose);
    connect(m_select, SIGNAL(fileSelected( const DocLnk & ) ),
	    this, SLOT( slotFileBridgeSelected(const DocLnk & ) ) );
    //connect to close me and other signals as well
    m_stack->addWidget( m_select, NORMAL );
  }else{
    m_new->show();
  }
}
void OFileSelector::setCloseVisible( bool visible )
{
  m_shClose = visible;
  if( m_selector == NORMAL ){
    setNewVisible( m_shNew ); // yeah baby
  }else{
    m_close->show();
  }
}
void OFileSelector::reread()
{
  if( m_selector == NORMAL ){
    setNewVisible( m_shNew ); // make it a initializeSelector
  }else if ( m_selector == EXTENDED || m_selector == EXTENDED_ALL ){
    reparse();
    //}else{
    //;
  }
}

const DocLnk *OFileSelector::selected()
{
  if( m_selector == NORMAL ){
    return m_select->selected();
  }else{
    DocLnk *lnk = new DocLnk(selectedDocument() );
    return lnk;
  }
}

void OFileSelector::setYesCancelVisible( bool show )
{
  initializeYes(); // FIXME if YesCancel is not shown we will initialize it to hide it :(
  m_shYesNo = show;
  if( m_shYesNo )
    m_boxOk->show();
  else
    m_boxOk->hide();

}
void OFileSelector::setToolbarVisible( bool show )
{
  m_shTool = show;
  initializeListView(); // FIXME see above waste of memory
  if(!m_shTool ){
    m_location->hide();
    m_up->hide();
    m_homeButton->hide();
    m_docButton->hide();
  }else{
    m_location->show();
    m_up->show();
    m_homeButton->show();
    m_docButton->show();
  }
}
void OFileSelector::setPermissionBarVisible( bool show )
{
  m_shPerm = show;
  initializePerm();
  if( m_shPerm )
    m_checkPerm->show();
  else
    m_checkPerm->hide();
}
void OFileSelector::setLineEditVisible( bool show )
{
  if( show ){
    initializeName();
    m_boxName->show();
  }else{
    if( m_shLne && m_boxName != 0 ){ // check if we showed before this is the way to go
      m_boxName->hide();
    }
  }
  m_shLne = show;
}

void OFileSelector::setChooserVisible( bool show )
{
  m_shChooser = show;
  initializeChooser();
  if( m_shChooser ){
    m_boxView->hide();
  }else{
    m_boxView->show();
  }
}

QCheckBox* OFileSelector::permissionCheckbox()
{
  if( m_selector == NORMAL )
    return 0l;
  else
    return m_checkPerm;
}
bool OFileSelector::setPermission()const
{
  return m_checkPerm == 0 ? false : m_checkPerm->isChecked();
}
void OFileSelector::setPermissionChecked( bool check )
{
  if( m_checkPerm )
    m_checkPerm->setChecked( check );
}

void OFileSelector::setMode(int mode) // FIXME do direct raising
{
  m_mode = mode;
  if( m_selector == NORMAL )
    return;
}
void OFileSelector::setShowDirs(bool )
{
  m_dir = true;
  reparse();
}
void OFileSelector::setCaseSensetive(bool caSe )
{
  m_case = caSe;
  reparse();
}
void OFileSelector::setShowFiles(bool show )
{
  m_files = show;
  reparse();
}
///
bool OFileSelector::cd(const QString &path )
{
  m_currentDir = path;
  reparse();
  return true;
}
void OFileSelector::setSelector(int mode )
{
QString text;
  switch( mode ){
  case NORMAL:
    text = tr("Documents");
    break;
  case EXTENDED:
    text = tr("Files");
    break;
  case EXTENDED_ALL:
    text = tr("All Files");
    break;
  }
  slotViewCheck( text );
}

void OFileSelector::setPopupMenu(QPopupMenu *popup )
{
  m_custom = popup;
  m_showPopup = true;
}

//void OFileSelector::updateL

QString OFileSelector::selectedName() const
{
  QString name;
  if( m_selector == NORMAL ){
    const DocLnk *lnk = m_select->selected();
    name = lnk->file();
    delete lnk;
  }else if( m_selector == EXTENDED || m_selector == EXTENDED_ALL ){
    QListViewItem *item  = m_View->currentItem();
    if( item != 0 )
      name = m_currentDir + "/" + item->text( 1 );
  }else { // installed view
    ;
  }
  return name;
}
QStringList OFileSelector::selectedNames()const
{
  QStringList list;
  if( m_selector == NORMAL ){
    list << selectedName();
  }else if ( m_selector == EXTENDED || m_selector == EXTENDED_ALL ) {
    list << selectedName(); // FIXME implement multiple Selections
  }
  return list;
}
/** If mode is set to the Dir selection this will return the selected path.
 *  
 *
 */
QString OFileSelector::selectedPath()const
{
  QString path;
  if( m_selector == NORMAL ){
    path = QPEApplication::documentDir();
  }else if( m_selector == EXTENDED || m_selector == EXTENDED_ALL ){
    ;
  }
  return path;
}
QStringList OFileSelector::selectedPaths() const
{
  QStringList  list;
  list << selectedPath();
  return list;
}
QString OFileSelector::directory()const
{
  if( m_selector == NORMAL )
    return QPEApplication::documentDir();
  
  return QDir(m_currentDir).absPath();
}

int OFileSelector::fileCount()
{
  int count;
  switch( m_selector ){
  case NORMAL:
    count = m_select->fileCount();
    break;
    //case CUSTOM:
  case EXTENDED:
  case EXTENDED_ALL:
  default:
    count = m_View->childCount();
    break;
  }
  return count;
}
DocLnk OFileSelector::selectedDocument() const
{
  DocLnk lnk;
  switch( m_selector ){
  case NORMAL:{
    const DocLnk *lnk2 = m_select->selected();
    lnk =  DocLnk(*lnk2 ); // copy
    delete lnk2;
    break;
  }
  case EXTENDED:
  case EXTENDED_ALL:
  default:
    lnk = DocLnk( selectedName() ); // new DocLnk
    break;
  }
  return lnk;
}
QValueList<DocLnk> OFileSelector::selectedDocuments() const
{
  QValueList<DocLnk> docs;
  docs.append( selectedDocument() );
  return docs;
}


// slots internal

void OFileSelector::slotOk()
{
  emit ok();
}
void OFileSelector::slotCancel()
{
  emit cancel();
}
void OFileSelector::slotViewCheck(const QString &sel)
{
  if( sel == tr("Documents" ) ){
    if( m_select == 0 ){
      // autMime? fix cause now we use All and not the current
      m_select = new FileSelector(m_autoMime ? QString::null : m_mimetypes.join(";"),
				  m_stack, "fileselector",
				  FALSE, FALSE);
      connect(m_select, SIGNAL(fileSelected( const DocLnk & ) ),
	      this, SLOT( slotFileBridgeSelected(const DocLnk & ) ) );
      //connect to close me and other signals as well
      
      m_stack->addWidget( m_select, NORMAL );
    }
    m_stack->raiseWidget( NORMAL );
    m_selector = NORMAL;
  }else if( sel == tr("Files") ){
    m_selector = EXTENDED;
    initializeListView();
    reparse();
    m_stack->raiseWidget( EXTENDED );
  }else if( sel == tr("All Files") ){
    m_selector = EXTENDED_ALL;
    initializeListView();
    reparse();
    m_stack->raiseWidget( EXTENDED ); // same widget other QFileFilter 
  }
}
void OFileSelector::slotMimeCheck(const QString &mime)
{
  if( m_selector == NORMAL ){
    if( m_autoMime ){
      delete m_select;
      m_select = new FileSelector( mime == tr("All") ? QString::null : mime,
				m_stack, "fileselector",
				FALSE, FALSE);

      connect(m_select, SIGNAL(fileSelected( const DocLnk & ) ),
	      this, SLOT( slotFileBridgeSelected(const DocLnk & ) ) );
      //connect to close me and other signals as well
      m_stack->addWidget( m_select, NORMAL );
      m_stack->raiseWidget( NORMAL );
      updateMimes();
      m_mimeCheck->clear();
      m_mimeCheck->insertStringList(m_mimetypes );
      m_mimeCheck->setCurrentItem(indexByString( m_mimeCheck, mime) );
    }
  }else{ // others
    qWarning("Mime %s", mime.latin1() );
    if(m_shChooser ){
      qWarning("Current Text %s", m_mimeCheck->currentText().latin1() );
      //m_mimeCheck->setCurrentItem(indexByString( m_mimeCheck, mime)  );
    }
    reparse();
  }

}
void OFileSelector::slotLocationActivated(const QString &file)
{
  cd(file.left(file.find("<-",0,TRUE)));
  reparse();
}
void OFileSelector::slotInsertLocationPath(const QString &currentPath, int count)
{
  QStringList pathList;
  bool underDog = FALSE;
  for(int i=0;i<count;i++) {
    pathList << m_location->text(i);
    if( m_location->text(i) == currentPath)
      underDog = TRUE;
  }
  if( !underDog) {
    m_location->clear();
    if( currentPath.left(2)=="//")
      pathList.append( currentPath.right(currentPath.length()-1) );
    else
      pathList.append( currentPath );
    m_location->insertStringList( pathList,-1);
  }
}
void OFileSelector::locationComboChanged()
{
  cd( m_location->lineEdit()->text());
  reparse();
}
void OFileSelector::init()
{
  m_lay = new QVBoxLayout( this );
  m_lay->setSpacing(0 );

  m_stack = new QWidgetStack( this );
  if( m_selector == NORMAL ){
    m_select = new FileSelector(m_autoMime ? QString::null : m_mimetypes.join(";"),
				m_stack, "fileselector",
				FALSE, FALSE);

    connect(m_select, SIGNAL(fileSelected( const DocLnk & ) ),
	    this, SLOT( slotFileBridgeSelected(const DocLnk & ) ) );
    //connect to close me and other signals as well

    m_stack->addWidget( m_select, NORMAL );
    m_stack->raiseWidget( NORMAL );
  }else{ // we're in init so it will be EXTENDED or EXTENDED_ALL
    // and initializeListview will take care of those
    // toolbar get's generade in initializeListView
    initializeListView( ); // will raise the widget as well
  }
  m_lay->addWidget( m_stack, 100 ); // add to the layout 10 = stretch

  if( m_shLne ) // the LineEdit with the current FileName
    initializeName();

  if( m_shPerm ) // the Permission QCheckBox
    initializePerm();

  if( m_shChooser ) // the Chooser for the view and Mimetypes
    initializeChooser();

  if( m_shYesNo ) // the Yes No button row
    initializeYes( );
}
void OFileSelector::updateMimes()
{
  if( m_autoMime ){
    m_mimetypes.clear();
    m_mimetypes.append( tr("All") );
    if( m_selector == NORMAL ){
      DocLnkSet  set;
      Global::findDocuments(&set, QString::null );
      QListIterator<DocLnk> dit( set.children() );
      for( ; dit.current(); ++dit ){
	if( !m_mimetypes.contains( (*dit)->type() ) )
	  m_mimetypes.append( (*dit)->type() );
      }
    }// else done in reparse
  }
}
void OFileSelector::initVars()
{
  if( m_mimetypes.isEmpty() )
    m_autoMime = true;
  else
    m_autoMime = false;
  m_shClose = false;
  m_shNew = false;
  m_shTool = true;
  m_shPerm = false;
  m_shLne = true;
  m_shChooser = true;
  m_shYesNo = true;
  m_case = false;
  m_dir = true;
  m_files = true;
  m_showPopup = false;
  if(m_pixmaps == 0 ) // init the pixmaps
    initPics();

  // pointers
  m_location = 0;
  m_mimeCheck = 0;
  m_viewCheck = 0;
  m_homeButton = 0;
  m_docButton = 0;
  m_hideButton = 0;
  m_ok = 0;
  m_cancel = 0;
  m_reread = 0;
  m_up = 0;
  m_View = 0;
  m_checkPerm = 0;
  m_pseudo = 0;
  m_pseudoLayout = 0;
  m_select = 0;
  m_stack = 0;
  m_lay = 0;
  m_Oselector = 0;
  m_boxToolbar = 0;
  m_boxOk = 0;
  m_boxName = 0;
  m_boxView = 0;
  m_custom = 0;
  m_edit = 0;
  m_fnLabel = 0;
  m_new = 0;
  m_close = 0;
}
void OFileSelector::addFile(const QString &mime, QFileInfo *info, bool symlink)
{
  if(!m_files)
    return;
  //  if( !compliesMime(info->absFilePath(), mime ) )
  //  return;
  MimeType type( info->absFilePath() );
  if( mime != tr("All") && type.id() != mime )
    return;
  QPixmap pix = type.pixmap();
  QString dir;
  QString name;
  bool locked;
  if( pix.isNull() )
    pix = Resource::loadPixmap( "UnknownDocument-14");
  dir = info->dirPath( true );
  if( symlink )
    name = info->fileName() + " -> " +info->dirPath() + "/" + info->readLink();
  else {
    name = info->fileName();
    if( ( m_mode == OPEN && !info->isReadable() )||
	( m_mode == SAVE && !info->isWritable() ) ){
      
      locked = true; pix = Resource::loadPixmap("locked");
    }
  }
  new OFileSelectorItem( m_View, pix, name,
			 info->lastModified().toString(),
			 QString::number( info->size() ),
			 dir, locked );
}
void OFileSelector::addDir(const QString &mime, QFileInfo *info, bool symlink )
{
  if(!m_dir)
    return;
  if( m_selector == EXTENDED_ALL || m_selector == EXTENDED ){
    bool locked = false;
    QString name;
    QPixmap pix;
    if( ( m_mode == OPEN && !info->isReadable() ) || ( m_mode == SAVE && !info->isWritable() ) ){
      locked = true;
      if( symlink )
	pix = (*m_pixmaps)["symlinkedlocked"];
      else
	pix = Resource::loadPixmap("lockedfolder");
    }else { // readable
      pix = symlink ?  (*m_pixmaps)["dirsymlink"] : Resource::loadPixmap("folder") ;
    }
    name = symlink ? info->fileName() + "->" + info->dirPath(true) + "/" +info->readLink() : info->fileName() ;
    new OFileSelectorItem( m_View, pix, name,
			   info->lastModified().toString(),
			   QString::number( info->size() ), 
			   info->dirPath( true ), locked,
			   true );

  }// else CUSTOM View
}
void OFileSelector::delItems()
{

}
void OFileSelector::initializeName()
{
  /**  Name Layout Line
   *   This is the Layout line arranged in 
   *   horizontal way each components 
   *   are next to each other
   *   but we will only do this if
   *   we didn't initialize a while ago.
   */
  if( m_boxName == 0 ){
    m_boxName = new QHBox( this ); // remove this this? or use a QHBox
    m_fnLabel = new QLabel( m_boxName );
    m_fnLabel->setText( tr("Name:") );
    m_edit = new QLineEdit( m_boxName );
    //m_boxName->addWidget( m_fnLabel );
    m_boxName->setMargin( 5 );
    m_boxName->setSpacing( 8 );
    //m_boxName->setStretchFactor(m_edit, 100 ); // 100 is stretch factor

    m_lay->addWidget( m_boxName, 0 ); // add it to the topLevel layout
  }// else we already initialized
  // maybe show the components? 
  //
}
void OFileSelector::initializeYes()
{
  /** The Save Cancel bar
   *
   */
  if( m_boxOk == 0 ){
    m_boxOk = new QHBox( this );
    m_ok = new QPushButton( tr("&Save"),m_boxOk , "save" );
    m_cancel = new QPushButton( tr("C&ancel"), m_boxOk, "cancel" );

    //m_boxOk->addWidget( m_ok );
    //m_boxOk->addWidget( m_cancel );
    m_boxOk->setMargin( 5 );
    m_boxOk->setSpacing( 10 );
    m_lay->addWidget( m_boxOk, 0 );

    connect( m_ok, SIGNAL( clicked() ),
	     this, SLOT(slotOk() ) );
    connect( m_cancel, SIGNAL( clicked() ),
	     this, SLOT( slotCancel() ) );
  }
}
void OFileSelector::initializeChooser()
{
  if( m_boxView == 0 ){
    m_boxView = new QHBox( this );
    m_viewCheck = new QComboBox( m_boxView, "view check");
    m_mimeCheck = new QComboBox( m_boxView, "mime check");
    m_boxView->setSpacing( 8 );
    m_lay->addWidget(m_boxView, 0 );

    m_viewCheck->insertItem( tr("Documents") );
    m_viewCheck->insertItem( tr("Files") );
    m_viewCheck->insertItem( tr("All Files") );
    if(!m_autoMime )
      m_mimeCheck->insertItem(m_mimetypes.join(",") );
    else{
      updateMimes();
      m_mimeCheck->insertStringList( m_mimetypes );
    }
    connect( m_viewCheck, SIGNAL( activated(const QString & ) ),
	     this, SLOT( slotViewCheck(const QString & ) ) );
    connect( m_mimeCheck, SIGNAL( activated(const QString & ) ),
	     this, SLOT( slotMimeCheck( const QString & ) ) );
  }
}
void OFileSelector::initializeListView()
{
  if( m_pseudo == 0 ){
    m_pseudo = new QWidget( m_stack, "Pseudo Widget");
    m_pseudoLayout = new QVBoxLayout( m_pseudo );
    // toolbar
    m_boxToolbar = new QHBox( m_pseudo );
    m_boxToolbar->setSpacing(0 ); // next to each other please

    // toolbar members 
    {
      // location QComboBox
      m_location = new QComboBox( m_boxToolbar );
      m_location->setEditable( TRUE ); 
      m_location->setDuplicatesEnabled( FALSE );
      connect( m_location, SIGNAL(activated(const QString &) ),
	       this, SLOT( slotLocationActivated(const QString &) ) );
      connect( m_location->lineEdit(), SIGNAL(returnPressed() ),
	       this, SLOT(locationComboChanged() ) );
      // UP Button
      m_up = new QPushButton(Resource::loadIconSet("up"),"",
			     m_boxToolbar,"cdUpButton");
      m_up->setFixedSize( QSize( 20, 20 ) );
      connect(m_up ,SIGNAL(clicked()),this,SLOT(cdUP()  ) );
      m_up->setFlat(TRUE);

      // Home Button
      m_homeButton = new QPushButton(Resource::loadIconSet("home") ,
				     "", m_boxToolbar);
      m_homeButton->setFixedSize( QSize( 20, 20 ) );
      connect(m_homeButton,SIGNAL(clicked()),this,SLOT(slotHome() ) );
      m_homeButton->setFlat(TRUE);
      // Documents Button
      m_docButton = new QPushButton(Resource::loadIconSet("DocsIcon"),"",
				    m_boxToolbar,"docsButton");
      m_docButton->setFixedSize( QSize( 20, 20 ) );
      connect(m_homeButton,SIGNAL(clicked()),this,SLOT(slotDoc() ) );
      m_docButton->setFlat(TRUE);

      // Close button
      m_close = new QPushButton( Resource::loadIconSet( "close"), "",
				 m_boxToolbar );
      connect( m_close, SIGNAL(clicked() ), this, SIGNAL(closeMe()  ) );
      m_close->setFixedSize( 20, 20 );

      m_boxToolbar->setFixedHeight( 20 );
      m_pseudoLayout->addWidget(m_boxToolbar );

      // let;s fill the Location ComboBox
      StorageInfo storage;
      const QList<FileSystem> &fs = storage.fileSystems();
      QListIterator<FileSystem> it ( fs );
      for( ; it.current(); ++it ){
	const QString disk = (*it)->name();
	const QString path = (*it)->path();
	m_location->insertItem(path+ "<-"+disk );
      }
      int count = m_location->count();
      m_location->insertItem( m_currentDir );
      m_location->setCurrentItem( count );
      // due to the New and Close button we can not simply hide m_boxToolBar to not show it
      if( !m_shTool ){
	m_location->hide(  );
	m_up->hide(  );
	m_homeButton->hide(  );
	m_docButton->hide( );
      }
      if(!m_shClose )
	m_close->hide();
      if(!m_shNew)
	m_close->hide();

    } // off toolbar
    // the Main ListView
    // make a QWidgetStack first so Views can share the Toolbar
    m_View = new QListView( m_pseudo, "Extended view");
    QPEApplication::setStylusOperation( m_View->viewport(),
					QPEApplication::RightOnHold);
    m_View->addColumn(" " );
    m_View->addColumn(tr("Name"), 135 );
    m_View->addColumn(tr("Size"), -1 );
    m_View->addColumn(tr("Date"), 60 );
    m_View->addColumn(tr("Mime Type"), -1 );
    QHeader *header = m_View->header();
    header->hide();
    m_View->setSorting( 1 );
    m_View->setAllColumnsShowFocus( TRUE );

    connect(m_View, SIGNAL(selectionChanged() ),
	    this, SLOT(slotSelectionChanged() ) );

    connect(m_View, SIGNAL(currentChanged(QListViewItem *) ),
	    this, SLOT(slotCurrentChanged(QListViewItem * ) ) );

    connect(m_View, SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint &, int) ), 
	    this, SLOT(slotClicked( int, QListViewItem *, const QPoint &, int) ) );

    connect(m_View, SIGNAL(mouseButtonPressed(int, QListViewItem *, const QPoint &, int )), 
	    this, SLOT(slotRightButton(int, QListViewItem *, const QPoint &, int  ) ) );

    m_pseudoLayout->addWidget( m_View, 288 ); 
    m_stack->addWidget( m_pseudo, EXTENDED );
  }
}
void OFileSelector::initializePerm()
{
  if( m_checkPerm == 0 ){
    m_checkPerm = new QCheckBox(tr("Ser Permission"), this, "perm");
    m_checkPerm->setChecked( false );
    m_lay->addWidget( m_checkPerm );

  }
}
void OFileSelector::initPics()
{
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
// if a mime complies with the m_mimeCheck->currentItem
bool OFileSelector::compliesMime( const QString &path, const QString &mime )
{
  if( mime == "All" )
    return true;
  MimeType type( path );
  if( type.id() == mime )
    return true;
  return false;
}

void OFileSelector::slotFileSelected( const QString &string )
{
  if( m_shLne )
    m_edit->setText( string );
  emit fileSelected( string );
}
void OFileSelector::slotFileBridgeSelected( const DocLnk &lnk )
{
  slotFileSelected( lnk.name() );
  // emit fileSelected( lnk );
}
void OFileSelector::slotSelectionChanged()
{

}
void OFileSelector::slotCurrentChanged(QListViewItem* item )
{
  if( item == 0 )
    return;
  if( m_selector == EXTENDED || m_selector == EXTENDED_ALL ) {
    OFileSelectorItem *sel = (OFileSelectorItem*) item; // start to use the C++ casts ;)
    if(!sel->isDir() ){
      if( m_shLne )
	m_edit->setText( sel->text(1) );
    }
  }
}
void OFileSelector::slotClicked( int button, QListViewItem *item, const QPoint &, int)
{
  if ( item == 0 )
    return;

  if( button != Qt::LeftButton )
    return;

  switch( m_selector ){
  default:
    break;
  case EXTENDED: // fall through
  case EXTENDED_ALL:{
    OFileSelectorItem *sel = (OFileSelectorItem*)item;
    if(!sel->isLocked() ){
      QStringList str = QStringList::split("->", sel->text(1) );
      if( sel->isDir() ){
	cd( sel->directory() + "/" + str[0].stripWhiteSpace() );
	// if MODE Dir m_shLne set the Text
      }else{
	if( m_shLne )
	  m_edit->setText(  str[0].stripWhiteSpace() );
	emit fileSelected( sel->directory() + "/" + str[0].stripWhiteSpace() );
      }
    }
    break;
  }
  }
}
void OFileSelector::slotRightButton(int button, QListViewItem *item, const QPoint &, int )
{
  if( item == 0 )
    return;

  if( button != Qt::RightButton )
    return;
  slotContextMenu( item );
}
void OFileSelector::slotContextMenu( QListViewItem *item)
{

}
void OFileSelector::slotChangedDir()
{
  OFileSelectorItem *sel = (OFileSelectorItem*)m_View->currentItem();
  if(sel->isDir() ){
    QStringList str = QStringList::split("->", sel->text(1) );
    cd( sel->directory() + "/" + str[0].stripWhiteSpace() );
  }
}
void OFileSelector::slotOpen()
{
  OFileSelectorItem *sel = (OFileSelectorItem*)m_View->currentItem();
  if(!sel->isDir() ){
    QStringList str = QStringList::split("->", sel->text(1) );
    slotFileSelected( sel->directory() +"/" +str[0].stripWhiteSpace() );
  }
}
void OFileSelector::slotRescan()
{

}
void OFileSelector::slotRename()
{
  reparse();
}
void OFileSelector::slotDelete()
{
  OFileSelectorItem *sel = (OFileSelectorItem*)m_View->currentItem();
  QStringList list = QStringList::split("->", sel->text(1) );
  if( sel->isDir() ){
    QString str = QString::fromLatin1("rm -rf ") + sel->directory() +"/" + list[0]; //better safe than sorry
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
    slotInsertLocationPath( m_currentDir, count);
    m_location->setCurrentItem( indexByString( m_location, m_currentDir));
    //this wont work in all instances
    // FIXME
  }
}
void OFileSelector::slotHome()
{
  cd(QDir::homeDirPath() );
}
void OFileSelector::slotDoc()
{
  cd(QPEApplication::documentDir() );
}
void OFileSelector::slotNavigate( )
{

}
// fill the View with life
void OFileSelector::reparse()
{
  if( m_selector == NORMAL )
    return;
  if( m_selector == EXTENDED || m_selector == EXTENDED_ALL )
    m_View->clear();
  else // custom view
    ; // currentView()->clear();
  if( m_shChooser)
    qWarning("reparse %s", m_mimeCheck->currentText().latin1() );

  QString currentMimeType;
  // let's update the mimetype
  if( m_autoMime ){
    m_mimetypes.clear();
    // ok we can change mimetype so we need to be able to give a selection
    if( m_shChooser ) {
      currentMimeType = m_mimeCheck->currentText();
      m_mimeCheck->clear();

      // let's find possible mimetypes
      QDir dir( m_currentDir );
      dir.setFilter( QDir::Files | QDir::Readable );
      dir.setSorting( QDir::Size );
      const QFileInfoList *list = dir.entryInfoList();
      QFileInfoListIterator it( *list );
      QFileInfo *fi;
      while( (fi=it.current() ) ) {
	if( fi->extension() == QString::fromLatin1("desktop") ){
	  ++it;
	  continue;
	}
	MimeType type( fi->absFilePath() );
	if( !m_mimetypes.contains( type.id() ) ){
	  //qWarning("Type %s", type.id().latin1() );
	  m_mimetypes.append( type.id() );
	}

	++it;
      }
      // add them to the chooser
      m_mimeCheck->insertItem( tr("All") );
      m_mimeCheck->insertStringList( m_mimetypes );
      m_mimeCheck->setCurrentItem( indexByString( m_mimeCheck, currentMimeType ) );
      currentMimeType = m_mimeCheck->currentText();
    }
  }else { // no autoMime
    currentMimeType = m_mimetypes.join(";");
    if( m_shChooser ){
      m_mimeCheck->clear();
      m_mimeCheck->insertItem(m_mimetypes.join(",") );
    }
  }
  // now we got our mimetypes we can add the files

  QDir dir( m_currentDir );

  int sort;
  if ( m_case )
    sort = (QDir::IgnoreCase | QDir::Name | QDir::DirsFirst | QDir::Reversed);
  else  
    sort = (QDir::Name | QDir::DirsFirst | QDir::Reversed);
  dir.setSorting( sort );

  int filter;
  if( m_selector == EXTENDED_ALL /*|| m_selector ==CUSTOM_ALL */ ){
    filter = QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All;
  }else
    filter = QDir::Files | QDir::Dirs | QDir::All;
  dir.setFilter( filter );

  // now go through all files
  const QFileInfoList *list = dir.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  while( (fi=it.current() ) ){
    //qWarning("True and only" );
    if( fi->fileName() == QString::fromLatin1("..") || fi->fileName() == QString::fromLatin1(".") ){
      //qWarning(".. or ." );
      ++it;
      continue;
    }
    if( fi->isSymLink() ){
      QString file = fi->dirPath( true ) + "/" + fi->readLink();
      for( int i = 0; i<=4; i++) { // 5 tries to prevent dos
	QFileInfo info( file );
	if( !info.exists() ){
	  addSymlink( currentMimeType, fi, TRUE );
	  break;
	}else if( info.isDir() ){
	  addDir( currentMimeType, fi, TRUE );
	  break;
	}else if( info.isFile() ){
	  addFile( currentMimeType, fi, TRUE );
	  break;
	}else if( info.isSymLink() ){
	  file = info.dirPath(true ) + "/" + info.readLink() ;
	  break;
	}else if( i == 4){
	  addSymlink( currentMimeType, fi );
	}
      } // off for loop
    }else if( fi->isDir() ){
      addDir( currentMimeType, fi );
    }else if( fi->isFile() ){
      addFile( currentMimeType, fi );
    }
    //qWarning( "%s", fi->fileName().latin1() );
    ++it;
  } // of while loop
  m_View->sort(); 
  if( m_shTool ){
    m_location->insertItem( m_currentDir );

  }
  // reenable painting and updates
}
