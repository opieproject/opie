/***************************************************************************
                          scqtfileedit.h  -  description
                             -------------------
    begin                : Mon Sep 3 2001
    copyright            : (C) 2001 by Werner Schulte
    email                : sc@schulte-ac.de
 ***************************************************************************/

/* $Id: scqtfileedit.h,v 1.1 2003-07-22 19:23:19 zcarsten Exp $ */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCQTFILEEDIT_H
#define SCQTFILEEDIT_H

#include <qdir.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qlistview.h>
#include <qcombobox.h>
#include <qtimer.h>

#include <scqtfiledlg.h>

#define MAXDIRCOMBOCOUNT  8
#define MAXTYPECOMBOCOUNT 8

/**
  *@author Werner Schulte
  */

class ScQtFileEditDlg : public ScQtFileDlg, QDir
{
  Q_OBJECT
public:
  ScQtFileEditDlg( QWidget *parent=0, const char *name=0,
               const QString &path="/", const QString &filter="*" );
  ~ScQtFileEditDlg();
  QString getResult( );
  QString getFileName( );
  void readIt( );
  void setAutoExtension( bool ex=false );
  void allowMkDir( bool mkdir=false );
  void setNFilter( bool ff=false );
  void setFSorting( bool ff=false );
  void setMode( int mode=QDir::All );

private slots:
  void slotOK( );
  void slotCancel( );
  void slotDoubleClicked( QListViewItem *item=0 );
  void slotSelectionChanged( QListViewItem *item=0 );
  void slotDirComboBoxChanged( int item );
  void slotTypeComboBoxChanged( int item );
  void slotFileTextChanged( const QString &txt="" );
  void slotShowDir();
  void slotMkDir();

protected:
  QTimer showTimer;
  bool   mkdirflag, fsorting, fnfilter;
  int    fmode;
  const QFileInfoList *qfil;
  bool rFlag, exflag; // recursionFlag,extensionflag
  QString bpath, bfilter, filename;
  QString dirstr,linkstr, filestr;
  QPixmap *fileIcon, *dirIcon, *linkIcon, *cdToParentIcon;
  const QFileInfoList *readDir( const QString &path="/", const QString &filter="*" );
  int checkComboEntryExists( QComboBox *com=0, const QString &str="" );
  void insDirCombo( const QString &str="" );
  void insTypeCombo( const QString &str="" );
  void initDirCombo( const QString &str="" );
  void initTypeCombo( const QString &str="" );
  void autoExtension( );

};

class ScQtFileEdit : public QObject
{
  Q_OBJECT
public:
  ScQtFileEdit( QWidget *parent=0, const char *name=0 );
  ~ScQtFileEdit();
  static void resize( int width, int height );
  static QString getOpenFileName( QWidget *parent=0, const char *name=0,
                                  const QString &path="/", const QString &filter="*" );
  static QString getSaveAsFileName( QWidget *parent=0, const char *name=0,
                                    const QString &path="/", const QString &filter="*" );
  static QString getDirName( QWidget *parent=0, const char *name=0,
                             const QString &path="/" );
  static QString mkDir( QWidget *parent=0, const char *name=0,
                        const QString &path="/", const QString &filter="*" );

protected:

};

#endif
