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

class OFileSelectorItem : public QListViewItem {
 public:
  OFileSelectorItem(const QPixmap &pixmap, const QString &path,
		    const QString &date, const QString &size,
		    bool isDir=false ){
    setPixmap(0, pixmap );
    setText(1, path );
    setText(2, size );
    setText(3, date );
    dir = isDir;
  }
  bool isDir()const{
    return dir;
  }
  QString path()const{
    return text(1 );
  }
 private:
  bool dir:1;
};

class OFileSelector : public QWidget {
  Q_OBJECT
 public:
  enum Mode {OPEN=1, SAVE, FILESELECTOR };
  enum Selector{NORMAL=1, EXTENDED };
  OFileSelector(int mode, const QString &dirName, const QString &fileName = Qtring::null, const QStringList mimetypes = QStringList() );
  bool showToolbar() const;
  bool showPermissionBar() const;
  bool showLineEdit()const;
  bool showChooser( )const;
  void setShowToolbar( bool show );
  void setShowPermissionBar( bool show );
  void setShowLineEdit(bool show) ;
  void setShowChooser( bool chooser );
  QCheckBox* permissionCheckbox();
  bool setPermission() const;
  void setPermissionChecked( bool check );
  void setMode( int );
  int mode()const;

  QString selectedName( );
  const DocLnk* selectedDocument()const;

  void reparse(); // re reads the dir
  QString directory();
  int fileCount();

 signals:
  void fileSelected( const DocLnk & );
  void fileSelected( const QString & );
  void closeMe();
  void ok();
  void cancel();
 protected:
  void slotOk();
  void slotCancel();

  int m_mode, m_selector;
  QComboBox *m_location;
  QPushButton *m_homeButton, *m_docButton, *m_hideButton, *m_ok, *m_cancel;
  QPushButton  *m_reread, *m_up;
  QListView *m_View;
  QString m_currentDir;
  FileSelector *m_Select;
  QWidgetStack *m_stack;
  QGridLayout *m_lay;
  QHBox *m_toolbar;
  QHBox *m_ok;
  bool m_shTool:1;
  bool m_shPerm:1;
  bool m_shLne:1;
  bool m_shChooser:1;

 protected:

 private:
  class OFileSelectorPrivate;
  OFileSelectorPrivate *d;

};


#endif
