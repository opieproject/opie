/*
  This is based on code and idea of
  L. J. Potter ljp@llornkcor.com
  Thanks a lot


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

#ifndef opiefileselector_h
#define opiefileselector_h

#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qvaluelist.h>

#include <qpe/applnk.h>
#include <qlistview.h>
/** This is OPIEs FileDialog Widget. You can use it
 *  as a dropin replacement of the fileselector and
 *  or use any of the new features.
 *  This is also a complete FileSave and FileLoad widget
 *  If you look for a Dialog check OFileDialog  
 *
 */
class DocLnk;
class QCheckBox;
class QComboBox;
class QPushButton;
class FileSelector;
class QGridLayout;
class QLineEdit;
class QLabel;
class QWidgetStack;
class QHBoxLayout;
class QVBoxLayout;
class QPopupMenu;
class QFileInfo;
// 
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

class OFileSelector : public QWidget {
  Q_OBJECT
 public:
  enum Mode {OPEN=1, SAVE=2, FILESELECTOR=4, DIR=8 };
  enum Selector{NORMAL=1, EXTENDED = 2, EXTENDED_ALL =4 };
  enum View { DIRS = 1, FILES = 2, TREE = 4, ICON = 8 };
  OFileSelector(QWidget *wid, int mode, int selector, const QString &dirName, const QString &fileName = QString::null, const QStringList &mimetypes = QStringList() );
  ~OFileSelector() {};
  bool isToolbarVisible() const { return m_shTool;  };
  bool isPermissionBarVisible() const {  return m_shPerm; };
  bool isLineEditVisible()const { return m_shLne; };
  bool isChooserVisible( )const { return m_shChooser; };
  bool isYesCancelVisible()const { return m_shYesNo;  };
  void setYesCancelVisible( bool show );
  void setToolbarVisible( bool show );
  void setPermissionBarVisible( bool show );
  void setLineEditVisible(bool show) ;
  void setChooserVisible( bool chooser );

  QCheckBox* permissionCheckbox();
  bool setPermission() const;
  void setPermissionChecked( bool check );

  void setMode( int );

  bool showDirs()const { return m_dir; }
  void setShowDirs(bool );

  const QListView* listView() { return m_View; };

  bool isCaseSensetive()const {  return m_case; }
  void setCaseSensetive(bool caSe );

  bool showFiles()const { return m_files; };
  void setShowFiles(bool );
  bool cd(const QString &path );


  int mode()const { return m_mode;  };
  int selector()const { return m_selector; };
  void setSelector( int );
  
  bool showPopup()const { return m_showPopup; };
  void setShowPopup( bool pop ) { m_showPopup = pop; };
  void setPopupMenu( QPopupMenu * );

  void updateLay();

  void reparse(); // re reads the dir

  QString selectedName( )const;
  QStringList selectedNames()const;

  QString selectedPath() const;
  QStringList selectedPaths() const;

  QString directory()const;
  int fileCount();

  /* the user needs to delete it */
  DocLnk selectedDocument()const;
  /* the user needs to delete it */
  QValueList<DocLnk> selectedDocuments()const;

 signals:
  void fileSelected( const DocLnk & );
  void fileSelected( const QString & );
  void dirSelected(const QString &dir );
  void closeMe();
  void ok();
  void cancel();

 protected slots:
  void slotOk();
  void slotCancel();
  void slotViewCheck(const QString & );
  void slotMimeCheck(const QString & );
  void locationComboActivated(const QString & );
  void insertLocationPath(const QString &, int);
  void locationComboChanged();
 protected:
  void init();
  void updateMimes();
  
 protected:

 private:
  int m_mode, m_selector;
  QComboBox *m_location, *m_mimeCheck, *m_viewCheck;
  QPushButton *m_homeButton, *m_docButton, *m_hideButton, *m_ok, *m_cancel;
  QPushButton  *m_reread, *m_up;
  QListView *m_View;
  QCheckBox *m_checkPerm;
  QWidget *m_pseudo;
  QVBoxLayout *m_pseudoLayout;

  QString m_currentDir;
  QString m_name;
  QStringList m_mimetypes, requestedMimeTypesList;

  FileSelector *m_select;
  QWidgetStack *m_stack;
  QVBoxLayout *m_lay;
  QGridLayout *m_Oselector;

  QHBoxLayout *m_boxToolbar;
  QHBoxLayout *m_boxOk;
  QHBoxLayout *m_boxName;
  QHBoxLayout *m_boxView;

  QPopupMenu *m_custom;

  QLineEdit *m_edit;
  QLabel *m_fnLabel;
  bool m_shTool:1;
  bool m_shPerm:1;
  bool m_shLne:1;
  bool m_shChooser:1;
  bool m_shYesNo:1;
  bool m_boCheckPerm:1;
  bool m_autoMime:1;
  bool m_case:1;
  bool m_dir:1;
  bool m_files:1;
  bool m_showPopup:1;

  // implementation todo
  virtual void addFile(const QString &mime, QFileInfo *info, bool symlink = FALSE );
  virtual void addDir( const QString &mime, QFileInfo *info , bool symlink = FALSE );
  virtual void addSymlink(const QString &, QFileInfo *, bool = FALSE ){};
  void delItems();
  void initializeName();
  void initializeYes();
  void initializeChooser();
  void initializeListView();
  void initPics();
  bool compliesMime(const QString &path, const QString &mime);

  class OFileSelectorPrivate;
  OFileSelectorPrivate *d;
  static QMap<QString,QPixmap> *m_pixmaps;
 
private slots:
   void slotFileSelected(const QString & ); // not really meant to be a slot
   void slotFileBridgeSelected( const DocLnk & );
   virtual void slotSelectionChanged();
   virtual void slotCurrentChanged(QListViewItem* );
   virtual void slotClicked( int, QListViewItem *item, const QPoint &, int);
   virtual void slotRightButton(int, QListViewItem *, const QPoint &, int );
   virtual void slotContextMenu( QListViewItem *item);
   // listview crap see above
   // PopupMenu crap
   virtual void slotChangedDir();
   virtual void slotOpen();
   virtual void slotRescan();
   virtual void slotRename();
   virtual void slotDelete();
   virtual void cdUP();
   virtual void slotHome();
   virtual void slotDoc();
   virtual void slotNavigate( );
};


#endif

