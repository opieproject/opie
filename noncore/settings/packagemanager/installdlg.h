/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2003 Dan Williams <drw@handhelds.org>
             .=l.
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

#ifndef INSTALLDLG_H
#define INSTALLDLG_H

#include <qwidget.h>

#include "opackage.h"

class QComboBox;
class QLabel;
class QMultiLineEdit;
class QPushButton;

class OPackageManager;

class InstallDlg : public QWidget
{
    Q_OBJECT

public:
    InstallDlg( QWidget *parent = 0x0, OPackageManager *pm = 0x0, const QString &caption = QString::null,
                bool showDestInfo = true,
                OPackage::Command command1 = OPackage::NotDefined, QStringList *packages1 = 0x0,
                OPackage::Command command2 = OPackage::NotDefined, QStringList *packages2 = 0x0,
                OPackage::Command command3 = OPackage::NotDefined, QStringList *packages3 = 0x0 );
    ~InstallDlg();

private:
    OPackageManager *m_packman;     // Pointer to application instance of package manager

    // UI controls
    QComboBox       *m_destination; // Destination selection list
    QLabel          *m_availSpace;  // Text label to display available space on selected destination
    QMultiLineEdit  *m_output;      // Multi-line edit to display status
    QPushButton     *m_btnStart;    // Start/abort/close button
    QPushButton     *m_btnOptions;  // Installation options button

    // Commands and packages to execute
    int                m_numCommands; // Number of commands to be executed
    int                m_currCommand; // Number of currently executing command
    OPackage::Command  m_command[3];  // List of commands to be executed
    QStringList       *m_packages[3]; // Lists of package names associated to commands (m_command[])

private slots:
    // UI control slots
    void slotDisplayAvailSpace( const QString &destination );
    void slotBtnStart();
    void slotBtnOptions();

    // Execution slots
    void slotOutput( char *msg );

signals:
    void closeInstallDlg();
};

#endif
