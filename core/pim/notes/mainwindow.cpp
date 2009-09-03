/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
            .=l.             Portions copyright (C) 2002 Henning Holtschneider <hh@holtschneider.com>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

/*
 *  OPIE Notes
 *
 *  based on NoteZ 1.1.0 by Henning Holtschneider <hh@holtschneider.com>
 *
 *  moved to OPIE Pim framework by Marcin Juszkiewicz <openembedded@hrw.one.pl>
 *
 *  History:
 *  - 2005.10.12 - started work
 *  - 2005.10.19 - version 0.2:
 *                 - first working version
 *                 - info sent to NoteZ author and to OPIE devel
 *
 *  - 2005.10.20 - version 0.3:
 *                 - load/save in UTF-8
 *                 - load all files from Documents/text/plain
 *                 - create .txt files not .ntz - timestamp used as name
 *                 - one variable (documentsDirName) keep location of notes
 *                 - code (re)indented and converted to spaces
 *                 - variables translated to English (were in German)
 *                 - started work on beaming
 *
 *  - 2005.10.26 - version 0.4:
 *                 - added check does Documents/text/plain exist and create it if not
 *
 *  ToDo:
 *  - beaming
 *  - moving to SQLite database
 *  - category support
 *  - searching
 *
 */

#include "mainwindow.h"
#include "editwindow.h"

#include <opie2/omemoaccess.h>
#include <opie2/omemoaccessbackend_text.h>

#include <qpe/ir.h>

using namespace Opie;
using namespace Opie::Notes;


MemoListItem::MemoListItem ( QListBox *listbox, const QString &text, int uid )
    : QListBoxText( listbox, text ), m_uid( uid )
{
}

int MemoListItem::uid()
{
    return m_uid;
}


mainWindowWidget::mainWindowWidget( QWidget *parent, const char *name, WFlags)
    : Opie::OPimMainWindow( "Notes", "Notes", tr( "Note" ), "notes",
            parent, name, WType_TopLevel | WStyle_ContextHelp )
{
    setCaption( tr("Notes"));
    notesList = new QListBox(this, "notesList");
    setCentralWidget(notesList);

    m_manager.load();
    
    m_selected = -1;
    refreshList();

    QObject::connect(notesList, SIGNAL(returnPressed(QListBoxItem*)), this, SLOT(slotItemEdit()));
    QObject::connect(notesList, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(slotItemEdit()));

    connect( this, SIGNAL(categorySelected(const QString&)),
            this, SLOT(slotCategoryChanged(const QString&)) );

    initBars();
}

mainWindowWidget::~mainWindowWidget()
{
}

void mainWindowWidget::slotCategoryChanged( const QString &category )
{
    m_curCat = category;
    if ( m_curCat == tr( "All" ) )
        m_curCat = QString::null;

    refreshList();
}

void mainWindowWidget::deleteFile()
{
    if( notesList->count() > 0 )
    {
        switch (QMessageBox::warning(0, tr("Delete note"), tr("Really delete\n'") + notesList->currentText() + "'?",
                    QMessageBox::Yes, QMessageBox::No)) 
        {
            case QMessageBox::Yes:
                m_selected = notesList->currentItem();
                m_manager.remove( currentMemoUid() );
                m_manager.save();
                refreshList();
                break;

            case QMessageBox::No:
                // don't delete
                break;
        }
    }
}

int mainWindowWidget::currentMemoUid()
{
    int number = notesList->currentItem();
    if( number > -1 ) {
        MemoListItem *item = (MemoListItem *)notesList->item( number );
        return item->uid();
    }
    else
        return 0;
}

OPimMemo mainWindowWidget::currentMemo()
{
    OPimMemo memo;
    int uid = currentMemoUid();
    if( uid != 0 ) {
        memo = m_manager.memo( uid );
    }
    return memo;
}

void mainWindowWidget::editMemo( OPimMemo &memo, bool create )
{
    editWindowWidget *editWindow = new editWindowWidget(0, "editWindow", true);

    editWindow->readMemo(memo);
    if( create )
        editWindow->setCaption( tr( "Add note" ) );
    else
        editWindow->setCaption( tr( "Edit note" ) );

    if(QPEApplication::execDialog(editWindow) == QDialog::Accepted)
    {
        editWindow->writeMemo( memo );
        
        if( create ) {
            m_manager.add( memo );
            // the new selection will be always at the end and count is already
            // 1 bigger than m_selected
            m_selected = notesList->count();
        }
        else {
            m_manager.update( memo.uid(), memo );
        }
    }

    m_manager.save();

    refreshList();
}

int mainWindowWidget::create()
{
    QString name;
    int now = time(0);

    OPimMemo memo;
    memo.setUid( now );
    this->editMemo( memo, true );

    return 0;   //FIXME
}

void mainWindowWidget::slotItemEdit() 
{
    openFile();
}

void mainWindowWidget::slotItemDelete() 
{
    deleteFile();
}

void mainWindowWidget::slotItemNew() 
{
    create();
}

void mainWindowWidget::slotItemDuplicate()
{
    OPimMemo memo = currentMemo();
    if( memo.uid() != 0 ) {
        int now = time(0);
        memo.setUid( now );
        this->editMemo( memo, true );
    }
}

void mainWindowWidget::openFile()
{
    OPimMemo memo = currentMemo();
    if( memo.uid() != 0 ) {
        m_selected = notesList->currentItem();
        this->editMemo( memo, false );
    }
}

void mainWindowWidget::refreshList()
{
    QString title;
    int cat = 0;

    notesList->clear();

    if ( m_curCat != tr( "All" ) )
        cat = currentCatId();
    if ( m_curCat == tr( "Unfiled" ) )
        cat = -1;

    int filter = OPimMemoAccess::FilterCategory;

    OPimMemoAccess::List notes = m_manager.sorted( true, 0, filter, cat );
    
    //notes = m_access->allRecords();
    for ( OPimMemoAccess::List::Iterator it = notes.begin(); it != notes.end(); ++it ) {
        if ( (*it).text().isEmpty() )
            title = tr("<empty file>");
        else
            title = (*it).description();

        if (title.length() < 1)
            title = tr("<no title>");
        
        new MemoListItem( notesList, title, (*it).uid() );
    }
    
    if( notesList->count() > 0 )
    {
        if( m_selected == -1 )
        {
            notesList->setCurrentItem( 0 );
        }
        else
        {
            if( notesList->count() > m_selected  )
            {
                notesList->setCurrentItem( m_selected );
            }
            else
            {
                notesList->setCurrentItem( notesList->count() - 1 );
            }

        }
    }
    else
    {
        m_selected = -1;
    }

}

int mainWindowWidget::currentCatId()
{
    return m_manager.catId( m_curCat );
}

static const char * beamfile = "/tmp/obex/memo.txt";

void mainWindowWidget::slotItemBeam()
{
    Ir obex;

    OPimMemo memo = currentMemo();
    if( memo.uid() != 0 ) {
        odebug << "trying to beam" << oendl;
        QDir tmpdir( "/tmp/obex/" );
        tmpdir.mkdir( "/tmp/obex/" );
        tmpdir.remove( beamfile ); // delete if exists
        OPimMemoAccessBackend_Text *backend = new OPimMemoAccessBackend_Text( "notes", QString::fromLatin1(beamfile) );
        OPimMemoAccess acc( QString::null, QString::null, backend );
        acc.load();
        acc.add( memo );
        acc.save();
        obex.send( beamfile, memo.description(), "text/plain" );
    }
}

void mainWindowWidget::edit( int uid )    
{ 
    OPimMemo memo = m_manager.memo( uid );
    if( memo.uid() != 0 ) {
        this->editMemo( memo, false );
    }
}

void mainWindowWidget::slotItemFind()           { toBeDone();}
void mainWindowWidget::slotConfigure()          { toBeDone();}
void mainWindowWidget::flush()                  { toBeDone();}
void mainWindowWidget::reload()                 { toBeDone();}
bool mainWindowWidget::remove( int /*uid*/ )    { toBeDone(); return false; }
void mainWindowWidget::beam(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::show(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::add( const Opie::OPimRecord& ) { toBeDone();}

void mainWindowWidget::toBeDone(void)
{
    QMessageBox::information( this, "Notes", tr("Not yet implemented"));
}

