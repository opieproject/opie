/***************************************************************************
                          installdlgimpl.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
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
#ifndef INSTALLDLGIMPL_H
#define INSTALLDLGIMPL_H

#include <vector>
using namespace std;

#include <qstring.h>

#include "ipkg.h"
#include "install.h"

class InstallDlgImpl : public InstallDlg
{
public:
    InstallDlgImpl( vector<QString> &packageList, DataManager *dataManager, QWidget * parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    InstallDlgImpl( QWidget * parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    ~InstallDlgImpl();

	bool showDlg();
	bool upgradeServer( QString &server );

protected:

private:
    DataManager *dataMgr;
	vector<QString> installList;
	vector<QString> removeList;
	vector<QString> updateList;
    int flags;
    Ipkg ipkg;
    bool upgradePackages;

	bool runIpkg( QString &option, const QString& package, const QString& dest, int flags );

	void optionsSelected();
	void installSelected();
    void displayText(const QString &text );
};

#endif
