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

#include "settings.h"

#include "datamgr.h"

class SettingsImpl : public SettingsBase
{
public:
     SettingsImpl( DataManager *dataManager, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SettingsImpl();

	bool showDlg( int i );

private:
    DataManager *dataMgr;
    QString serverName;
    QString destinationName;
    int currentSelectedServer;
    int currentSelectedDestination;
    bool changed;
    bool newserver;
    bool newdestination;

    void setupConnections();
    void setupData();

    void editServer( int s );
    void changeServerDetails();
	void newServer();
	void removeServer();

    void editDestination( int s );
    void changeDestinationDetails();
	void newDestination();
	void removeDestination();

    void toggleJumpTo( bool val );

    void proxyApplyChanges();
};
