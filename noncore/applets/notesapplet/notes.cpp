/**********************************************************************
** Copyright (C) 2002 L.J. Potter <llornkcor@handhelds.org>

**  All rights reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "notes.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/otaskbarapplet.h>
#include <opie2/oresource.h>
#include <opie2/opimmemo.h>

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include <qpe/ir.h>
#include <qpe/config.h>

using namespace Opie;
using namespace Opie::Notes;

/* QT */
#include <qmultilineedit.h>
#include <qlistbox.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfile.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtimer.h>

//===========================================================================

MemoListItem::MemoListItem ( QListBox *listbox, const QString &text, int uid )
    : QListBoxText( listbox, text ), m_uid( uid )
{
}

int MemoListItem::uid()
{
    return m_uid;
}

//===========================================================================

NotesControl::NotesControl( QWidget *, const char * )
        : QVBox( 0, "NotesControl",/* WDestructiveClose | */WStyle_StaysOnTop )
//        : QFrame( parent, name, WDestructiveClose | WStyle_StaysOnTop | WType_Popup )
{
    Config cfg("Notes");
    cfg.setGroup("Options");
    m_showMax = cfg.readBoolEntry("ShowMax", false);

    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    m_loaded = false;
    m_edited = false;
    QVBox *vbox = new QVBox( this, "Vlayout" );
    QHBox  *hbox = new QHBox( this, "HLayout" );

    m_editArea = new QMultiLineEdit( vbox, "OpieNotesView" );

    m_notesList = new QListBox( vbox, "OpieNotesBox" );

    QPEApplication::setStylusOperation( m_notesList->viewport(), QPEApplication::RightOnHold );

    m_notesList->setFixedHeight( 50 );

    vbox->setMargin( 6 );
    vbox->setSpacing( 3 );


    setFocusPolicy(QWidget::StrongFocus);

    newButton = new QPushButton( hbox, "newButton" );
    newButton->setText(tr("New"));

    saveButton = new QPushButton( hbox, "saveButton" );
    saveButton->setText(tr("Save"));

    deleteButton = new QPushButton( hbox, "deleteButton" );
    deleteButton->setText(tr("Delete"));


    connect( m_notesList, SIGNAL( mouseButtonPressed(int,QListBoxItem*,const QPoint&)),
             this,SLOT( boxPressed(int,QListBoxItem*,const QPoint&)) );

    connect( m_notesList, SIGNAL(highlighted(const QString&)), this, SLOT(slotBoxSelected(const QString&)));

    connect( &menuTimer, SIGNAL( timeout() ), SLOT( showMenu() ) );

    connect( m_editArea,SIGNAL( textChanged() ), this, SLOT(slotViewEdited() ) );

    connect( newButton, SIGNAL(clicked()), this, SLOT(slotNewButton()) );
    connect( saveButton, SIGNAL(clicked()), this, SLOT(slotSaveButton()) );
    connect( deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteButton()) );

    m_manager.load();
    m_selected = -1;
    setCaption("Notes");
}

void NotesControl::showAutoMax()
{
    if( m_showMax ) {
        odebug << "show max" << oendl;
        showMaximized();
    }
    else {
        odebug << "no show max" << oendl;
        QWidget *wid = QPEApplication::desktop();
        QRect rect = QApplication::desktop()->geometry();
        setGeometry( ( wid->width() / 2) - ( width() / 2 ) , 28 , wid->width() -10 , 180);
        move ( (rect.center()/2) - (this->rect().center()/2));
        show();
    }
}

void NotesControl::showEvent( QShowEvent *e )
{
    refreshList();
    load();
    m_editArea->setFocus();

    QVBox::showEvent( e );
}

void NotesControl::closeEvent( QCloseEvent *e )
{
    if( m_edited )
        save();

    QVBox::closeEvent( e );
}

int NotesControl::selectedMemoUid()
{
    int number = m_notesList->currentItem();
    if( number > -1 ) {
        MemoListItem *item = (MemoListItem *)m_notesList->item( number );
        return item->uid();
    }
    else
        return 0;
}

void NotesControl::slotSaveButton()
{
    save();
    refreshList();
}

void NotesControl::slotDeleteButton()
{
    if( m_notesList->count() > 0 && m_loaded ) {
        switch (QMessageBox::warning(this, tr("Delete note"), tr("Really delete\n'") + m_notesList->currentText() + "'?",
                    QMessageBox::Yes, QMessageBox::No))
        {
            case QMessageBox::Yes:
                m_selected = m_notesList->currentItem();
                m_manager.remove( selectedMemoUid() );
                m_manager.save();
                refreshList();
                load();
                break;

            case QMessageBox::No:
                // don't delete
                break;
        }
    }
}

void NotesControl::slotNewButton()
{
    if( m_edited ) {
        save();
        refreshList();
    }

    m_selected = -1;
    m_editArea->clear();
    m_editArea->setFocus();
    m_notesList->clearSelection();
    m_currentMemo = OPimMemo();
    m_edited = false;
    m_loaded = false;
}

void NotesControl::slotBeamButton()
{
    Ir ir;
    if(ir.supported()){
        this->hide();
        QString selectedText = m_notesList->currentText();
        if( !selectedText.isEmpty()) {
            QString file = QDir::homeDirPath() + "/" +selectedText;
            QFile f(file);
            Ir *irFile = new Ir(this, "IR");
            connect( irFile, SIGNAL(done(Ir*)), this, SLOT( slotBeamFinished(Ir*) ) );
            irFile->send( file, "Note", "text/plain" );
        }
    }
}

void NotesControl::slotBeamFinished(Ir *)
{
    this->show();
}

void NotesControl::boxPressed(int mouse, QListBoxItem *, const QPoint&)
{
    switch (mouse) {
        case 1:
            break;
        case 2:
            menuTimer.start( 500, TRUE );
            break;
    };
}

void NotesControl::slotBoxSelected( const QString & )
{
    if( m_edited ) {
        save();
    }

    load();
}


void NotesControl::showMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    m->insertItem( tr( "Beam Out" ), this,  SLOT( slotBeamButton() ));
    m->insertItem( tr( "Toggle Maximized" ), this,  SLOT( slotShowMax() ));
    m->insertSeparator();
    m->insertItem( tr( "Delete" ), this,  SLOT( slotDeleteButton() ));
    m->setFocus();
    m->exec( QCursor::pos() );

    delete m;
}

void NotesControl::focusOutEvent ( QFocusEvent * e )
{
    if( e->reason() == QFocusEvent::Popup )
        save();
    else {
        if(!m_loaded) {
            refreshList();
            load();
        }
    }
    QWidget::focusOutEvent(e);
}

void NotesControl::save()
{
    if( m_edited ) {
        if( m_loaded ) {
            odebug << "save: LOADED" << oendl;
            m_currentMemo.setText( m_editArea->text() );
            m_manager.update( m_currentMemo.uid(), m_currentMemo );
        }
        else {
            odebug << "save: NOTLOADED" << oendl;
            int now = time(0);
            m_currentMemo.setUid( now );
            m_currentMemo.setText( m_editArea->text() );
            m_manager.add( m_currentMemo );
        }
        m_manager.save();
        
        m_edited = false;

//X    Config cfg("Notes");
//X    cfg.setGroup("Docs");
//X        cfg.writeEntry( "LastDoc", oldDocName );
//X        cfg.write();

    }
    else
        odebug << "Save:: NOTEDITED" << oendl;
}

void NotesControl::refreshList()
{
    int cat = 0;

/*    if ( m_curCat != tr( "All" ) )
        cat = currentCatId();
    if ( m_curCat == tr( "Unfiled" ) )
        cat = -1;

    int filter = OPimMemoAccess::FilterCategory;*/
    int filter = 0;

    OPimMemoAccess::List notes = m_manager.sorted( true, 0, filter, cat );
    populateList( notes );
    
    update();
}

void NotesControl::populateList( OPimMemoAccess::List &list )
{
    QString title;
    int uid = 0;
    int currentUid = m_currentMemo.uid();

    m_notesList->clear();

    for ( OPimMemoAccess::List::Iterator it = list.begin(); it != list.end(); ++it ) {
        if ( (*it).text().isEmpty() )
            title = tr("<empty file>");
        else
            title = (*it).description();

        if (title.length() < 1)
            title = tr("<no title>");

        uid = (*it).uid();

        new MemoListItem( m_notesList, title, uid );
        if( m_selected == -1 && uid == currentUid )
            m_selected = m_notesList->count() - 1;
    }

    if( m_notesList->count() > 0 ) {
        if( m_selected == -1 ) {
            m_notesList->setCurrentItem( 0 );
        }
        else {
            if( m_notesList->count() > m_selected  ) {
                m_notesList->setCurrentItem( m_selected );
            }
            else {
                m_notesList->setCurrentItem( m_notesList->count() - 1 );
            }
        }
    }
    else {
        m_selected = -1;
    }
}

void NotesControl::load()
{
/*X     Config cfg("Notes");
        cfg.setGroup("Docs");
        cfg.writeEntry( "LastDoc", oldDocName );
        cfg.write();
    }*/

    int uid = selectedMemoUid();
    if( uid != 0 ) {
        m_currentMemo = m_manager.memo( uid );
        m_editArea->setText( m_currentMemo.text() );
        m_loaded = true;
        m_edited = false;
    }
    else {
        m_editArea->clear();
        m_loaded = false;
        m_edited = false;
    }
}

void NotesControl::slotViewEdited()
{
    m_edited = true;
}


void NotesControl::slotShowMax()
{
     Config cfg("Notes");
     cfg.setGroup("Options");
     m_showMax = !m_showMax;
     cfg.writeEntry("ShowMax", m_showMax);
     cfg.write();
     hide();
}

//===========================================================================

NotesApplet::NotesApplet( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );
    m_notesPixmap = Opie::Core::OResource::loadImage( "edit", Opie::Core::OResource::SmallIcon );
    vc = new NotesControl;
}

NotesApplet::~NotesApplet()
{
    delete vc;
}

int NotesApplet::position()
{
    return 6;
}

void NotesApplet::mousePressEvent( QMouseEvent *)
{
    if( !vc->isHidden() )
        vc->close();
    else
        vc->showAutoMax();
}

void NotesApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    p.drawPixmap( 0, 2, m_notesPixmap );
}


EXPORT_OPIE_APPLET_v1(  NotesApplet )

