/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "configuredlg.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvbuttongroup.h>
#include <qwhatsthis.h>

ConfigureDlg::ConfigureDlg( QWidget *parent, const QString &swordPath, bool alwaysOpenNew, int numVerses,
                            bool disableBlanking, int copyFormat, const QFont *font )
    : QDialog( parent, QString::null, true, WStyle_ContextHelp )
    , m_tabs( this )
{
    setCaption( tr( "Configure Dagger" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 4 );
    layout->addWidget( &m_tabs );

    // General tab
    QWidget *widget = new QWidget( this );
    QGridLayout *grid = new QGridLayout( widget, 1, 2, 4, 2 );
    grid->setRowStretch( 9, 5 );
    grid->setColStretch( 0, 2 );

    QLabel *label = new QLabel( tr( "Path where Sword texts are located:" ), widget );
    label->setAlignment( Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak );
    QWhatsThis::add( label, tr( "Enter the path where the Sword modules (Bible texts, commentaries, etc.) can be found.  This path should contain either the 'mods.conf' file or 'mods.d' sub-directory." ) );
    grid->addMultiCellWidget( label, 0, 0, 0, 1 );
    m_swordPath = new QLineEdit( swordPath, widget );
    QWhatsThis::add( m_swordPath, tr( "Enter the path where the Sword texts (Bibles, commentaries, etc.) can be found.  This path should contain either the 'mods.conf' file or 'mods.d' sub-directory." ) );
    grid->addMultiCellWidget( m_swordPath, 1, 1, 0, 1 );
    label = new QLabel( tr( "(Note: Dagger must be restarted for this option to take affect.)" ), widget );
    label->setAlignment( Qt::AlignHCenter | Qt::AlignTop | Qt::WordBreak );
    QWhatsThis::add( label, tr( "Enter the path where the Sword modules (Bible texts, commentaries, etc.) can be found.  This path should contain either the 'mods.conf' file or 'mods.d' sub-directory." ) );
    grid->addMultiCellWidget( label, 2, 2, 0, 1 );

    grid->addRowSpacing( 3, 15 );

    m_alwaysOpenNew = new QCheckBox( tr( "Always open texts in new window?" ), widget );
    m_alwaysOpenNew->setChecked( alwaysOpenNew );
    QWhatsThis::add( m_alwaysOpenNew, tr( "Tap here to always open texts in a new window.  If this option is not selected, only one copy of a Sword text will be opened." ) );
    grid->addMultiCellWidget( m_alwaysOpenNew, 4, 4, 0, 1 );

    grid->addRowSpacing( 5, 15 );

    label = new QLabel( tr( "Number of verses to display at a time:" ), widget );
    label->setAlignment( Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak );
    QWhatsThis::add( label, tr( "Enter the number of verses to display at a time.  This also affects how far the scroll to previous/next page buttons on the Navigation bar scroll." ) );
    grid->addWidget( label, 6, 0 );
    m_numVerses = new QSpinBox( 1, 20, 1, widget );
    m_numVerses->setValue( numVerses );
    QWhatsThis::add( m_numVerses, tr( "Enter the number of verses to display at a time.  This also affects how far the scroll to previous/next page buttons on the Navigation bar scroll." ) );
    grid->addWidget( m_numVerses, 6, 1 );

    grid->addRowSpacing( 7, 15 );

    m_disableScreenBlank = new QCheckBox( tr( "Disable automatic screen power-down?" ), widget );
    m_disableScreenBlank->setChecked( disableBlanking );
    QWhatsThis::add( m_disableScreenBlank, tr( "Tap here to disable Opie's automatic power management feature which will dim and turn off the screen after a specified time.  This will only be effective while Dagger is running." ) );
    grid->addMultiCellWidget( m_disableScreenBlank, 8, 8, 0, 1 );

    m_tabs.addTab( widget, "SettingsIcon", tr( "General" ) );

    // Copy tab
    widget = new QWidget( this );
    QWhatsThis::add( widget, tr( "Select the format used when copying the current verse to the clipboard." ) );
    layout = new QVBoxLayout( widget );
    layout->setMargin( 4 );

    QVButtonGroup *bg = new QVButtonGroup( tr( "Select copy format" ), widget );
    m_copyTextFull = new QRadioButton( tr( "\"Verse (Book cc:vv, text)\"" ), bg );
    connect( m_copyTextFull, SIGNAL(clicked()), this, SLOT(slotCopyFormatSelected()) );
    m_copyFull = new QRadioButton( tr( "\"Verse (Book cc:vv)\"" ), bg );
    connect( m_copyFull, SIGNAL(clicked()), this, SLOT(slotCopyFormatSelected()) );
    m_copyVerse = new QRadioButton( tr( "\"Verse\"" ), bg );
    connect( m_copyVerse, SIGNAL(clicked()), this, SLOT(slotCopyFormatSelected()) );
    m_copyKey = new QRadioButton( tr( "\"Book cc:vv\"" ), bg );
    connect( m_copyKey, SIGNAL(clicked()), this, SLOT(slotCopyFormatSelected()) );
    layout->addWidget( bg );

    layout->addSpacing( 15 );

    label = new QLabel( tr( "Example:" ), widget );
    label->setAlignment( Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak );
    layout->addWidget( label );

    layout->addSpacing( 15 );

    m_copyExample = new QLabel( widget );
    m_copyExample->setAlignment( Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak );
    layout->addWidget( m_copyExample );

    if ( copyFormat == 0 )
        m_copyTextFull->animateClick();
    else if ( copyFormat == 1 )
        m_copyFull->animateClick();
    else if ( copyFormat == 2 )
        m_copyVerse->animateClick();
    else if ( copyFormat == 3 )
        m_copyKey->animateClick();

    layout->addStretch();

    m_tabs.addTab( widget, "copy", tr( "Copy" ) );

    // Font tab
    m_font = new Opie::Ui::OFontSelector( true, this );
    if ( font )
        m_font->setSelectedFont( *font );
    QWhatsThis::add( m_font, tr( "Select the font, style and size used for displaying texts." ) );

    m_tabs.addTab( m_font, "font", tr( "Font" ) );

    m_tabs.setCurrentTab( tr( "General" ) );
}

void ConfigureDlg::slotCopyFormatSelected()
{
    const QObject *option = sender();

    QString text = tr( "KJV" );
    QString verse = tr( "In the beginning God created the heaven and the earth." );
    QString key = tr( "Gen 1:1" );

    if ( option == m_copyTextFull && m_copyTextFull->isChecked() )
        m_copyExample->setText( QString( "%1 (%2, %3)" ).arg( verse ).arg( key ).arg( text ) );
    else if ( option == m_copyFull && m_copyFull->isChecked() )
        m_copyExample->setText( QString( "%1 (%2)" ).arg( verse ).arg( key ) );
    else if ( option == m_copyVerse && m_copyVerse->isChecked() )
        m_copyExample->setText( verse );
    else if ( option == m_copyKey && m_copyKey->isChecked() )
        m_copyExample->setText( key );
}
