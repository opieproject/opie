/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

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
