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

#include "textwidget.h"

#include <qlayout.h>
#include <qtextbrowser.h>

#include <versekey.h>

TextWidget::TextWidget( QWidget *parent, sword::SWModule *module, int numVerses, const QFont *font )
    : QWidget( parent, 0x0, 0x0 )
    , m_module( module )
    , m_numVerses( numVerses )
{
    if ( parent )
        installEventFilter( parent );

    QVBoxLayout *layout = new QVBoxLayout( this, 2, 2 );

    m_textView = new QTextBrowser( this );
    m_textView->installEventFilter( parent );
    m_textView->setMinimumHeight( 20 );
    m_textView->setHScrollBarMode( QTextView::AlwaysOff );
    m_textView->setTextFormat( QTextView::RichText );
    connect( m_textView, SIGNAL(highlighted(const QString &)),
             this, SIGNAL(sigRefClicked(const QString &)) );
    layout->addWidget( m_textView );

    // Set font
    if ( font )
        setFont( *font );

    // Set initial text
    if ( m_module )
    {
        m_isBibleText = !strcmp( module->Type(), "Biblical Texts" );
        if ( m_isBibleText )
        {
            m_key = new sword::VerseKey( "g" );

            //connect( m_textView, SIGNAL(highlighted(const QString&)),
            //         this, SLOT(slotReferenceClicked(const QString&)) );
            //connect( parent, SIGNAL( strongsNumbers( bool ) ), this, SLOT( slotStrongsNumbers( bool ) ) );
        }
        else
        {
            m_key = new sword::SWKey( "" );
        }
        m_module->SetKey( m_key );
        setText();
    }
}

TextWidget::~TextWidget()
{
    // TODO - why does this cause a SIGSEV???
    //delete m_key;
}

QString TextWidget::getCurrVerse()
{
    m_module->SetKey( m_key->getText() );
    return ( QString ) m_module->StripText();
}

void TextWidget::prevPage()
{
    (*m_key) -= m_numVerses;
    setText();
}

void TextWidget::prevVerse()
{
    (*m_key)--;
    setText();
}

void TextWidget::setKey( const QString &newKey )
{
    m_key->setText( newKey.latin1() );
    setText();
}

void TextWidget::nextVerse()
{
    (*m_key)++;
    setText();
}

void TextWidget::nextPage()
{
    (*m_key) += m_numVerses;
    setText();
}

void TextWidget::slotNumVersesChanged( int numVerses )
{
    m_numVerses = numVerses;
    setText();
}

void TextWidget::slotFontChanged( const QFont *newFont )
{
    setFont( *newFont );
    // TODO - shouldn't have to reset text, but couldn't get repaint() to work
    setText();
}

void TextWidget::slotOptionChanged()
{
    setText();
}

void TextWidget::setText()
{
    if ( m_key->Error() )
        return;

    m_module->SetKey( m_key->getText() );
    m_fullKey = QString( "%1 (%2)" ).arg( m_key->getShortText() ).arg( m_module->Name() );

    if ( m_isBibleText )
    {
        m_textView->setVScrollBarMode( QTextView::AlwaysOff );

        m_abbrevKey = m_key->getShortText();

        QString fullText;

        for ( int i = 0; i < m_numVerses; i++ )
        {
            QString key = ( QString ) m_module->KeyText();
            QString verseStr = ( QString ) *m_module;

            // Format current verse (adding chapter and/or book headings if necessary)
            int verse = static_cast<sword::VerseKey>(m_module->Key()).Verse();
            if ( verse == 1 )
            {
                int chapter = static_cast<sword::VerseKey>(m_module->Key()).Chapter();
                if ( chapter == 1 )
                {
                    QString book = static_cast<sword::VerseKey>(m_module->Key()).getBookName();
                    verseStr = QString( "<p><center><big><b>%1</b></big></center><br><center><b>Chapter %1</b></center><p><b>%2</b>&nbsp;%3<p>" )
                                       .arg( book ).arg( chapter ).arg( verse ).arg( verseStr );
                }
                else
                {
                    verseStr = QString( "<center><b>Chapter %1</b></center><p><b>%2</b>&nbsp;%3<p>" )
                                       .arg( chapter ).arg( verse ).arg( verseStr );
                }
            }
            else
            {
                verseStr = QString( "<b>%1</b>&nbsp;%2<p>" ).arg( verse ).arg( verseStr );
            }

            fullText.append( verseStr );

            m_module->Key()++;

            if ( m_module->Key().Error() )
                break;
        }

        m_textView->setText( fullText );
    }
    else // !isBibleText
    {
        m_abbrevKey = m_key->getText();
        m_textView->setText( ( QString ) *m_module );
    }
}
