/***************************************************************************
                          networkpkgmgr.h  -  description
                             -------------------
    begin                : Mon Aug 26 13:32:30 BST 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NETWORKPKGMGR_H
#define NETWORKPKGMGR_H

#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qcombobox.h>
#include <qlistview.h>

#include "datamgr.h"
#include "progressdlg.h"

/** NetworkPackageManager is the base class of the project */
class NetworkPackageManager : public QWidget
{
  Q_OBJECT
public:
    /** construtor */
    NetworkPackageManager( DataManager *dataManager, QWidget* parent=0, const char *name=0);
    /** destructor */
    ~NetworkPackageManager();

    void updateData();

private:
    DataManager *dataMgr;

    QComboBox *serversList;
    QListView *packagesList;
    QPushButton *update;
    QPushButton *upgrade;
    QPushButton *download;
    QPushButton *apply;

    ProgressDlg *progressDlg;

    int timerId;

    void timerEvent ( QTimerEvent * );

    void initGui();
    void setupConnections();
    void showProgressDialog( char *initialText );
    QString dealWithItem( QCheckListItem *item );

public slots:
    void serverSelected( int index );
    void applyChanges();
    void upgradePackages();
    void downloadPackage();
    void updateServer();
    void displayText( const QString &t );
};

#endif
