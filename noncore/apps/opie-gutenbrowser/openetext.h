/***************************************************************************
                          openetext.h  -  description
                             -------------------
    begin                : Tue Jul 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OPENETEXT_H
#define OPENETEXT_H
#include "gutenbrowser.h"
#include <qdialog.h>
#include <qstringlist.h> 
#ifndef Q_WS_QWS
#include <qfiledialog.h>
#endif
//#include <fileselector.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qstrlist.h>
#include <qwidget.h>
//#include "CConfigFile.h"

/**
  *@author llornkcor
  */

class OpenEtext : public QDialog  {
   Q_OBJECT
public: 
  OpenEtext(QWidget *parent, QString name);
  ~OpenEtext();

    QPushButton* RemoveButton, * OpenFileButton, * OpenButton, *scanButton, *editButton; 
  void getTitles();
    QString title;
  QString file;
  QString selFile;
  QString s_numofFiles;
  QString fileName;
  QString name;
  QString openFileTitle;
  QStringList fileList;
  QString  local_library;
  QString  local_index;
  QString title_text;
  void remFile();
    void removeSelection();
    QString titleFromLibrary( const QString fileName);
    bool FindTitle( const QString filename);
    bool checkConf();
//    CConfigFile *config;

protected: 

    void initDialog();
  QListBox *QListBox_1;
  QPushButton *QPushButton_OK;
  QPushButton *QPushButton_Cancel;
  QLabel *QLabel_1;

private:
private slots:

  virtual void select_title(int index );
  virtual void remove();
  virtual void scan();
  virtual void open();
  virtual void OpenTitle();
  virtual void editTitle();
};

#endif
