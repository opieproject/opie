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

#include "filterdlg.h"

FilterDlg::FilterDlg( QWidget *parent, OPackageManager *pm, const QString &name,
               const QString &server, const QString &destination,
               OPackageManager::Status status, const QString &category )
    : QDialog( parent, QString::null, true )
{
    setCaption( tr( "Filter packages" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    QScrollView *sv = new QScrollView( this );
    layout->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );
    layout = new QVBoxLayout( container, 4, 4 );

    // Category
    m_categoryCB = new QCheckBox( tr( "Category:" ), container );
    connect( m_categoryCB, SIGNAL(toggled(bool)), this, SLOT(slotCategorySelected(bool)) );
    m_category = new QComboBox( container );
    m_category->insertStringList( pm->categories() );
    initItem( m_category, m_categoryCB, category );
    layout->addWidget( m_categoryCB );
    layout->addWidget( m_category );

    // Package name
    m_nameCB = new QCheckBox( tr( "Names containing:" ), container );
    connect( m_nameCB, SIGNAL(toggled(bool)), this, SLOT(slotNameSelected(bool)) );
    m_name = new QLineEdit( name, container );
    if ( !name.isNull() )
        m_nameCB->setChecked( true );
    m_name->setEnabled( !name.isNull() );
    layout->addWidget( m_nameCB );
    layout->addWidget( m_name );

    // Status
    m_statusCB = new QCheckBox( tr( "With the status:" ), container );
    connect( m_statusCB, SIGNAL(toggled(bool)), this, SLOT(slotStatusSelected(bool)) );
    m_status = new QComboBox( container );
    connect( m_status, SIGNAL(activated(const QString &)), this, SLOT(slotStatusChanged(const QString &)) );
    QString currStatus;
    switch ( status )
    {
        case OPackageManager::All : currStatus = tr( "All" );
            break;
        case OPackageManager::Installed : currStatus = tr( "Installed" );
            break;
        case OPackageManager::NotInstalled : currStatus = tr( "Not installed" );
            break;
        case OPackageManager::Updated : currStatus = tr( "Updated" );
            break;
        default : currStatus = QString::null;
    };
    m_status->insertItem( tr( "All" ) );
    m_status->insertItem( tr( "Installed" ) );
    m_status->insertItem( tr( "Not installed" ) );
    m_status->insertItem( tr( "Updated" ) );
    initItem( m_status, m_statusCB, currStatus );
    layout->addWidget( m_statusCB );
    layout->addWidget( m_status );

    // Server
    m_serverCB = new QCheckBox( tr( "Available from the following server:" ), container );
    connect( m_serverCB, SIGNAL(toggled(bool)), this, SLOT(slotServerSelected(bool)) );
    m_server = new QComboBox( container );
    m_server->insertStringList( *(pm->servers()) );
    initItem( m_server, m_serverCB, server );
    layout->addWidget( m_serverCB );
    layout->addWidget( m_server );

    // Destination
    m_destCB = new QCheckBox( tr( "Installed on device at:" ), container );
    connect( m_destCB, SIGNAL(toggled(bool)), this, SLOT(slotDestSelected(bool)) );
    m_destination = new QComboBox( container );
    m_destination->insertStringList( *(pm->destinations()) );
    initItem( m_destination, m_destCB, destination );
    layout->addWidget( m_destCB );
    layout->addWidget( m_destination );

    //showMaximized();
}

void FilterDlg::initItem( QComboBox *comboBox, QCheckBox *checkBox, const QString &selection )
{
    if ( !selection.isNull() )
    {
        checkBox->setChecked( true );

        for ( int i = 0; i < comboBox->count(); i++ )
        {
            if ( comboBox->text( i ) == selection )
            {
                comboBox->setCurrentItem( i );
                return;
            }
        }
    }
    comboBox->setEnabled( !selection.isNull() );
}

void FilterDlg::slotNameSelected( bool selected )
{
    m_name->setEnabled( selected );
}

void FilterDlg::slotServerSelected( bool selected )
{
    m_server->setEnabled( selected );
}

void FilterDlg::slotDestSelected( bool selected )
{
    m_destination->setEnabled( selected );
}

void FilterDlg::slotStatusSelected( bool selected )
{
    m_status->setEnabled( selected );

    if ( !selected && !m_destCB->isEnabled() )
    {
        // If status check box has been deselected and destination option was previously deselected
        // (because status == "Not installed"), re-enable destination option
        m_destCB->setEnabled( true );
        m_destination->setEnabled( true );
    }
    else if ( selected && m_destCB->isEnabled() && m_status->currentText() == tr( "Not installed" ) )
    {
        // If status check box has been selected and status == "Not installed", disable destination option
        m_destCB->setEnabled( false );
        m_destCB->setChecked( false );
        m_destination->setEnabled( false );
    }
}

void FilterDlg::slotStatusChanged( const QString &category )
{
    bool notInstalled = ( category == tr( "Not installed" ) );
    m_destCB->setEnabled( !notInstalled );
    m_destination->setEnabled( !notInstalled );
    if ( notInstalled )
        m_destCB->setChecked( false );
}

void FilterDlg::slotCategorySelected( bool selected )
{
    m_category->setEnabled( selected );
}
