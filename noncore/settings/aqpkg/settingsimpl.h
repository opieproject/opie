/***************************************************************************
                          settingsimpl.h  -  description
                             -------------------
    begin                : Thu Aug 29 2002
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

#include "datamgr.h"

#include <qdialog.h>

class QCheckBox;
class QLineEdit;
class QListBox;

class SettingsImpl : public QDialog
{
    Q_OBJECT
public:
     SettingsImpl( DataManager *dataManager, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SettingsImpl();

	bool showDlg();

private:
    QListBox  *servers;
    QLineEdit *servername;
    QLineEdit *serverurl;
    QCheckBox *active;
    QListBox  *destinations;
    QLineEdit *destinationname;
    QLineEdit *destinationurl;
    QCheckBox *linkToRoot;
    QLineEdit *txtHttpProxy;
    QCheckBox *chkHttpProxyEnabled;
    QLineEdit *txtFtpProxy;
    QCheckBox *chkFtpProxyEnabled;
    QLineEdit *txtUsername;
    QLineEdit *txtPassword;

    DataManager *dataMgr;
    QString serverName;
    QString destinationName;
    int currentSelectedServer;
    int currentSelectedDestination;
    bool changed;
    bool newserver;
    bool newdestination;

    QWidget *initServerTab();
    QWidget *initDestinationTab();
    QWidget *initProxyTab();
    
    void setupConnections();
    void setupData();

public slots:
    void editServer( int s );
    void changeServerDetails();
    void newServer();
    void removeServer();
    void editDestination( int s );
    void changeDestinationDetails();
    void newDestination();
    void removeDestination();
    void proxyApplyChanges();
};
