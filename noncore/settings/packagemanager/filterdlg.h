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

#ifndef FILTERDLG_H
#define FILTERDLG_H

#include "opackagemanager.h"

#include <qcheckbox.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qscrollview.h>

class FilterDlg : public QDialog
{
    Q_OBJECT

public:
    FilterDlg( QWidget *parent = 0x0, OPackageManager *pm = 0x0, const QString &name = 0x0,
               const QString &server = 0x0, const QString &destination = 0x0,
               OPackageManager::Status status = OPackageManager::All,
               const QString &category = 0x0 );

    QString name()
        { return m_nameCB->isChecked() ? m_name->text() : QString::null; };
    QString server()
        { return m_serverCB->isChecked() ? m_server->currentText() : QString::null; };
    QString destination()
        { return m_destCB->isChecked() ? m_destination->currentText() : QString::null; };
    OPackageManager::Status status()
        {
            if ( m_statusCB->isChecked() )
            {
                if ( m_status->currentText() == tr( "All" ) )
                    return OPackageManager::All;
                else if ( m_status->currentText() == tr( "Installed" ) )
                    return OPackageManager::Installed;
                else if ( m_status->currentText() == tr( "Not installed" ) )
                    return OPackageManager::NotInstalled;
                else if ( m_status->currentText() == tr( "Updated" ) )
                    return OPackageManager::Updated;
            }

            return OPackageManager::NotDefined;
        };
    QString category()
        { return m_categoryCB->isChecked() ? m_category->currentText() : QString::null; };

private:
    QCheckBox *m_nameCB;
    QCheckBox *m_serverCB;
    QCheckBox *m_destCB;
    QCheckBox *m_statusCB;
    QCheckBox *m_categoryCB;

    QLineEdit *m_name;
    QComboBox *m_server;
    QComboBox *m_destination;
    QComboBox *m_status;
    QComboBox *m_category;

    void initItem( QComboBox *comboBox, QCheckBox *checkBox, const QString &selection );

private slots:
    void slotNameSelected( bool );
    void slotServerSelected( bool );
    void slotDestSelected( bool );
    void slotStatusSelected( bool );
    void slotStatusChanged( const QString & );
    void slotCategorySelected( bool );
};

#endif
