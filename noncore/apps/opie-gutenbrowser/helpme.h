/***************************************************************************
                          helpme.h  -  description
                             -------------------
    begin                : Sat Dec 4 1999
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

#ifndef HELPME_H
#define HELPME_H

#include "gutenbrowser.h"
#include <qmultilineedit.h>
//#include "CConfigFile.h"
//#include <kprocess.h>
//#include <kprogress.h>
#include <qdialog.h>
#include <qprogressdialog.h>
#include <qwidget.h>


/**
  *@author llornkcor
  */

class HelpMe : public QDialog  {
   Q_OBJECT
public:
  HelpMe(QWidget *parent, QString name);
  ~HelpMe();

// KProgress *progressBar;
  QString url;
  QLabel  *label;
QLabel  *QTlabel;
//    CConfigFile *config;
    QString local_library;

private slots:
  void goToURL(); 
  void goToURL2();
  void goToURL3();
  void help();
  void goGetit( QString url);
 private:
  QMultiLineEdit* Edit;


};

#endif
