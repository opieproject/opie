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

#ifndef INSTALLDLG_H
#define INSTALLDLG_H

#include <qwidget.h>

#include "opackage.h"

class QComboBox;
class QLabel;
class QMultiLineEdit;
class QPushButton;
class QProgressBar;

class OConfItem;
class OPackageManager;

class InstallDlg : public QWidget
{
    Q_OBJECT

public:
    InstallDlg( QWidget *parent = 0l, OPackageManager *pm = 0l,
                const QString &caption = QString::null,
                OPackage::Command command1 = OPackage::NotDefined,
                const QStringList &packages1 = QStringList(),
                OPackage::Command command2 = OPackage::NotDefined,
                const QStringList &packages2 = QStringList(),
                OPackage::Command command3 = OPackage::NotDefined,
                const QStringList &packages3 = QStringList() );

private:
    OPackageManager *m_packman;     // Pointer to application instance of package manager

    bool m_installFound; // Indicates if an install is being done, controls display of
                         // destination selection, available space

    // UI controls
    QComboBox       *m_destination; // Destination selection list
    QLabel          *m_availSpace;  // Text label to display available space on selected destination
    QMultiLineEdit  *m_output;      // Multi-line edit to display status
    QPushButton     *m_btnStart;    // Start/abort/close button
    QPushButton     *m_btnOptions;  // Installation options button
    QProgressBar    *m_progressBar;

    // Commands and packages to execute
    int               m_numCommands; // Number of commands to be executed
    int               m_currCommand; // Number of currently executing command
    OPackage::Command m_command[3];  // List of commands to be executed
    QStringList       m_packages[3]; // Lists of package names associated to commands (m_command[])
    OConfItem        *m_destItem;    // Pointer to destination for package installation

private slots:
    // UI control slots
    void slotDisplayAvailSpace( const QString &destination );
    void slotBtnStart();
    void slotBtnOptions();

    // Execution slots
    void slotOutput( const QString &msg );
    void slotProgress( const QString &msg, int percentage );

signals:
    void closeInstallDlg();
};

#endif
