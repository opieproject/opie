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

mainWindowWidget::mainWindowWidget( QWidget *parent, const char *name, WFlags)
    : Opie::OPimMainWindow( "Notes", "Notes", tr( "Note" ), "notes",
            parent, name, WType_TopLevel | WStyle_ContextHelp )
{
    setCaption( tr("Notes"));
    notesList = new QListBox(this, "notesList");
    setCentralWidget(notesList);

    QString documentsDirName = QPEApplication::documentDir() + "/text/plain/";

    QDir fileList( documentsDirName );

    m_access = new OPimMemoAccess();
    m_access->load();
    
    if(!fileList.exists())
    {
        fileList.setPath(QPEApplication::documentDir() + "/text/");

        if(!fileList.exists())
        {
            QString text;

            if(!fileList.mkdir(fileList.absPath()))
            {
                QMessageBox::critical(0, tr("i/o error"), tr("Could not create directory '%1'").arg( fileList.absPath() ) );
            }
            else
            {
                fileList.setPath(documentsDirName);

                if(!fileList.mkdir(fileList.absPath()))
                {
                    QMessageBox::critical(0, tr("i/o error"), tr("Could not create directory '%1'").arg( fileList.absPath() ) );
                }
            }
        }
    }

    this->selected = -1;
    refreshList();

    QObject::connect(notesList, SIGNAL(returnPressed(QListBoxItem*)), this, SLOT(slotItemEdit()));
    QObject::connect(notesList, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(slotItemEdit()));

    initBars();
}

mainWindowWidget::~mainWindowWidget()
{
    delete m_access;
}

void mainWindowWidget::deleteFile()
{
    if( notesList->count() > 0 )
    {
        switch (QMessageBox::warning(0, tr("Delete note"), tr("Really delete\n'") + notesList->currentText() + "'?",
                    QMessageBox::Yes, QMessageBox::No)) 
        {
            case QMessageBox::Yes:
                this->selected = notesList->currentItem();
                m_access->remove( m_notes[notesList->currentItem()] );
                m_access->save();
                refreshList();
                break;

            case QMessageBox::No:
                // don't delete
                break;
        }
    }
}

void mainWindowWidget::editMemo( OPimMemo &memo, bool create )
{
    editWindowWidget *editWindow = new editWindowWidget(0, "editWindow", true);

    editWindow->readMemo(memo);

    if(QPEApplication::execDialog(editWindow) == QDialog::Accepted)
    {
        editWindow->writeMemo( memo );
        
        if( create ) {
            m_access->add( memo );
            // the new selection will be always at the end and count is already
            // 1 bigger than selected
            this->selected = notesList->count();
        }
        else {
            m_access->replace( memo );
        }
    }

    m_access->save();

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
    int number = notesList->currentItem();
    if( number > -1 ) {
        OPimMemo memo = m_notes[number];
        int now = time(0);
        memo.setUid( now );
        this->editMemo( memo, true );
    }
}

void mainWindowWidget::openFile()
{
    int number = notesList->currentItem();

    if( notesList->count() > 0 )
    {
        this->selected = number;
        OPimMemo memo = m_notes[number];
        this->editMemo( memo, false );
    }
}

void mainWindowWidget::refreshList()
{
    QString title;

    notesList->clear();

    m_notes = m_access->allRecords();
    for ( OPimMemoAccess::List::Iterator it = m_notes.begin(); it != m_notes.end(); ++it ) {
        if ( (*it).text().isEmpty() )
            title = tr("<empty file>");
        else
            title = (*it).description();

        if (title.length() < 1)
            title = tr("<no title>");
        
        notesList->insertItem( title );
    }
    
    if( notesList->count() > 0 )
    {
        if( this->selected == -1 )
        {
            notesList->setCurrentItem( 0 );
        }
        else
        {
            if( notesList->count() > this->selected  )
            {
                notesList->setCurrentItem( this->selected );
            }
            else
            {
                notesList->setCurrentItem( notesList->count() - 1 );
            }

        }
    }
    else
    {
        this->selected = -1;
    }

}

static const char * beamfile = "/tmp/obex/memo.txt";

void mainWindowWidget::slotItemBeam()
{
    Ir obex;

    int number = notesList->currentItem();
    if( number > -1 ) {
        OPimMemo memo = m_notes[number];
    
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

void mainWindowWidget::slotItemFind()           { toBeDone();}
void mainWindowWidget::slotConfigure()          { toBeDone();}
void mainWindowWidget::flush()                  { toBeDone();}
void mainWindowWidget::reload()                 { toBeDone();}
bool mainWindowWidget::remove( int /*uid*/ )    { toBeDone(); return false; }
void mainWindowWidget::beam(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::show(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::edit(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::add( const Opie::OPimRecord& ) { toBeDone();}

void mainWindowWidget::toBeDone(void)
{
    QMessageBox::information( this, "Notes", tr("Not yet implemented"));
}

