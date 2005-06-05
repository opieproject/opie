/*
                             This file is part of the Opie Project

                             Copyright (C)2004, 2005 Dan Williams <drw@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
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

#ifndef OIPKGCONFIGDLG_H
#define OIPKGCONFIGDLG_H

#include <opie2/otabwidget.h>

#include <qdialog.h>
#include <qlayout.h>

#include "oipkg.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QListBox;
class QPushButton;

class OIpkgConfigDlg : public QDialog
{
    Q_OBJECT

public:
    OIpkgConfigDlg( OIpkg *ipkg = 0l, bool installOptions = false, QWidget *parent = 0l );

protected slots:
    void accept();
    void reject();

private:
    OIpkg         *m_ipkg;    // Pointer to Ipkg class for retrieving/saving configuration options
    OConfItemList *m_configs; // Local list of configuration items

    bool           m_installOptions; // If true, will only display the Options tab

    // Server/Destination cached information
    int m_serverCurrent; // Index of currently selected server in m_serverList
    int m_destCurrent;   // Index of currently selected destination in m_destList

    // UI controls
    QVBoxLayout           m_layout;        // Main dialog layout control
    Opie::Ui::OTabWidget  m_tabWidget;     // Main tab widget control
    QWidget              *m_serverWidget;  // Widget containing server configuration controls
    QWidget              *m_destWidget;    // Widget containing destination configuration controls
    QWidget              *m_proxyWidget;   // Widget containing proxy configuration controls
    QWidget              *m_optionsWidget; // Widget containing ipkg execution configuration controls

    // Server configuration UI controls
    QListBox    *m_serverList;      // Server list selection
    QPushButton *m_serverEditBtn;   // Server edit button
    QPushButton *m_serverDeleteBtn; // Server edit button

    // Destination configuration UI controls
    QListBox    *m_destList;      // Destination list selection
    QPushButton *m_destEditBtn;   // Destination edit button
    QPushButton *m_destDeleteBtn; // Destination edit button

    // Proxy server configuration UI controls
    QLineEdit *m_proxyHttpServer;  // HTTP proxy server URL edit box
    QCheckBox *m_proxyHttpActive;  // Activate HTTP proxy check box
    QLineEdit *m_proxyFtpServer;   // FTP proxy server edit box
    QCheckBox *m_proxyFtpActive;   // Activate FTP proxy check box
    QLineEdit *m_proxyUsername;    // Proxy server username edit box
    QLineEdit *m_proxyPassword;    // Proxy server password edit box

    // Options configuration UI controls
    QCheckBox *m_optForceDepends;   // Force depends ipkg option checkbox
    QCheckBox *m_optForceReinstall; // Force reinstall ipkg option checkbox
    QCheckBox *m_optForceRemove;    // Force remove ipkg option checkbox
    QCheckBox *m_optForceOverwrite; // Force overwrite ipkg option checkbox
    QComboBox *m_optVerboseIpkg;    // Ipkg verbosity option selection
    QLineEdit *m_optSourceLists;    // Ipkg source lists destination directory

    void initServerWidget();
    void initDestinationWidget();
    void initProxyWidget();
    void initOptionsWidget();

    void initData();

private slots:
    void slotServerSelected( int index );
    void slotServerNew();
    void slotServerEdit();
    void slotServerDelete();

    void slotDestSelected( int index );
    void slotDestNew();
    void slotDestEdit();
    void slotDestDelete();

    void slotOptSelectSourceListsPath();
};

class OIpkgServerDlg : public QDialog
{
    Q_OBJECT

public:
    OIpkgServerDlg( OConfItem *server = 0l, QWidget *parent = 0l );

protected slots:
    void accept();

private:
    OConfItem *m_server;

    // UI controls
    QLineEdit *m_name;       // Server name edit box
    QLineEdit *m_location;   // Server location URL edit box
    QCheckBox *m_compressed; // Indicates whether the server is a 'src/gz' feed
    QCheckBox *m_active;     // Indicates whether the server is activated
};

class OIpkgDestDlg : public QDialog
{
    Q_OBJECT

public:
    OIpkgDestDlg( OConfItem *dest = 0l, QWidget *parent = 0l );

protected slots:
    void accept();

private:
    OConfItem *m_dest;

    // UI controls
    QLineEdit *m_name;       // Destination name edit box
    QLineEdit *m_location;   // Destination location URL edit box
    QCheckBox *m_active;     // Indicates whether the destination is activated

private slots:
    void slotSelectPath();
};

#endif
