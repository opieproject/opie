/*
  This is based on code and ideas of
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

#include <qpe/fileselector.h>

#include <qdir.h>
#include <qguardedptr.h>
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
class QGridLayout;
class QLineEdit;
class QLabel;
class QWidgetStack;
class QHBoxLayout;
class QVBoxLayout;
class QPopupMenu;
class QFileInfo;
class QHBox;
class OFileView;
class OLister;
class OFileSelectorMain;
class OFileFactory;
//

/* the mimetypes one name and a list of mimetypes */
typedef QMap< QString, QStringList> MimeTypes;

/**
 * FIXME later
 */
struct OPopupMenuFactory {
    OPopupMenuFactory() {}

};


/**
 * Opie the default OFileSelector
 * It features multiple views.
 */
class OFileSelector : public QWidget {
  Q_OBJECT

  /* friends are evil but I don't want to make the
   * methods public
   */
  friend class OLister;
  friend class OFileView;
 public:
  /**
   *  The mode of the file selector
   *  Either open, save, fileselector or dir browsing mode
   *
   */
  enum Mode {Open = 1, Save = 2, Fileselector = 4, Dir = 8 };

  /**
   * Selector. Either Normal for the one shipped with
   * libqpe or Extended. for the Extended
   * ExtendedAll also shows 'hidden' files
   */
  enum Selector{Normal=0, Extended = 1, ExtendedAll = 2};

  /**
   *  This is reserved for futrue views
   */
  enum View { Dirs = 1, Files = 2, Tree = 4, Icon = 8 };

  /**
   *  A c'tor which should be used for advanced mode
   *  @param wid the parent
   *  @param mode the Mode of the Selector
   *  @param selector the current View of the Selector
   *  @param dirName in which dir to start
   *  @param fileName  a proposed filename
   *  @param mimetypes A list of mimetypes \
   *   QString is for a identifier name like "Text files"
   *   the coresponding QStringList is used for the mimetypes
   *   if empty it'll fill the list of mimetypes depending
   *   on the content of the current directory
   */

  OFileSelector(QWidget *wid, int mode, int selector,
		const QString &dirName,
		const QString &fileName = QString::null,
                const MimeTypes &mimetypes = MimeTypes() );


  /**
   * This is a QPE compatible c'tor
   */
  OFileSelector(const QString &mimeFilter, QWidget *parent,
		const char *name, bool newVisible = TRUE,
		bool closeVisible = FALSE );

  ~OFileSelector();

  // currently only for the FileSelector Mode
  /* compability mode but only work
   * with FileSelector
   */
  void setNewVisible( bool /*b*/ );
  void setCloseVisible(bool /*b*/ );

  // end file selector mode
  // deprecated
  void reread();
  // make sure not to leak please
  const DocLnk *selected();
  // end deprecated

  /**
   * @return if the toolbar is visible
   */
  bool isToolbarVisible() const { return m_shTool;  };

  /**
   * @return if the permissionBas is visible
   */
  bool isPermissionBarVisible() const {  return m_shPerm; };

  /**
   * @return if the lineEdit is visible
   */
  bool isLineEditVisible()const { return m_shLne; };

  /**
   * if the chooser is visible
   */
  bool isChooserVisible( )const { return m_shChooser; };

  /**
   * @return if the yesCancel Bar is visible
   */
  bool isYesCancelVisible()const { return m_shYesNo;  };

  /**
   * set Yes/Cancel visible
   */
  void setYesCancelVisible( bool show );

  /**
   * set the toolbar visible
   */
  void setToolbarVisible( bool show );

  /**
   * set the permissionBar to be visible
   */
  void setPermissionBarVisible( bool show );

  /**
   * set the lineedit for file entering visible
   */
  void setLineEditVisible(bool show) ;

  /**
   * set the chooser is visible
   */
  void setChooserVisible( bool chooser );

  /**
   * The permissionCheckbox
   *
   */
  QCheckBox* permissionCheckbox();

  /**
   * setPermission
   */
  bool setPermission() const;

  /**
   * set ther permission to bool
   */
  void setPermissionChecked( bool check );

  /**
   * set the Selector Mode
   */
  void setMode( int );

  void setLister( const QString& name );
  void setView( const QString& all );
  /**
   * whether or not to show dirs
   */
  bool showDirs()const { return m_dir; }

  /**
   * setShowDirs
   */
  void setShowDirs(bool );

  /**
   * set CaseSensetive
   */
  bool isCaseSensetive()const {  return m_case; }

  /**
   * set if to be case sensetive
   */
  void setCaseSensetive(bool caSe );

  /**
   * @return if to show files
   */
  bool showFiles()const { return m_files; };

  /**
   * set if files should be shown
   */
  void setShowFiles(bool );

  /**
   * change dir to path
   */
  bool cd(const QString &path );


  /**
   * return the mode of the fileselector
   */
  int mode()const { return m_mode;  };

  /**
   * return the selector
   */
  int selector()const { return m_selector; };

  /**
   * set the Selector
   */
  void setSelector( int );

  /**
   * wether or not to show popups
   */
  bool showPopup()const { return m_showPopup; };

  /**
   * set show popups
   */
  void setShowPopup( bool pop ) { m_showPopup = pop; }

  /**
   * set the popup factory
   */
  void setPopupFactory( OPopupMenuFactory * );

  /**
   * reparse the current directory and updates
   * the views + mimetypes
   */
  void reparse(); // re reads the dir

  /**
   * return the selected name
   */
  QString selectedName( )const;

  /**
   * for multiple selections return multiple
   * filenames
   */
  QStringList selectedNames()const;

  /**
   * return the complete to the file
   */
  QString selectedPath() const;

  /**
   * return the completed paths
   */
  QStringList selectedPaths() const;

  /**
   * the current directory
   */
  QString directory()const;

  /**
   * fileCount
   */
  int fileCount();

  DocLnk selectedDocument()const;

  QValueList<DocLnk> selectedDocuments()const;

  OFileView* currentView();
  OFileView* currentView()const;
  OLister* currentLister()const;
  OFileFactory* factory();
  int filter();
  int sorting();
  QPixmap pixmap( const QString& );
    /* our tool bar */
  QWidget* toolBar();

 signals:
  void fileSelected( const DocLnk & );
  void fileSelected( const QString & );
  void dirSelected(const QString &dir );
  void dirSelected( const QDir& );
  void closeMe();
  void ok();
  void cancel();
  void contextMenu();

 private slots:
  void slotOk();
  void slotCancel();
  void slotViewCheck(const QString & );
  void slotMimeCheck(const QString & );
  void slotLocationActivated(const QString & );
  void slotInsertLocationPath(const QString &, int);
  void locationComboChanged();

 private:
  void init();
  void updateMimes();


 private:

    OFileFactory* m_fileFactory;
    OFileSelectorMain* m_mainView;
    OLister* m_lister;
    QString m_listerName;
    OFileView* m_fileView;
    FileSelector* m_select;
    int m_mode, m_selector;
    QComboBox *m_location,
      *m_mimeCheck,
      *m_viewCheck;

    QPushButton *m_homeButton,
      *m_docButton,
      *m_hideButton,
      *m_ok, *m_cancel;
    QPushButton  *m_reread,
      *m_up,
      *m_new,
      *m_close;
    QListView *m_View;
    QCheckBox *m_checkPerm;
    QWidget *m_pseudo;
    QVBoxLayout *m_pseudoLayout;

    QString m_currentDir;
    QString m_name;

    QMap<QString, QStringList> m_mimetypes;

    QVBoxLayout *m_lay;
    QGridLayout *m_Oselector;

    QHBox *m_boxToolbar;
    QHBox *m_boxOk;
    QHBox *m_boxName;
    QHBox *m_boxView;


    QLineEdit *m_edit;
    QLabel *m_fnLabel;

    bool m_shClose     : 1;
    bool m_shNew       : 1;
    bool m_shTool      : 1;
    bool m_shPerm      : 1;
    bool m_shLne       : 1;
    bool m_shChooser   : 1;
    bool m_shYesNo     : 1;
    bool m_boCheckPerm : 1;
    bool m_autoMime    : 1;
    bool m_case        : 1;
    bool m_dir         : 1;
    bool m_files       : 1;
    bool m_showPopup   : 1;
    bool m_showHidden : 1;

    void initVars();

    void delItems();
    void initializeName();
    void initializeYes();
    void initializeChooser();
    void initializePerm();
    void initPics();
    bool compliesMime(const QString &path,
                      const QString &mime);
    bool compliesMime(const QString& mime );
    /**
     * Updates the QComboBox with the current mimetypes
     */
    void updateMimeCheck();

    void initializeOldSelector();
    void initToolbar();
    void initLocations();
    void initializeView(); // FIXME
    void fillList();
    void initFactory();
    /**
     * Returns the current mimetype
     */
    QString currentMimeType()const;
    class OFileSelectorPrivate;
    OFileSelectorPrivate *d;
    static QMap<QString,QPixmap> *m_pixmaps;

private slots:
    void slotFileSelected(const QString & ); // not really meant to be a slot
    void slotFileBridgeSelected( const DocLnk & );
    // listview above
    // popup below
    virtual void slotDelete();
    virtual void cdUP();
    virtual void slotHome();
    virtual void slotDoc();
    virtual void slotNavigate( );

    /* for OLister */
private:

    /* for OFileView */
private:
    void internFileSelected( const QString& );
    void internFileSelected( const DocLnk& );
    void internContextMenu();
    void internChangedDir( const QString& );
    void internChangedDir( const QDir& ) ;

};


#endif

