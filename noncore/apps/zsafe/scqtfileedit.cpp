/***************************************************************************
                          scqtfileedit.cpp  -  description
                             -------------------
    begin                : Mon Sep 3 2001
    copyright            : (C) 2001 by Werner Schulte
    email                : sc@schulte-ac.de
 ***************************************************************************/

/* $Id: scqtfileedit.cpp,v 1.3 2004-03-01 20:04:34 chicken Exp $ */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "qlineedit.h"
#include "qtoolbutton.h"

#include "scqtfileedit.h"

#include <qpe/qpeapplication.h>

// #define DEBUGFILEEDIT

/* XPM */
static const char* const file_xpm[]={
    "16 16 5 1",
    ". c #7f7f7f",
    "# c None",
    "c c #000000",
    "b c #bfbfbf",
    "a c #ffffff",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".bbbbbbbbbbbc###",
    "ccccccccccccc###"};

/* XPM */
static const char * const link_file_xpm[]={
    "16 16 10 1",
    "h c #808080",
    "g c #a0a0a0",
    "d c #c3c3c3",
    ". c #7f7f7f",
    "c c #000000",
    "b c #bfbfbf",
    "f c #303030",
    "e c #585858",
    "a c #ffffff",
    "# c None",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaadc###",
    ".aaaaaaaaaadc###",
    ".aaaacccccccc###",
    ".aaaacaaaaaac###",
    ".aaaacaeaeaac###",
    ".aaaacaefcfac###",
    ".aaaacaagchac###",
    ".ddddcaaahaac###",
    "ccccccccccccc###"};

/* XPM */
static const char * const closed_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "a c #cccccc",
    "c c #ffffff",
    "................",
    "................",
    "..*****.........",
    ".*ababa*........",
    "*abababa******..",
    "*cccccccccccc*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "**************d.",
    ".dddddddddddddd.",
    "................"};

/* XPM */
static const char* const cdtoparent_xpm[]={
    "15 13 3 1",
    ". c None",
    "* c #000000",
    "a c #ffff99",
    "..*****........",
    ".*aaaaa*.......",
    "***************",
    "*aaaaaaaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaa***aaaaaaa*",
    "*aa*****aaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa******aaa*",
    "*aaaaaaaaaaaaa*",
    "*aaaaaaaaaaaaa*",
    "***************"};

ScQtFileEditDlg::ScQtFileEditDlg( QWidget *parent, const char *name,
                          const QString &path, const QString &filter )
          : ScQtFileDlg( parent, name, TRUE ), QDir( path, filter )
{
//  ListView->setRootIsDecorated ( TRUE );

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg:: constructor name=%s", name );
  printf ("\n   path=%s", (const char *)path );
  printf ("\n   filter=%s", (const char *)filter );
  printf ("\n   parent=%p", parent );
  fflush(stdout);
#endif

  bpath    = path;
  bfilter  = filter;
  exflag   = false;

  ListView->setAllColumnsShowFocus( TRUE );
  ListView->setColumnAlignment( 1, Qt::AlignRight );

  fileIcon = new QPixmap( (const char **)file_xpm);
  dirIcon = new QPixmap( (const char **)closed_xpm);
  linkIcon = new QPixmap( (const char **)link_file_xpm);
  cdToParentIcon = new QPixmap( (const char **)cdtoparent_xpm);

#ifdef QWS
  QPEApplication::execDialog( this );
#endif

  mkdirflag = false;
  MkDirButton->setEnabled( false );

  initDirCombo( bpath );
  initTypeCombo( bfilter );

  dirstr  = tr("dir");
  filestr = tr("file");
  linkstr = tr("link");

  rFlag = false;

  showTimer.stop();
  connect( &showTimer, SIGNAL(timeout()), SLOT( slotShowDir() ) );

  fmode = QDir::All;
  fnfilter = false;
  fsorting = false;

}

ScQtFileEditDlg::~ScQtFileEditDlg()
{
#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::destructor called" );
  fflush(stdout);
#endif

  if ( fileIcon != NULL )
  {
    delete fileIcon;
    fileIcon = NULL;
  }
  if ( dirIcon != NULL )
  {
    delete dirIcon;
    dirIcon = NULL;
  }
  if ( linkIcon != NULL )
  {
    delete linkIcon;
    linkIcon = NULL;
  }
  if ( cdToParentIcon != NULL )
  {
    delete cdToParentIcon;
    cdToParentIcon = NULL;
  }
}

void ScQtFileEditDlg::readIt( )
{
  qfil = readDir( bpath, bfilter );
  slotShowDir( );
}

void ScQtFileEditDlg::setAutoExtension( bool ex )
{
  exflag = ex;
}


int ScQtFileEditDlg::checkComboEntryExists( QComboBox *com, const QString &str )
{
  int i;

  if ( com )
  {
    for ( i = 0; i < com->count(); i++ )
    {
      if ( com->text( i ) == str )
      {
        com->setCurrentItem( i );
        return( 1 );
      }
    }
  }

  return( 0 );
}


void ScQtFileEditDlg::initDirCombo( const QString &str )
{
  DirComboBox->clear();
  DirComboBox->insertItem( "/" );
  if ( str.isEmpty() )
    return;
  DirComboBox->insertItem( str, -1 );
}

void ScQtFileEditDlg::initTypeCombo( const QString &str )
{
  TypeComboBox->clear();
  TypeComboBox->insertItem( "*" );
  if ( str.isEmpty() )
    return;
  TypeComboBox->insertItem( str, -1 );
}

void ScQtFileEditDlg::insDirCombo( const QString &str )
{
  if ( str.isEmpty() )
    return;

  if ( !checkComboEntryExists( DirComboBox, str ) )
  {
    if ( DirComboBox->count() >= MAXDIRCOMBOCOUNT )
    {
      // 2. Item loeschen (das erste ist "/")
      DirComboBox->removeItem( 1 );
    }
    // neues Item anhaengen
    DirComboBox->insertItem( str, -1 );
    DirComboBox->setCurrentItem( DirComboBox->count() - 1 );
  }
}

void ScQtFileEditDlg::insTypeCombo( const QString &str )
{
  if ( str.isEmpty() )
    return;

  if ( !checkComboEntryExists( TypeComboBox, str ) )
  {
    if ( TypeComboBox->count() >= MAXTYPECOMBOCOUNT )
    {
      // 2. Item loeschen (das erste ist "/")
      TypeComboBox->removeItem( 1 );
    }
    // neues Item anhaengen
    TypeComboBox->insertItem( str, -1 );
    TypeComboBox->setCurrentItem( TypeComboBox->count() - 1 );
  }
}


const QFileInfoList *ScQtFileEditDlg::readDir( const QString &path, const QString &filter )
{
    static QString qpath, qfilter;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::readDir called, path = %s", (const char *)path );
  printf ("\n                             filter = %s", (const char *)filter );
  fflush(stdout);
#endif

    rFlag = true;

    if ( path.isEmpty() )
      qpath = "/";
    else
      qpath = path;

    if ( filter.isEmpty() )
      qfilter = "*";
    else
      qfilter = filter;

    insDirCombo( qpath );
    insTypeCombo( qfilter );

    setFilter( fmode );
    if ( fsorting )
      setSorting(  QDir::DirsFirst | QDir::Name );
    if ( fnfilter )
      setNameFilter( qfilter );
    setPath( qpath );

    const QFileInfoList *dirlist = entryInfoList();

    if ( !dirlist )
    {
      rFlag = false;
      return( 0 );
    }

    rFlag = false;

    return( dirlist );
}

QString ScQtFileEditDlg::getResult( )
{
  static QString qstr;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::getResult called" );
  fflush(stdout);
#endif

  qstr = bpath + "/";
  qstr = qstr + filename;
  bpath = cleanDirPath( qstr );

  return( bpath );
}

QString ScQtFileEditDlg::getFileName( )
{
  int a;
  static QString qstr;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::getFileName called" );
  fflush(stdout);
#endif

  a = bpath.findRev( "/", -1, TRUE );
  if (a == -1)
    qstr = bpath;  // Nicht gefunden
  else
    qstr = bpath.right( bpath.length() - a - 1 );

  return( qstr );
}

// Ab hier die Slots -----------------------------------

void ScQtFileEditDlg::slotShowDir()
{
    static QListViewItem *qlvitem;
    static QString ftypestr, fsizestr;
    static QFileInfo *fi;                   // pointer for traversing

    if ( !qfil )                            // pointer war Null
      return;

    ListView->clear();  // ersma loeschen
    QFileInfoListIterator it( *qfil );             // create list iterator
    if ( !it )                                     // pointer war Null
      return;

    while ( (fi=it.current()) )
    {           // for each file...
       if ( !fi )   // current war 0
         return;

       if ( !fi->fileName().isNull() )
       {
         if ( fi->isDir() == TRUE )
         {
           ftypestr = dirstr;
           fsizestr = "" ;
         }
         else if ( fi->isSymLink() == TRUE )
         {
           ftypestr = linkstr;
           fsizestr = "" ;
         }
         else if ( fi->isFile() == TRUE )
         {
           ftypestr = filestr;
           fsizestr.sprintf( "%d", fi->size() );
         }
         else
         {
           printf( "something else\n" );
           fflush(stdout);
         }

         if ( !(fi->fileName() == "." ) )   // der wird ausgespart
         {
           qlvitem = new QListViewItem ( ListView, fi->fileName(), fsizestr, ftypestr );
           if ( fi->fileName() == ".." )
             qlvitem->setPixmap( 0, *cdToParentIcon );
           else if ( fi->isDir() == TRUE )
             qlvitem->setPixmap( 0, *dirIcon );
           else if ( fi->isSymLink() == TRUE )
             qlvitem->setPixmap( 0, *linkIcon );
           else if ( fi->isFile() == TRUE )
             qlvitem->setPixmap( 0, *fileIcon );
         }
      }
       ++it;                               // goto next list element
    }
}

void ScQtFileEditDlg::slotDirComboBoxChanged( int item )
{
  if ( !rFlag )
  {
    bpath = DirComboBox->currentText( );
    qfil = readDir( bpath, bfilter );
    slotShowDir( );
  }
}

void ScQtFileEditDlg::slotTypeComboBoxChanged( int item )
{
  if ( !rFlag )
  {
    bfilter = TypeComboBox->currentText( );
    qfil = readDir( bpath, bfilter );
    slotShowDir( );
  }
}

void ScQtFileEditDlg::slotFileTextChanged( const QString &txt )
{
  filename = txt;
}

void ScQtFileEditDlg::slotSelectionChanged( QListViewItem *item )
{
  static QString qstr, rstr;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::slotSelectionChanged called" );
  fflush(stdout);
#endif
  if ( item )
  {
    if ( !item->text(2) )
      return;

    if ( item->text(2) == dirstr )
    {
      if ( !rFlag )
      {
        rstr = bpath;
        qstr = bpath + "/";
        qstr = qstr + item->text(0);
        bpath = cleanDirPath( qstr );
        qfil = readDir( bpath, bfilter );
        if ( qfil == 0 )
        {
          bpath = rstr;
          // Nochmal lesen, um die ComboBoxen zu setzten
          qfil = readDir( bpath, bfilter );
        }
        showTimer.start( 10, true );
      }
    }
    else if ( item->text(2) == filestr )
    {
      FNameLineEdit->setText( item->text(0) );
      filename = item->text(0);
    }
  }
}


void ScQtFileEditDlg::slotMkDir( )
{
  QString qstr;

  if ( !FNameLineEdit->text().isEmpty() )
  {
    qstr = bpath + "/";
    qstr = qstr + FNameLineEdit->text();
    mkdir( qstr, true );
    qfil = readDir( bpath, bfilter );
    FNameLineEdit->setText( "" );
    slotShowDir( );
  }
}

void ScQtFileEditDlg::slotDoubleClicked( QListViewItem *item )
{
  static QString qstr, rstr;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::slotDoubleClicked called" );
  fflush(stdout);
#endif

  if ( item )
  {
    if ( item->text(2) == dirstr )
    {
      if ( !rFlag )
      {
        rstr = bpath;
        qstr = bpath + "/";
        qstr = qstr + item->text(0);
        bpath = cleanDirPath( qstr );
        qfil = readDir( bpath, bfilter );
        if ( qfil == 0 )
        {
          bpath = rstr;
          // Nochmal lesen, um die ComboBoxen zu setzten
          qfil = readDir( bpath, bfilter );
        }
        showTimer.start( 10, TRUE );
      }
    }
  }
}

void ScQtFileEditDlg::autoExtension( )
{
  QString qstr;
  int len;

  if ( exflag )
  {
    qstr = TypeComboBox->currentText( );
    if ( qstr != "*" )
    {
      len = qstr.findRev( ".", -1 );
      if ( len >= 0 )
      {
        qstr = qstr.right( qstr.length() - len );
        len = filename.findRev( qstr, -1 );
        if ( len < 0 )
          filename = filename + qstr;
      }
    }
  }
}

void ScQtFileEditDlg::setMode( int mode )
{
  fmode = mode;
}

void ScQtFileEditDlg::setNFilter( bool ff )
{
  fnfilter = ff;
}

void ScQtFileEditDlg::setFSorting( bool ff )
{
  fsorting = ff;
}

void ScQtFileEditDlg::allowMkDir( bool mkdir )
{
    mkdirflag = mkdir;
    MkDirButton->setEnabled( mkdir );
}


void ScQtFileEditDlg::slotOK( )
{
#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::slotOK called" );
  fflush(stdout);
#endif
  autoExtension();

  accept();
}

void ScQtFileEditDlg::slotCancel( )
{

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEditDlg::slotCancel called" );
  fflush(stdout);
#endif

  reject();
}


/**************************************************************************
***************************************************************************/
ScQtFileEdit::ScQtFileEdit( QWidget *parent, const char *name )
{
#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEdit constructor called");
  fflush(stdout);
#endif
}

ScQtFileEdit::~ScQtFileEdit()
{
#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEdit destructor called");
  fflush(stdout);
#endif
}

QString ScQtFileEdit::getOpenFileName( QWidget *parent, const char *name,
                                       const QString &path, const QString &filter )
{
  static ScQtFileEditDlg *fd;
  static QString fname;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEdit::getOpenFileName name=%s", name );
  printf ("\n  path=%s", (const char *)path );
  printf ("\n  filter=%s", (const char *)filter );
  fflush(stdout);
#endif

  fd = new ScQtFileEditDlg( parent, name, path, filter );
  fd->setAutoExtension( false );
  fd->setMode( QDir::All|QDir::System );
  fd->setNFilter( true );
  fd->setFSorting( true );
  fd->readIt();
  fd->exec();

  if ( fd->result() == QDialog::Accepted )
    fname = fd->getResult();
  else
    fname = "";

  delete fd;
  return( fname );
}

QString ScQtFileEdit::getSaveAsFileName( QWidget *parent, const char *name,
                                const QString &path, const QString &filter )
{
  static ScQtFileEditDlg *fd;
  static QString fname;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEdit::getSaveAsFileName name=%s", name );
  printf ("\n  path=%s", (const char *)path );
  printf ("\n  filter=%s", (const char *)filter );
  fflush(stdout);
#endif

  fd = new ScQtFileEditDlg( parent, name, path, filter );
  fd->allowMkDir( true ); // CS:
  fd->setAutoExtension( true );
  fd->setNFilter( true );
  fd->setFSorting( true );
  fd->readIt();
  fd->exec();

  if ( fd->result() == QDialog::Accepted )
    fname = fd->getResult();
  else
    fname = "";

  delete fd;
  return( fname );
}

QString ScQtFileEdit::getDirName( QWidget *parent, const char *name,
                              const QString &path )
{
  static ScQtFileEditDlg *fd;
  static QString fname;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEdit::getDirName name=%s", name );
  printf ("\n  path=%s", (const char *)path );
  fflush(stdout);
#endif

  fd = new ScQtFileEditDlg( parent, name, path );
  fd->setMode( QDir::Dirs );
  fd->setNFilter( false );
  fd->setFSorting( true );
  fd->readIt();
  fd->exec();

  if ( fd->result() == QDialog::Accepted )
    fname = fd->getResult();
  else
    fname = "";

  delete fd;
  return( fname );
}

QString ScQtFileEdit::mkDir( QWidget *parent, const char *name,
                         const QString &path, const QString &filter )
{
  static ScQtFileEditDlg *fd;
  static QString fname;

#ifdef DEBUGFILEEDIT
  printf ("\nScQtFileEdit::mkDir name=%s", name );
  printf ("\n  basepath=%s", (const char *)path );
  printf ("\n  filter=%s", (const char *)filter );
  fflush(stdout);
#endif

  fd = new ScQtFileEditDlg( parent, name, path, filter );
  fd->setAutoExtension( true );
  fd->allowMkDir( true );
  fd->setNFilter( false );
  fd->setFSorting( true );
  fd->readIt();
  fd->exec();

  if ( fd->result() == QDialog::Accepted )
    fname = fd->getResult();
  else
    fname = "";

  delete fd;
  return( fname );
}

