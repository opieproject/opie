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

#include "opentextdlg.h"

#include <qpe/resource.h>

#include <qheader.h>
#include <qlayout.h>
#include <qpixmap.h>

OpenTextDlg::OpenTextDlg( QWidget *parent, sword::SWMgr *swordMgr, QPixmap *bibleIcon,
                          QPixmap *commentaryIcon, QPixmap *lexiconIcon )
    : QDialog( parent, QString::null, true )
    , m_textList( this )
{
    setCaption( tr( "Open text" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 4 );
    layout->addWidget( &m_textList );

    m_textList.setRootIsDecorated( true );
    m_textList.addColumn( tr( "Icon" ),35 );
    m_textList.addColumn( tr( "Text" ) );
    m_textList.header()->hide();
    m_textList.setAllColumnsShowFocus( true );
    m_textList.setSorting( 1 );

    m_commentaries = new QListViewItem( &m_textList, QString::null, tr( "Commentaries" ) );
    m_commentaries->setPixmap( 0, *commentaryIcon );
    m_textList.insertItem( m_commentaries );
    m_lexicons = new QListViewItem( &m_textList, QString::null, tr( "Lexicons/Dictionaries" ) );
    m_lexicons->setPixmap( 0, *lexiconIcon );
    m_textList.insertItem( m_lexicons );
    m_bibles = new QListViewItem( &m_textList, QString::null, tr( "Biblical Texts" ) );
    m_bibles->setPixmap( 0, *bibleIcon );
    m_textList.insertItem( m_bibles );
    connect( &m_textList, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotItemClicked(QListViewItem*)) );

    if ( swordMgr )
    {
        sword::ModMap::iterator it;
        QString type;
        QPixmap *icon = 0x0;
        QListViewItem *parent = 0x0;

        for ( it = swordMgr->Modules.begin(); it != swordMgr->Modules.end(); it++ )
        {
            type = it->second->Type();
            if ( type == "Biblical Texts" )
            {
                icon = bibleIcon;
                parent = m_bibles;
            }
            else if ( type == "Commentaries" )
            {
                icon = commentaryIcon;
                parent = m_commentaries;
            }
            else if ( type == "Lexicons / Dictionaries" )
            {
                icon = lexiconIcon;
                parent = m_lexicons;
            }

            parent->insertItem( new QListViewItem( parent, QString::null, it->first.c_str() ) );
        }
    }

    m_textList.sort();
}

void OpenTextDlg::slotItemClicked( QListViewItem *item )
{
    if ( item == m_bibles || item == m_lexicons || item == m_commentaries )
    {
        m_textList.clearSelection();
        if ( item->childCount() > 0 )
        {
            item->setOpen( !item->isOpen() );
        }
    }
}
