#include <qcombobox.h>
#include <qdir.h>
#include <qhbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qwidgetstack.h>
#include <qregexp.h>
#include <qobjectlist.h>

/* hacky but we need to get FileSelector::filter */
#define private public
#include <qpe/fileselector.h>
#undef private

#include <qpe/qpeapplication.h>
#include <qpe/mimetype.h>
#include <qpe/resource.h>
#include <qpe/storage.h>

#include "ofileselector_p.h"
#include "ofileselector.h"



OFileViewInterface::OFileViewInterface( OFileSelector* selector )
    : m_selector( selector ) {
}
OFileViewInterface::~OFileViewInterface() {
}
QString OFileViewInterface::name()const{
    return m_name;
}
void OFileViewInterface::setName( const QString& name ) {
    m_name = name;
}
OFileSelector* OFileViewInterface::selector()const {
    return m_selector;
}
DocLnk OFileViewInterface::selectedDocument()const {
    return DocLnk( selectedName() );
}
bool OFileViewInterface::showNew()const {
    return selector()->showNew();
}
bool OFileViewInterface::showClose()const {
    return selector()->showClose();
}
MimeTypes OFileViewInterface::mimeTypes()const {
    return selector()->mimeTypes();
}
QStringList OFileViewInterface::currentMimeType()const {
    return selector()->currentMimeType();
}
void OFileViewInterface::activate( const QString& ) {
    // not implemented here
}
void OFileViewInterface::ok() {
    emit selector()->ok();
}
void OFileViewInterface::cancel() {
    emit selector()->cancel();
}
void OFileViewInterface::closeMe() {
    emit selector()->closeMe();
}
void OFileViewInterface::fileSelected( const QString& str) {
    emit selector()->fileSelected( str);
}
void OFileViewInterface::fileSelected( const DocLnk& lnk) {
    emit selector()->fileSelected( lnk );
}
void OFileViewInterface::setCurrentFileName( const QString& str ) {
    selector()->m_lneEdit->setText( str );
}
QString OFileViewInterface::currentFileName()const{
    return selector()->m_lneEdit->text();
}
QString OFileViewInterface::startDirectory()const{
    return selector()->m_startDir;
}


ODocumentFileView::ODocumentFileView( OFileSelector* selector )
    : OFileViewInterface( selector ) {
    m_selector = 0;
    setName( QObject::tr("Documents") );
}
ODocumentFileView::~ODocumentFileView() {

}
QString ODocumentFileView::selectedName()const {
    if (!m_selector)
        return QString::null;

    return m_selector->selectedDocument().file();
}
QString ODocumentFileView::selectedPath()const {
    return QPEApplication::documentDir();
}
QString ODocumentFileView::directory()const {
    return selectedPath();
}
void ODocumentFileView::reread() {
    if (!m_selector)
        return;

    m_selector->setNewVisible( showNew() );
    m_selector->setCloseVisible( showClose() );
    m_selector->filter = currentMimeType().join(";");
    m_selector->reread();
}
int ODocumentFileView::fileCount()const {
    if (!m_selector)
        return -1;

    return m_selector->fileCount();
}
DocLnk ODocumentFileView::selectedDocument()const {
    if (!m_selector)
        return DocLnk();

    return m_selector->selectedDocument();
}
QWidget* ODocumentFileView::widget( QWidget* parent ) {
    if (!m_selector ) {
        m_selector = new FileSelector(currentMimeType().join(";"), parent, "fileselector", showNew(), showClose() );
        QObject::connect(m_selector, SIGNAL(fileSelected( const DocLnk& ) ),
                         selector(), SLOT(slotDocLnkBridge(const DocLnk&) ) );
        QObject::connect(m_selector, SIGNAL(closeMe() ),
                         selector(), SIGNAL(closeMe() ) );
        QObject::connect(m_selector, SIGNAL(newSelected(const DocLnk& ) ),
                         selector(), SIGNAL(newSelected(const DocLnk& ) ) );
    }

    return m_selector;
}

/*
 * This is the file system view used
 * we use a QListView + QListViewItems for it
 */

OFileSelectorItem::OFileSelectorItem( QListView* view, const QPixmap& pixmap,
                                      const QString& path, const QString& date,
                                      const QString& size, const QString& dir,
                                      bool isLocked, bool isDir )
    : QListViewItem( view )
{
    setPixmap(0, pixmap );
    setText(1, path );
    setText(2, size );
    setText(3, date );
    m_isDir = isDir;
    m_dir = dir;
    m_locked = isLocked;
}
OFileSelectorItem::~OFileSelectorItem() {

}
bool OFileSelectorItem::isLocked()const {
    return m_locked;
}
QString OFileSelectorItem::directory()const {
    return m_dir;
}
bool OFileSelectorItem::isDir()const {
    return m_isDir;
}
QString OFileSelectorItem::path()const {
    return text( 1 );
}
QString OFileSelectorItem::key( int id, bool )const {
    QString ke;
    if( id == 0 || id == 1 ){ // name
        if( m_isDir ){
            ke.append("0" );
            ke.append( text(1) );
        }else{
            ke.append("1" );
            ke.append( text(1) );
        }
        return ke;
    }else
        return text( id );

}

OFileViewFileListView::OFileViewFileListView( QWidget* parent, const QString& startDir,
                                              OFileSelector* sel)
    : QWidget( parent ), m_sel( sel ) {
    m_all = false;
    QVBoxLayout* lay = new QVBoxLayout( this );
    m_currentDir = startDir;

    /*
     * now we add a special bar
     * One Button For Up
     * Home
     * Doc
     * And a dropdown menu with FileSystems
     * FUTURE: one to change dir with lineedit
     * Bookmarks
     * Create Dir
     */
    QHBox* box = new QHBox(this );
    box->setBackgroundMode( PaletteButton );
    box->setSpacing( 0 );

    QToolButton *btn = new QToolButton( box );
    btn->setIconSet( Resource::loadIconSet("up") );
    connect(btn, SIGNAL(clicked() ),
            this, SLOT( cdUP() ) );

    btn = new QToolButton( box );
    btn->setIconSet( Resource::loadIconSet("home") );
    connect(btn, SIGNAL(clicked() ),
            this, SLOT( cdHome() ) );

    btn = new QToolButton( box );
    btn->setIconSet( Resource::loadIconSet("DocsIcon") );
    connect(btn, SIGNAL(clicked() ),
            this, SLOT(cdDoc() ) );

    m_btnNew = new QToolButton( box );
    m_btnNew->setIconSet( Resource::loadIconSet("new") );
    connect(m_btnNew, SIGNAL(clicked() ),
            this, SLOT(slotNew() ) );


    m_btnClose = new QToolButton( box );
    m_btnClose->setIconSet( Resource::loadIconSet("close") );
    connect(m_btnClose, SIGNAL(clicked() ),
            selector(), SIGNAL(closeMe() ) );

    btn = new QToolButton( box );
    btn->setIconSet( Resource::loadIconSet("cardmon/pcmcia") );

    /* let's fill device parts */
    QPopupMenu* pop = new QPopupMenu(this);
    connect(pop, SIGNAL( activated(int) ),
            this, SLOT(slotFSActivated(int) ) );

    StorageInfo storage;
    const QList<FileSystem> &fs = storage.fileSystems();
    QListIterator<FileSystem> it(fs);
    for ( ; it.current(); ++it ) {
        const QString disk = (*it)->name();
        const QString path = (*it)->path();
        m_dev.insert( disk, path );
        pop->insertItem( disk );
    }
    m_fsPop = pop;


    btn->setPopup( pop );

    lay->addWidget( box );

    m_view = new QListView( this );

    m_view->installEventFilter(this);

    QPEApplication::setStylusOperation( m_view->viewport(),
                                        QPEApplication::RightOnHold);
    m_view->addColumn(" " );
    m_view->addColumn(tr("Name"), 135 );
    m_view->addColumn(tr("Size"), -1 );
    m_view->addColumn(tr("Date"), 60 );
    m_view->addColumn(tr("Mime Type"), -1 );


    m_view->setSorting( 1 );
    m_view->setAllColumnsShowFocus( TRUE );

    lay->addWidget( m_view, 1000 );
    connectSlots();
}
OFileViewFileListView::~OFileViewFileListView() {
}
void OFileViewFileListView::slotNew() {
    DocLnk lnk;
    emit selector()->newSelected( lnk );
}
OFileSelectorItem* OFileViewFileListView::currentItem()const{
    QListViewItem* item = m_view->currentItem();
    if (!item )
        return 0l;

    return static_cast<OFileSelectorItem*>(item);
}
void OFileViewFileListView::reread( bool all ) {
    m_view->clear();

    if (selector()->showClose() )
        m_btnClose->show();
    else
        m_btnClose->hide();

    if (selector()->showNew() )
        m_btnNew->show();
    else
        m_btnNew->hide();

    m_mimes = selector()->currentMimeType();
    m_all = all;

    QDir dir( m_currentDir );
    if (!dir.exists() )
        return;

    dir.setSorting( QDir::Name | QDir::DirsFirst | QDir::Reversed );
    int filter;
    if (m_all )
        filter = QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All;
    else
        filter = QDir::Files | QDir::Dirs | QDir::All;
    dir.setFilter( filter );

    // now go through all files
    const QFileInfoList *list = dir.entryInfoList();
    if (!list) {
        cdUP();
        return;
    }
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while( (fi=it.current() ) ){
      if( fi->fileName() == QString::fromLatin1("..") || fi->fileName() == QString::fromLatin1(".") ){
          ++it;
          continue;
      }

      /*
       * It is a symlink we try to resolve it now but don't let us attack by DOS
       *
       */
      if( fi->isSymLink() ){
          QString file = fi->dirPath( true ) + "/" + fi->readLink();
          for( int i = 0; i<=4; i++) { // 5 tries to prevent dos
              QFileInfo info( file );
              if( !info.exists() ){
                  addSymlink( fi, TRUE );
                  break;
              }else if( info.isDir() ){
                  addDir( fi, TRUE );
                  break;
              }else if( info.isFile() ){
                  addFile( fi, TRUE );
                  break;
              }else if( info.isSymLink() ){
                  file = info.dirPath(true ) + "/" + info.readLink() ;
                  break;
              }else if( i == 4){ // couldn't resolve symlink add it as symlink
                  addSymlink( fi );
              }
        } // off for loop for symlink resolving
      }else if( fi->isDir() )
          addDir(  fi );
      else if( fi->isFile() )
        addFile( fi );

      ++it;
    } // of while loop
  m_view->sort();

}
int OFileViewFileListView::fileCount()const{
    return m_view->childCount();
}
QString OFileViewFileListView::currentDir()const{
    return m_currentDir;
}
OFileSelector* OFileViewFileListView::selector() {
    return m_sel;
}

bool OFileViewFileListView::eventFilter (QObject *o, QEvent *e) {
    if ( e->type() == QEvent::KeyPress ) {
        QKeyEvent *k = (QKeyEvent *)e;
        if ( (k->key()==Key_Enter) || (k->key()==Key_Return)) {
            slotClicked( Qt::LeftButton,m_view->currentItem(),QPoint(0,0),0);
            return true;
        }
    }
    return false;
}


void OFileViewFileListView::connectSlots() {
    connect(m_view, SIGNAL(clicked(QListViewItem*) ),
            this, SLOT(slotCurrentChanged(QListViewItem*) ) );
    connect(m_view, SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint&, int ) ),
            this, SLOT(slotClicked(int, QListViewItem*, const QPoint&, int ) ) );
}
void OFileViewFileListView::slotCurrentChanged( QListViewItem* item) {
    if (!item)
        return;
#if 0

    OFileSelectorItem *sel = static_cast<OFileSelectorItem*>(item);

    if (!sel->isDir() ) {
        selector()->m_lneEdit->setText( sel->text(1) );
        // if in fileselector mode we will emit selected
        if ( selector()->mode() == OFileSelector::FileSelector ) {
            qWarning("slot Current Changed");
            QStringList str = QStringList::split("->", sel->text(1) );
            QString path = sel->directory() + "/" + str[0].stripWhiteSpace();
            emit selector()->fileSelected( path );
            DocLnk lnk( path );
            emit selector()->fileSelected( lnk );
        }
    }
#endif
}
void OFileViewFileListView::slotClicked(int button , QListViewItem* item, const QPoint&, int ) {
    if (!item || ( button != Qt::LeftButton) )
        return;

    OFileSelectorItem *sel = static_cast<OFileSelectorItem*>(item);
    if (!sel->isLocked() ) {
        QStringList str = QStringList::split("->", sel->text(1) );
        if (sel->isDir() ) {
            m_currentDir = sel->directory() + "/" + str[0].stripWhiteSpace();
            emit selector()->dirSelected( m_currentDir );
            reread( m_all );
        }else { // file
            qWarning("slot Clicked");
            selector()->m_lneEdit->setText( str[0].stripWhiteSpace() );
            QString path = sel->directory() + "/" + str[0].stripWhiteSpace();
            emit selector()->fileSelected( path );
            DocLnk lnk( path );
            emit selector()->fileSelected( lnk );
        }
    } // not locked
}
void OFileViewFileListView::addFile( QFileInfo* info, bool symlink ) {
    MimeType type( info->absFilePath() );
    if (!compliesMime( type.id() ) )
        return;

    QPixmap pix = type.pixmap();
    QString dir, name; bool locked;
    if ( pix.isNull() ) {
        QWMatrix matrix;
        QPixmap pixer(Resource::loadPixmap("UnknownDocument") );
        matrix.scale( .4, .4 );
        pix = pixer.xForm( matrix );
    }
    dir = info->dirPath( true );
    locked = false;
    if ( symlink )
        name = info->fileName() + " -> " + info->dirPath() + "/" + info->readLink();
    else{
        name = info->fileName();
        if ( ( (selector()->mode() == OFileSelector::Open)&& !info->isReadable() ) ||
             ( (selector()->mode() == OFileSelector::Save)&& !info->isWritable() ) ) {
            locked = true; pix = Resource::loadPixmap("locked");
        }
    }
    (void)new OFileSelectorItem( m_view, pix, name,
                                 info->lastModified().toString(), QString::number( info->size() ),
                                 dir, locked );
}
void OFileViewFileListView::addDir( QFileInfo* info, bool symlink ) {
    bool locked = false; QString name; QPixmap pix;

    if ( ( ( selector()->mode() == OFileSelector::Open )  && !info->isReadable() ) ||
         ( ( selector()->mode() == OFileSelector::Save )  && !info->isWritable() ) ) {
        locked = true;
        if ( symlink )
            pix = Resource::loadPixmap( "opie/symlink" );
        else
            pix = Resource::loadPixmap( "lockedfolder" );
    }else
        pix = symlink ? Resource::loadPixmap( "opie/symlink") : Resource::loadPixmap("folder");

    name = symlink ? info->fileName() + " -> " + info->dirPath(true) + "/" + info->readLink() :
           info->fileName();

    (void)new OFileSelectorItem( m_view, pix, name,
                                 info->lastModified().toString(),
                                 QString::number( info->size() ),
                                 info->dirPath( true ), locked, true );


}
void OFileViewFileListView::addSymlink( QFileInfo* , bool ) {

}
void OFileViewFileListView::cdUP() {
    QDir dir( m_currentDir );
    dir.cdUp();

    if (!dir.exists() )
        m_currentDir = "/";
    else
        m_currentDir = dir.absPath();

    emit selector()->dirSelected( m_currentDir );
    reread( m_all );
}
void OFileViewFileListView::cdHome() {
    m_currentDir = QDir::homeDirPath();
    emit selector()->dirSelected( m_currentDir );
    reread( m_all );
}
void OFileViewFileListView::cdDoc() {
    m_currentDir = QPEApplication::documentDir();
    emit selector()->dirSelected( m_currentDir );
    reread( m_all );
}
void OFileViewFileListView::changeDir( const QString& dir ) {
    m_currentDir = dir;
    emit selector()->dirSelected( m_currentDir );
    reread( m_all );
}
void OFileViewFileListView::slotFSActivated( int id ) {
    changeDir ( m_dev[m_fsPop->text(id)]  );
}

/*  check if the mimetype in mime
 *  complies with the  one which is current
 */
/*
 * We've the mimetype of the file
 * We need to get the stringlist of the current mimetype
 *
 * mime = image@slashjpeg
 * QStringList = 'image@slash*'
 * or QStringList = image/jpeg;image/png;application/x-ogg
 * or QStringList = application/x-ogg;image@slash*;
 * with all these mime filters it should get acceptes
 * to do so we need to look if mime is contained inside
 * the stringlist
 * if it's contained return true
 * if not ( I'm no RegExp expert at all ) we'll look if a '@slash*'
 * is contained in the mimefilter and then we will
 * look if both are equal until the '/'
 */
bool OFileViewFileListView::compliesMime( const QString& str) {
    if (str.isEmpty() || m_mimes.isEmpty() || str.stripWhiteSpace().isEmpty() )
        return true;

    for (QStringList::Iterator it = m_mimes.begin(); it != m_mimes.end(); ++it ) {
        QRegExp reg( (*it) );
        reg.setWildcard( true );
        if ( str.find( reg ) != -1 )
            return true;

    }
    return false;
}
/*
 * The listView giving access to the file system!
 */
class OFileViewFileSystem : public OFileViewInterface {
public:
    OFileViewFileSystem( OFileSelector* );
    ~OFileViewFileSystem();

    QString selectedName() const;
    QString selectedPath() const;

    QString directory()const;
    void reread();
    int fileCount()const;

    QWidget* widget( QWidget* parent );
    void activate( const QString& );
private:
    OFileViewFileListView* m_view;
    bool m_all : 1;
};
OFileViewFileSystem::OFileViewFileSystem( OFileSelector* sel)
    : OFileViewInterface( sel ) {
    m_view = 0;
    m_all = false;
}
OFileViewFileSystem::~OFileViewFileSystem() {
}
QString OFileViewFileSystem::selectedName()const{
    if (!m_view )
        return QString::null;

    QString cFN=currentFileName();
    if (cFN.startsWith("/")) return cFN;
    return m_view->currentDir() + "/" + cFN;
}
QString OFileViewFileSystem::selectedPath()const{
    return QString::null;
}
QString OFileViewFileSystem::directory()const{
    if (!m_view)
        return QString::null;

    OFileSelectorItem* item = m_view->currentItem();
    if (!item )
        return QString::null;

    return QDir(item->directory() ).absPath();
}
void OFileViewFileSystem::reread() {
    if (!m_view)
        return;

    m_view->reread( m_all );
}
int OFileViewFileSystem::fileCount()const{
    if (!m_view )
        return -1;
    return m_view->fileCount();
}
QWidget* OFileViewFileSystem::widget( QWidget* parent ) {
    if (!m_view ) {
        m_view = new OFileViewFileListView( parent, startDirectory(), selector() );
    }
    return m_view;
}
void OFileViewFileSystem::activate( const QString& str) {
    m_all =  (str != QObject::tr("Files") );


}

/* Selector */
OFileSelector::OFileSelector( QWidget* parent, int mode, int sel,
                              const QString& dirName, const QString& fileName,
                              const MimeTypes& mimetypes,
                              bool showNew,  bool showClose)
    : QWidget( parent, "OFileSelector" )
{
    m_current = 0;
    m_shNew = showNew;
    m_shClose = showClose;
    m_mimeType = mimetypes;
    m_startDir = dirName;

    m_mode = mode;
    m_selector = sel;

    initUI();
    m_lneEdit->setText( fileName );
    initMime();
    initViews();

    QString str;
    switch ( m_selector ) {
    default:
    case Normal:
        str = QObject::tr("Documents");
        m_cmbView->setCurrentItem( 0 );
        break;
    case Extended:
        str = QObject::tr("Files");
        m_cmbView->setCurrentItem( 1 );
        break;
    case ExtendedAll:
        str = QObject::tr("All Files");
        m_cmbView->setCurrentItem( 2 );
        break;
    }
    slotViewChange( str );

}
OFileSelector::OFileSelector( const QString& mimeFilter, QWidget* parent, const char* name,
                              bool showNew, bool showClose )
    : QWidget( parent, name )
{
    m_current = 0;
    m_shNew   = showNew;
    m_shClose = showClose;
    m_startDir = QPEApplication::documentDir();

    if (!mimeFilter.isEmpty() )
        m_mimeType.insert(mimeFilter, QStringList::split(";", mimeFilter ) );

    m_mode = OFileSelector::FileSelector;
    m_selector = OFileSelector::Normal;

    initUI();
    initMime();
    initViews();
    m_cmbView->setCurrentItem( 0 );
    slotViewChange( QObject::tr("Documents") );
}
/*
 * INIT UI will set up the basic GUI
 * Layout: Simple VBoxLayout
 * On top a WidgetStack containing the Views...
 *  - List View
 *  - Document View
 * Below we will have a Label + LineEdit
 * Below we will have two ComoBoxes one for choosing the view one for
 *       choosing the mimetype
 */
void OFileSelector::initUI() {
    QVBoxLayout* lay = new QVBoxLayout( this );

    m_stack = new QWidgetStack( this );
    lay->addWidget( m_stack, 1000 );

    m_nameBox = new QHBox( this );
    (void)new QLabel( tr("Name:"), m_nameBox );
    m_lneEdit = new QLineEdit( m_nameBox );
    m_lneEdit ->installEventFilter(this);
    lay->addWidget( m_nameBox );

    m_cmbBox = new QHBox( this );
    m_cmbView = new QComboBox( m_cmbBox );
    m_cmbMime = new QComboBox( m_cmbBox );
    lay->addWidget( m_cmbBox );
}

/*
 * This will make sure that the return key in the name edit causes dialogs to close
 */

bool OFileSelector::eventFilter (QObject *o, QEvent *e) {
    if ( e->type() == QEvent::KeyPress ) {
        QKeyEvent *k = (QKeyEvent *)e;
        if ( (k->key()==Key_Enter) || (k->key()==Key_Return)) {
            emit ok();
            return true;
        }
    }
    return false;
}

/*
 * This will insert the MimeTypes into the Combo Box
 * And also connect the changed signal
 *
 * AutoMimeTyping is disabled for now. It used to reparse a dir and then set available mimetypes
 */
void OFileSelector::initMime() {
    MimeTypes::Iterator it;
    for ( it = m_mimeType.begin(); it != m_mimeType.end(); ++it ) {
        m_cmbMime->insertItem( it.key() );
    }
    m_cmbMime->setCurrentItem( 0 );

    connect( m_cmbMime, SIGNAL(activated(int) ),
             this, SLOT(slotMimeTypeChanged() ) );

}
void OFileSelector::initViews() {
    m_cmbView->insertItem( QObject::tr("Documents") );
    m_cmbView->insertItem( QObject::tr("Files") );
    m_cmbView->insertItem( QObject::tr("All Files") );
    connect(m_cmbView, SIGNAL(activated( const QString& ) ),
            this, SLOT(slotViewChange( const QString& ) ) );


    m_views.insert( QObject::tr("Documents"), new ODocumentFileView(this) );

    /* see above why add both */
    OFileViewInterface* in = new OFileViewFileSystem( this );
    m_views.insert( QObject::tr("Files"), in );
    m_views.insert( QObject::tr("All Files"), in );
}
OFileSelector::~OFileSelector() {

}
const DocLnk* OFileSelector::selected() {
    DocLnk* lnk = new DocLnk( currentView()->selectedDocument() );
    return lnk;
}
QString OFileSelector::selectedName()const{
       return currentView()->selectedName();
}
QString OFileSelector::selectedPath()const {
    return currentView()->selectedPath();
}
QString OFileSelector::directory()const {
    return currentView()->directory();
}
DocLnk OFileSelector::selectedDocument()const {
    return currentView()->selectedDocument();
}
int OFileSelector::fileCount()const {
    return currentView()->fileCount();
}
void OFileSelector::reread() {
    return currentView()->reread();
}
OFileViewInterface* OFileSelector::currentView()const{
    return m_current;
}
bool OFileSelector::showNew()const {
    return m_shNew;
}
bool OFileSelector::showClose()const {
    return m_shClose;
}
MimeTypes OFileSelector::mimeTypes()const {
    return m_mimeType;
}
int OFileSelector::mode()const{
    return m_mode;
}
int OFileSelector::selector()const{
    return m_selector;
}
QStringList OFileSelector::currentMimeType()const {
    return m_mimeType[m_cmbMime->currentText()];
}
void OFileSelector::slotMimeTypeChanged() {
    reread();
}
void OFileSelector::slotDocLnkBridge( const DocLnk& lnk) {
    m_lneEdit->setText( lnk.name() );
    emit fileSelected( lnk );
    emit fileSelected( lnk.name() );
}
void OFileSelector::slotFileBridge( const QString& str) {
    DocLnk lnk( str );
    emit fileSelected( lnk );
}
void OFileSelector::slotViewChange( const QString& view ) {
    OFileViewInterface* interface = m_views[view];
    if (!interface)
        return;

    interface->activate( view );
    if (m_current)
        m_stack->removeWidget( m_current->widget( m_stack ) );

    static int id = 1;

    m_stack->addWidget( interface->widget(m_stack), id );
    m_stack->raiseWidget( id );

    interface->reread();
    m_current = interface;

    id++;
}
void OFileSelector::setNewVisible( bool b ) {
    m_shNew = b;
    currentView()->reread();
}
void OFileSelector::setCloseVisible( bool b ) {
    m_shClose = b;
    currentView()->reread();
}
void OFileSelector::setNameVisible( bool b ) {
    if ( b )
        m_nameBox->show();
    else
        m_nameBox->hide();
}
