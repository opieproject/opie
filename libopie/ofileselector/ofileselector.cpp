

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
#include <qpe/global.h>
#include <qpe/mimetype.h>
#include <qpe/resource.h>
#include <qpe/storage.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "ofileview.h"
#include "ofileselectormain.h"
#include "ofileselector.h"
#include "olocallister.h"
#include "olister.h"
#include "odefaultfactories.h"

QMap<QString,QPixmap> *OFileSelector::m_pixmaps = 0;

namespace {
    /* let's find the index for a specified string */
    int indexByString( const QComboBox *box, const QString &str ){
        int index= 0;
        for(int i= 0; i < box->count(); i++ ){
            /* found */
            if( str == box->text(i ) ){
                index= i;
                break;
            }
        }
        return index;
    }
}

OFileSelector::OFileSelector( QWidget *wid, int mode, int selector,
			      const QString &dirName,
   			      const QString &fileName,
                              const QMap<QString,QStringList>& mimeTypes)
  : QWidget( wid, "OFileSelector")
{
  m_mimetypes = mimeTypes;
  if (mode == Save )
      m_name = fileName;

  initVars();

  m_mode = mode;
  m_selector = selector;
  m_currentDir =  dirName;
  init();
}

OFileSelector::OFileSelector(const QString &mimeFilter, QWidget *parent,
			     const char *name, bool newVisible,
			     bool closeVisible )
  : QWidget( parent, name )
{
    /* update the mimefilter */
  if (!mimeFilter.isEmpty() ) {
      QStringList list = QStringList::split(";", mimeFilter );
      m_mimetypes.insert(mimeFilter, list );
  }
  initVars();
  m_currentDir = QPEApplication::documentDir();
  m_mode = Fileselector;
  m_selector = Normal;
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
  if (m_new )
      m_new->show();
}
void OFileSelector::setCloseVisible( bool visible )
{
  m_shClose = visible;

  if( m_close )
    m_close->show();
}
void OFileSelector::reread()
{
  if( m_selector == Normal )
      initializeOldSelector();
  else
      reparse();
}

const DocLnk *OFileSelector::selected()
{
    DocLnk *lnk = new DocLnk(selectedDocument() );
    return lnk;
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
      // check if we showed before this is the way to go
      if( m_shLne && m_boxName != 0 )
          m_boxName->hide();
  }
  m_shLne = show;
}

void OFileSelector::setChooserVisible( bool show )
{
  m_shChooser = show;
  initializeChooser();

  if( m_shChooser )
      m_boxView->hide();
  else
      m_boxView->show();

}

QCheckBox* OFileSelector::permissionCheckbox()
{
  if( m_selector == Normal )
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
  if( m_selector == Normal )
    return;
}
void OFileSelector::setShowDirs(bool dir)
{
  m_dir = dir;
  if ( m_selector != Fileselector )
      reparse();
}
void OFileSelector::setCaseSensetive(bool caSe )
{
  m_case = caSe;

  if ( m_selector != Fileselector )
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
  case Normal:
    text = tr("Documents");
    break;
  case Extended:
    text = tr("Files");
    break;
  case ExtendedAll:
    text = tr("All Files");
    break;
  }
  slotViewCheck( text );
}

void OFileSelector::setPopupFactory(OPopupMenuFactory */*popup*/ )
{
/*  m_custom = popup;
  m_showPopup = true;
*/
}

//void OFileSelector::updateL

QString OFileSelector::selectedName() const
{
  QString name;
  if( m_selector == Normal ){
      DocLnk lnk = m_select->selectedDocument();
      name = lnk.file();
  }else {
      if ( m_shLne ) {
          name = m_currentDir + "/" +m_edit->text();
      }else{
          name = m_currentDir + "/" + currentView()->selectedName();
      }
  }
  return name;
}
QStringList OFileSelector::selectedNames()const
{
  QStringList list;
  if( m_selector == Normal ){
    list << selectedName();
  }else  {
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
  if( m_selector == Normal ){
    path = QPEApplication::documentDir();
  }  /* normal case to do */
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
  if( m_selector == Normal )
    return QPEApplication::documentDir();

  return QDir(m_currentDir).absPath();
}

int OFileSelector::fileCount()
{
  int count;
  switch( m_selector ){
  case Normal:
    count = m_select->fileCount();
    break;
  case Extended:
  case ExtendedAll:
  default:
      count = currentView()->fileCount();
    break;
  }
  return count;
}
DocLnk OFileSelector::selectedDocument() const
{
  DocLnk lnk;
  switch( m_selector ){
  case Normal:{
    lnk = m_select->selectedDocument();
    break;
  }
  case Extended:
  case ExtendedAll:
  default:
    lnk = DocLnk( selectedName() );
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
/* switch the views */
void OFileSelector::slotViewCheck(const QString &sel)
{
  if( sel == tr("Documents" ) ){
      initializeOldSelector();
      m_selector = Normal;

  }else {
;
  }
}

QString OFileSelector::currentMimeType() const{
    QString mime;
    QString currentText;
    if (m_shChooser && m_mimeCheck )
        currentText = m_mimeCheck->currentText();

    qWarning("CurrentText" + currentText );
    if (tr("All") == currentText ) return QString::null;
    else if (currentText.isEmpty() ) {
        ;
    }else {
        QMap<QString, QStringList>::ConstIterator it;
        it = m_mimetypes.find( currentText );
        if ( it != m_mimetypes.end() ) {
            mime = it.data().join(";");
        }else{
            mime = currentText;
        }
    }
    return mime;
}
void OFileSelector::slotMimeCheck(const QString &mime)
{
  if( m_selector == Normal ){
      initializeOldSelector();

      updateMimes();
      updateMimeCheck();
      m_mimeCheck->setCurrentItem(indexByString( m_mimeCheck, mime) );
  }else{ // others
    qWarning("Mime %s", mime.latin1() );
    if(m_shChooser ){
      qWarning("Current Text %s", m_mimeCheck->currentText().latin1() );
      //m_mimeCheck->setCurrentItem(indexByString( m_mimeCheck, mime)  );
    }
    reparse();
  }

}
/*
 * Ok if a non dir gets inserted into this combobox
 * we need to change it
 * QFileInfo and dirPath will give us the right Dir
 */
void OFileSelector::slotLocationActivated(const QString &file)
{
    qWarning("slotLocationActivated");
    QString name = file.left( file.find("<-", 0, TRUE ) );
    QFileInfo info( name );
    if ( info.isFile() )
        cd(info.dirPath( TRUE ) ); //absolute
    else
        cd(name );
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
/*
 * Do not crash anymore
 * don't try to change dir to a file
 */
void OFileSelector::locationComboChanged()
{
    QFileInfo info( m_location->lineEdit()->text() );
    qWarning("info %s %s", info.dirPath(true).latin1(), m_location->lineEdit()->text().latin1() );
    if (info.isFile() )
        cd(info.dirPath(TRUE) ); //absolute path
    else
        cd( m_location->lineEdit()->text() );

    reparse();
}
void OFileSelector::init()
{
  initFactory();
  m_lay = new QVBoxLayout( this );
  m_lay->setSpacing(0 );

  /* take care of the main view... */
  initToolbar();

  /* initialize the file lister */
  initLister();
  if( m_selector == Normal ){
      QString mime;
      if (!m_autoMime) {
          if (!m_mimetypes.isEmpty() ) {
              QMap<QString, QStringList>::Iterator it;
              it = m_mimetypes.begin(); // cause we're in the init
              mime = it.data().join(";");
          }
      }
      initializeOldSelector();
  }else{
      initializeView();
  }

  if( m_shLne ) // the LineEdit with the current FileName
      initializeName();

  if( m_shPerm ) // the Permission QCheckBox
      initializePerm();

  if( m_shChooser ) // the Chooser for the view and Mimetypes
      initializeChooser();

  if( m_shYesNo ) // the Yes No button row
      initializeYes( );

  if (m_selector != Normal )
      reparse();
}
void OFileSelector::updateMimes()
{
  if( m_autoMime ){
    m_mimetypes.clear();
    m_mimetypes.insert( tr("All"), QString::null );
    if( m_selector == Normal ){
      DocLnkSet  set;
      Global::findDocuments(&set, QString::null );
      QListIterator<DocLnk> dit( set.children() );
      for( ; dit.current(); ++dit ){
	if( !m_mimetypes.contains( (*dit)->type() ) )
	  m_mimetypes.insert( (*dit)->type(), (*dit)->type() );
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
  m_mainView = 0l;
  m_fileView = 0l;
  m_lister = 0l;

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
  m_lay = 0;
  m_Oselector = 0;
  m_boxToolbar = 0;
  m_boxOk = 0;
  m_boxName = 0;
  m_boxView = 0;
  m_edit = 0;
  m_fnLabel = 0;
  m_new = 0;
  m_close = 0;
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
    m_edit->setText( m_name );
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
/*
 * OK m_mimeCheck is a QComboBox we now want to fill
 * out that combobox
 * if automime we need to update the mimetypes
 */
void OFileSelector::updateMimeCheck() {
    m_mimeCheck->clear();
    if (m_autoMime ) {
        //m_mimeCheck->insertItem( tr("All") );
        updateMimes();
    }

    QMap<QString, QStringList>::Iterator it;
    for (it = m_mimetypes.begin(); it != m_mimetypes.end(); ++it ) {
        m_mimeCheck->insertItem( it.key() );
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


    updateMimeCheck();

    connect( m_viewCheck, SIGNAL( activated(const QString & ) ),
	     this, SLOT( slotViewCheck(const QString & ) ) );
    connect( m_mimeCheck, SIGNAL( activated(const QString & ) ),
	     this, SLOT( slotMimeCheck( const QString & ) ) );
  }
}
/* generate the buttons for the toolbar */
void OFileSelector::initToolbar() {
    m_mainView = new OFileSelectorMain( this );

    /* now generate the tool bar */
    qWarning( "toolbar" );
    m_pseudo = new QWidget( m_mainView,  "Pseudo Widget" );
    m_pseudoLayout = new QVBoxLayout( m_pseudo );

    m_boxToolbar = new QHBox( m_pseudo );
    m_boxToolbar->setSpacing( 0 );

    // tool bar members now
    m_location = new QComboBox( m_boxToolbar );
    m_location->setEditable( TRUE );
    m_location->setDuplicatesEnabled( FALSE );
    connect( m_location,  SIGNAL(activated(const QString& ) ),
             this, SLOT(slotLocationActivated(const QString& )) );
    connect( m_location->lineEdit(),  SIGNAL(returnPressed() ) ,
             this, SLOT(locationComboChanged() ) );

    // UP Button
    m_up = new QPushButton( Resource::loadIconSet("up"), QString::null,
                            m_boxToolbar,  "cdUpButton" );
    m_up->setFixedSize( QSize(20, 20 ) );
    connect( m_up, SIGNAL( clicked() ), this, SLOT(cdUP() ) );
    m_up->setFlat( TRUE );

    // Home Button
    m_homeButton = new QPushButton(Resource::loadIconSet("home"),
                                   QString::null, m_boxToolbar );
    m_homeButton->setFixedSize( QSize(20, 20 ) );
    connect(m_homeButton, SIGNAL(clicked() ), this, SLOT(slotHome() ) );
    m_homeButton->setFlat( TRUE );

    // Documents Button
    m_docButton = new QPushButton( Resource::loadIconSet("DocsIcon"),
                                   QString::null, m_boxToolbar,
                                   "docsButton" );
    m_docButton->setFixedSize( QSize(20, 20 ) );
    m_docButton->setFlat( true );
    connect( m_docButton, SIGNAL(clicked() ),
             this, SLOT(slotDoc() ) );

    // close button
    m_close = new QPushButton( Resource::loadIconSet( "close"), "",
                               m_boxToolbar );
    connect( m_close, SIGNAL(clicked() ), this, SIGNAL(closeMe()  ) );
    m_close->setFixedSize( 20, 20 );

    m_boxToolbar->setFixedHeight( 20 );
    m_pseudoLayout->addWidget(m_boxToolbar );

    /* init the locations */
    initLocations();

    if( !m_shTool ){
        m_location->hide(  );
        m_up->hide(  );
        m_homeButton->hide(  );
        m_docButton->hide( );
    }
    if(!m_shClose )
        m_close->hide();

    m_mainView->setToolbar( m_pseudo );
    m_lay->addWidget( m_mainView,  100 );
}
/* initialize the OLocalLister */
void OFileSelector::initLister() {
    m_lister = new OLocalLister(this);
}
/* put default locations into the bar */
void OFileSelector::initLocations () {

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

}
void OFileSelector::initializePerm()
{
  if( m_checkPerm == 0 ){
    m_checkPerm = new QCheckBox(tr("Set Permission"), this, "perm");
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
/*  check if the mimetype in mime
 *  complies with the  one which is current
 */
/*
 * We've the mimetype of the file
 * We need to get the stringlist of the current mimetype
 *
 * mime = image/jpeg
 * QStringList = 'image/*'
 * or QStringList = image/jpeg;image/png;application/x-ogg
 * or QStringList = application/x-ogg;image/*;
 * with all these mime filters it should get acceptes
 * to do so we need to look if mime is contained inside
 * the stringlist
 * if it's contained return true
 * if not ( I'm no RegExp expert at all ) we'll look if a '/*'
 * is contained in the mimefilter and then we will
 * look if both are equal until the '/'
 */
bool OFileSelector::compliesMime( const QString& mime ) {
    qWarning("mimetype is %s", mime.latin1() );
    QString currentText;
    if (m_shChooser )
        currentText = m_mimeCheck->currentText();

    qWarning("current text is %s", currentText.latin1() );
    QMap<QString, QStringList>::Iterator it;
    QStringList list;
    if ( currentText == tr("All") ) return true;
    else if ( currentText.isEmpty() && !m_mimetypes.isEmpty() ) {
        it = m_mimetypes.begin();
        list = it.data();
    }else if ( currentText.isEmpty() ) return true;
    else{
        it = m_mimetypes.find(currentText );
        if ( it == m_mimetypes.end() ) qWarning("not there"), list << currentText;
        else qWarning("found"), list = it.data();
    }


    if ( list.contains(mime) ) return true;
    qWarning("list doesn't contain it ");
    QStringList::Iterator it2;
    int pos;
    for ( it2 = list.begin(); it2 != list.end(); ++it2 ) {
        pos = (*it2).findRev("/*");
        if ( pos >= 0 ) {
            if ( mime.contains( (*it2).left(pos) ) ) return true;
        }
    }
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


void OFileSelector::slotDelete()
{
    /*
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
    */
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
  if( m_selector == Normal )
    return;

  currentView()->clear();

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
      m_mimetypes = currentLister()->mimeTypes( m_currentDir );

      // add them to the chooser
      updateMimeCheck();
      m_mimeCheck->setCurrentItem( indexByString( m_mimeCheck, currentMimeType ) );
      currentMimeType = m_mimeCheck->currentText();
    }
  }else { // no autoMime
      // let the mimetype be set from out side the m_mimeCheck FEATURE

    if( m_shChooser )
        currentMimeType = m_mimeCheck->currentText();

  }
  // now we got our mimetypes we can add the files

  currentLister()->reparse( m_currentDir );
  /* we're done with adding let's sort */
  currentView()->sort();


  if( m_shTool ){
    m_location->insertItem( m_currentDir );

  }
  // reenable painting and updates
}

/*
 * the factory
 */
void OFileSelector::initFactory() {
    m_fileFactory = new OFileFactory();
    m_fileFactory->addLister(tr("Files"), newLocalLister );
    m_fileFactory->addView(tr("List View"), newFileListView );
    /* the factory is just a dummy */
    m_fileFactory->addView(tr("Documents"), newFileListView );
}


OFileView* OFileSelector::currentView() {
    return m_fileView;
}
OFileView* OFileSelector::currentView() const{
    return m_fileView;
}
int OFileSelector::filter() {
    int filter;
    if ( m_selector == ExtendedAll )
        filter = QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All;
    else
        filter = QDir::Files | QDir::Dirs | QDir::All ;

    return filter;
}
int OFileSelector::sorting() {
    int sort;

    if (m_case )
        sort = ( QDir::IgnoreCase | QDir::Name | QDir::DirsFirst | QDir::Reversed );
    else
        sort = ( QDir::Name | QDir::DirsFirst | QDir::Reversed );

    return sort;
}
void OFileSelector::internFileSelected( const QString& s) {
    emit fileSelected( s );
}
void OFileSelector::internFileSelected( const DocLnk& d ) {
    emit fileSelected( d );
}
void OFileSelector::internContextMenu() {
    emit contextMenu();
}
void OFileSelector::internChangedDir( const QString& s) {
    emit dirSelected( s );
    cd(s );
}
void OFileSelector::internChangedDir( const QDir& s) {
    emit dirSelected( s );
}
QPixmap OFileSelector::pixmap( const QString& s ) {

    return (*m_pixmaps)[s];
}
OLister* OFileSelector::currentLister()const {
    return m_lister;
}
void OFileSelector::initializeOldSelector() {
    qWarning("initializeOldSelector");

    delete m_select;

    // we need to initialize but keep the selected mimetype
    /* we default not to show  close and new buttons */
    QString mime = currentMimeType();
    qWarning("MimeType " + mime );
    m_select = new FileSelector( mime ,
				 m_mainView, "fileselector",
				 FALSE, FALSE);
    m_select->setCategorySelectVisible( FALSE );
    m_select->setTypeComboVisible( FALSE );

    connect(m_select, SIGNAL(fileSelected( const DocLnk & ) ),
	    this, SLOT( slotFileBridgeSelected(const DocLnk & ) ) );
    connect(m_select, SIGNAL(closeMe() ),
	    this, SIGNAL(closeMe() ) );
    //connect to close me and other signals as well
    m_mainView->setWidget( m_select );
}
