/*
                            This file is part of the Opie Project

                             Copyright (c)  2003 Dan Williams <drw@handhelds.org>
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

#include <qdialog.h>
#include <qlayout.h>

#include <opie2/otabwidget.h>

#include "oipkg.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QListBox;

class OIpkgConfigDlg : public QDialog
{
    Q_OBJECT

public:
    OIpkgConfigDlg( OIpkg *ipkg = 0x0, bool installOptions = false, QWidget *parent = 0x0 );

protected slots:
    void accept();
    void reject();

private:
    OIpkg         *m_ipkg;    // Pointer to Ipkg class for retrieving/saving configuration options
    OConfItemList *m_configs; // Local list of configuration items

    bool           m_installOptions; // If true, will only display the Options tab

    // Server cached information
    bool           m_serverNew;      // Indicates if server information is for a new server
    int            m_serverCurrent;  // Index of currently selected server in m_serverList
    QString        m_serverCurrName; // Name of currently selected server in m_serverList

    // Destination cached information
    bool           m_destNew;      // Indicates if destination information is for a new destination
    int            m_destCurrent;  // Index of currently selected destination in m_destList
    QString        m_destCurrName; // Name of currently selected destination in m_destList

    // UI controls
    QVBoxLayout  m_layout;        // Main dialog layout control
    Opie::Ui::OTabWidget   m_tabWidget;     // Main tab widget control
    QWidget     *m_serverWidget;  // Widget containing server configuration controls
    QWidget     *m_destWidget;    // Widget containing destination configuration controls
    QWidget     *m_proxyWidget;   // Widget containing proxy configuration controls
    QWidget     *m_optionsWidget; // Widget containing ipkg execution configuration controls

    // Server configuration UI controls
    QListBox   *m_serverList;     // Server list selection
    QLineEdit  *m_serverName;     // Server name edit box
    QLineEdit  *m_serverLocation; // Server location URL edit box
    QCheckBox  *m_serverActive;   // Activate server check box

    // Destination configuration UI controls
    QListBox   *m_destList;     // Destination list selection
    QLineEdit  *m_destName;     // Destination name edit box
    QLineEdit  *m_destLocation; // Destination location URL edit box
    QCheckBox  *m_destActive;   // Activate destination check box

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

    void initServerWidget();
    void initDestinationWidget();
    void initProxyWidget();
    void initOptionsWidget();

    void initData();

    OConfItem *findConfItem( OConfItem::Type type = OConfItem::NotDefined, const QString &name = 0x0 );

private slots:
    void slotServerEdit( int index );
    void slotServerNew();
    void slotServerDelete();
    void slotServerUpdate();

    void slotDestEdit( int index );
    void slotDestNew();
    void slotDestDelete();
    void slotDestUpdate();
};

#endif
