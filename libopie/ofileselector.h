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

class OFileSelectorItem : public QListViewItem {
 public:
  OFileSelectorItem(QListView *view, const QPixmap &pixmap, const QString &path,
		    const QString &date, const QString &size,
		    bool isDir=false ): QListViewItem(view) {
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
  OFileSelector(QWidget *wid, int mode, int selector, const QString &dirName, const QString &fileName = QString::null, const QStringList mimetypes = QStringList() );

  bool showToolbar() const { return m_shTool;  };
  bool showPermissionBar() const {  return m_shPerm; };
  bool showLineEdit()const { return m_shLne; };
  bool showChooser( )const { return m_shChooser; };
  bool showYesCancel()const { return m_shYesNo;  };
  void setShowYesCancel( bool show );
  void setShowToolbar( bool show );
  void setShowPermissionBar( bool show );
  void setShowLineEdit(bool show) ;
  void setShowChooser( bool chooser );
  QCheckBox* permissionCheckbox();
  bool setPermission() const;
  void setPermissionChecked( bool check );
  void setMode( int );
  int mode()const { return m_mode;  };
  int selector()const { return m_selector; };
  void setSelector( int );
  QString selectedName( );
  const DocLnk* selectedDocument()const;
  void updateLay();

  void reparse(); // re reads the dir
  QString directory();
  int fileCount();

 signals:
  void fileSelected( const DocLnk & );
  void fileSelected( const QString & );
  void closeMe();
  void ok();
  void cancel();

 protected slots:
  void slotOk();
  void slotCancel();
  void slotViewCheck(const QString & );
 protected:
  void init();
  int m_mode, m_selector;
  QComboBox *m_location, *m_mimeCheck, *m_viewCheck;
  QPushButton *m_homeButton, *m_docButton, *m_hideButton, *m_ok, *m_cancel;
  QPushButton  *m_reread, *m_up;
  QListView *m_View;
  QCheckBox *m_checkPerm;

  QString m_currentDir;
  QString m_name;
  QStringList m_mimetypes;

  FileSelector *m_select;
  QWidgetStack *m_stack;
  QVBoxLayout *m_lay;
  QGridLayout *m_Oselector;

  QHBoxLayout *m_boxToolbar;
  QHBoxLayout *m_boxOk;
  QHBoxLayout *m_boxName;
  QHBoxLayout *m_boxView;

  QLineEdit *m_edit;
  QLabel *m_fnLabel;
  bool m_shTool:1;
  bool m_shPerm:1;
  bool m_shLne:1;
  bool m_shChooser:1;
  bool m_shYesNo:1;
  bool m_boCheckPerm:1;

 protected:

 private:
  void delItems();
  void initializeName();
  void initializeYes();
  void initializeChooser();

  class OFileSelectorPrivate;
  OFileSelectorPrivate *d;

};


#endif
