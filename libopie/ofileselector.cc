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

#include <qpe/qpeapplication.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/mimetype.h>

#include "ofileselector.h"


OFileSelector::OFileSelector(QWidget *wid, int mode, int selector, const QString &dirName,
			     const QString &fileName, const QStringList mimetypes ) : QWidget( wid )
{
  m_selector = selector;
  m_currentDir = dirName;
  m_name = fileName;
  m_mimetypes = mimetypes;
  if( mimetypes.isEmpty() )
    m_autoMime = true;

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

  m_lay = new QVBoxLayout(this);
  init();
  m_edit->setText( fileName );
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
  m_select = new FileSelector(m_mimetypes.join(";"), m_stack, "fileselector", FALSE, FALSE );
  m_stack->addWidget(m_select, NORMAL );
  m_lay->addWidget(m_stack );
  m_stack->raiseWidget(NORMAL );

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
 
  
};

void OFileSelector::setShowYesCancel( bool show )
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

void OFileSelector::setShowToolbar( bool show )
{
  if ( m_shTool == show )
    return;
  /*  if( show ){

  }else {

  }*/
}

void OFileSelector::setShowPermissionBar( bool show )
{
  if( show == m_shPerm )
    return;

  m_shPerm = show;

  updateLay();
}
void OFileSelector::setShowLineEdit( bool show )
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
void OFileSelector::setShowChooser( bool show )
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
QString OFileSelector::selectedName( )
{
  QString string;
  return string;
}
const DocLnk* OFileSelector::selectedDocument( )const
{
  return 0;
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
  delete m_edit;
  delete m_fnLabel;
  delete m_ok;
  delete m_cancel;
  delete m_mimeCheck;
  delete m_viewCheck;
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
  if(m_View== 0)
    return;

  m_View->clear();
 
  QDir dir( m_currentDir );
  QString currMime =m_mimeCheck->currentText();
  // update the mimetype now
  if( m_autoMime ) {
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
    m_mimeCheck->insertStringList(m_mimetypes );
    // set it to the current mimetype
  };
  dir.setFilter(QDir::All );
  dir.setSorting(QDir::Name | QDir::DirsFirst  );
  const QFileInfoList *list = dir.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  while( (fi=it.current())  ){
    if(fi->isSymLink() ){
      QString file = fi->readLink();
      for(int i=0; i<=4; i++ ){ // prepend from dos
	QFileInfo info( file );
	if( !info.exists() ){
	  addSymlink(m_currentDir, info.fileName(), TRUE );
	  break;
	}else if( info.isDir() ){ 
	  //addDir( );
	}else if( info.isFile() ){
	  
	}else if( info.isSymLink() ){
	  file = info.readLink();
	}else if( i == 4 ){ // just insert it and have the symlink symbol
	  addSymlink(m_currentDir, info.fileName() );
	}
      }
    }else if( fi->isDir() ){

    }else if( fi->isFile() ) { // file ?


    }
    ++it;  
  }
}
QString OFileSelector::directory()
{
  QString string;
  return string;
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
  m_boxView->insertSpacing(1, 8 );
  m_boxView->addWidget(m_mimeCheck, 0 );
  m_lay->addLayout(m_boxView );

  m_viewCheck->insertItem(tr("Documents") );
  m_viewCheck->insertItem(tr("Files") );
  m_viewCheck->insertItem(tr("All Files") );

  if(!m_autoMime )
    m_mimeCheck->insertItem(m_mimetypes.join("," ) );
  else{ // check
    updateMimes();
    m_mimeCheck->insertStringList( m_mimetypes );
  }

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


  }
}

void OFileSelector::slotViewCheck(const QString &view ){
  qWarning("changed: show %s", view.latin1() );
  // if the current view is the one
  QString currMime = m_mimeCheck->currentText();
  if( view == QString::fromLatin1("Documents") ){
    // get the mimetype now
    // check if we're the current widget and return
    if( m_View != 0) // delete 0 shouldn't crash but it did :( 
      delete m_View;
    m_View = 0;
    delete m_select;
    m_select = new FileSelector( currMime == "All" ? QString::null : currMime,
				 m_stack,"fileselector", FALSE, FALSE );
    m_stack->addWidget( m_select, NORMAL );
    m_stack->raiseWidget( NORMAL );
    m_selector = NORMAL;

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
};


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

  }
};
void OFileSelector::initializeListView()
{
  m_View = new QListView(m_stack, "Extended view" );
  m_stack->addWidget( m_View, EXTENDED );
  m_stack->raiseWidget( EXTENDED );
  QPEApplication::setStylusOperation( m_View->viewport(),QPEApplication::RightOnHold);
  // set up the stuff
  // Pixmap Name Date Size mime
  //(m_View->header() )->hide();
  //m_View->setRootIsDecorated(false);
  m_View->addColumn(" ");
  m_View->addColumn(tr("Name") );
  m_View->addColumn(tr("Size") );
  m_View->addColumn(tr("Date"), 60 );
  m_View->addColumn(tr("Mime Type") );
  QHeader *header = m_View->header();
  header->hide();
};








