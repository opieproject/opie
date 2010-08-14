/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
** Copyright (C) 2003 Stefan Eilers (eilers.stefan@epost.de)
**
** This file is part of the Open Palmtop Environment (see www.opie.info).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
**
**********************************************************************/

#define QTOPIA_INTERNAL_FD

// #include "addresssettings.h"
#include "addressbook.h"

#include <opie2/odebug.h>
#include <opie2/ofileselector.h>
#include <opie2/ofiledialog.h>
#include <opie2/opimcontact.h>
#include <opie2/ocontactaccessbackend_vcard.h>
#include <opie2/oresource.h>
#include <opie2/opimautoconvert.h>

#include <qpe/ir.h>
#include <qpe/qpemessagebox.h>
#include <qmenubar.h>
#include <qpe/qpeapplication.h>

#include <qaction.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qtimer.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


#include "picker.h"
#include "configdlg.h"

extern QString addressbookPersonalVCardName();

AddressbookWindow::AddressbookWindow( QWidget *parent, const char *name,
                      WFlags /*f*/ )
    : Opie::OPimMainWindow( "Addressbook", "Contacts", tr( "Contact" ), "AddressBook",
                            parent, name, WType_TopLevel | WStyle_ContextHelp ),
      abEditor(0l),
      syncing(false)
{
    setCaption( tr( "Contacts" ) );

    isLoading = true;

    initBars();

    m_config.load();

    m_forceClose = false;
    if( ! Opie::OPimAutoConverter::promptConvertData( Opie::Pim::OPimGlobal::CONTACTLIST, this, caption() ) ) {
        m_forceClose = true;
        QTimer::singleShot(0, qApp, SLOT(quit() ) );
        return;
    }
    
    // Create Views
    m_listContainer = new QWidget( this );
    QVBoxLayout *vb = new QVBoxLayout( m_listContainer );

    m_abView = new AbView( m_listContainer, m_config.orderList() );
    vb->addWidget( m_abView );
    connect( m_abView, SIGNAL(signalViewSwitched(int)),
         this, SLOT(slotViewSwitched(int)) );

    QObject::connect( m_abView, SIGNAL(signalNotFound()), this, SLOT(slotNotFound()) );

    // Letter Picker
    pLabel = new LetterPicker( m_listContainer );
    connect(pLabel, SIGNAL(letterClicked(char)), this, SLOT(slotSetLetter(char)));
    connect(m_abView, SIGNAL(signalClearLetterPicker()), pLabel, SLOT(clear()) );

    vb->addWidget( pLabel );

    // Quick search bar
    m_searchBar = new OFloatBar( "Search", this, QMainWindow::Top, true );
    m_searchBar->setHorizontalStretchable( true );
    m_searchBar->hide();
    m_searchEdit = new QLineEdit( m_searchBar, "m_searchEdit" );

    m_searchBar->setStretchableWidget( m_searchEdit );
    connect( m_searchEdit, SIGNAL(returnPressed()), this, SLOT(slotFind()) );

    QAction *a = new QAction( tr( "Start Search" ),
                              Opie::Core::OResource::loadPixmap( "find", Opie::Core::OResource::SmallIcon ),
                              QString::null, 0, this, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotFind()) );
    a->addTo( m_searchBar );

    // Insert Contact menu items
    QActionGroup *items = new QActionGroup( this, QString::null, false );

    m_actionMail = new QAction( tr( "Write Mail To" ),
                                Opie::Core::OResource::loadPixmap( "addressbook/sendmail", Opie::Core::OResource::SmallIcon ),
                                QString::null, 0, items, 0 );
    connect( m_actionMail, SIGNAL(activated()), this, SLOT(writeMail()) );

    a = new QAction( tr("Import vCard"),
                     Opie::Core::OResource::loadPixmap( "addressbook/fileimport", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, items, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(importvCard()) );

    a = new QAction( tr("Export vCard"),
                     Opie::Core::OResource::loadPixmap( "addressbook/fileexport", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, items, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(exportvCard()) );

    m_actionPersonal = new QAction( tr("My Personal Details"),
                                    Opie::Core::OResource::loadPixmap( "addressbook/identity", Opie::Core::OResource::SmallIcon ),
                                    QString::null, 0, items, 0 , true );
    connect( m_actionPersonal, SIGNAL(activated()), this, SLOT(slotPersonalView()) );

    insertItemMenuItems( items );

    // Insert View menu items
    items = new QActionGroup( this, QString::null, false );

    a = new QAction( tr("Show quick search bar"),QString::null, 0, items, 0, true );
    connect( a, SIGNAL(toggled(bool)), this, SLOT(slotShowFind(bool)) );

    insertViewMenuItems( items );

    // Fontsize
    defaultFont = new QFont( m_abView->font() );
     slotSetFont(m_config.fontSize());
    m_curFontSize = m_config.fontSize();

    setCentralWidget(m_listContainer);

    //    odebug << "adressbook contrsuction: t=" << t.elapsed() << oendl;
    connect( qApp, SIGNAL(flush()), this, SLOT(flush()) );
    connect( qApp, SIGNAL(reload()), this, SLOT(reload()) );
    connect( qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
    this, SLOT(appMessage(const QCString&,const QByteArray&)) );

    isLoading = false;

    // Handle category selection
    setViewCategory( m_config.category() );
    m_abView->setShowByCategory( m_config.category() );
    connect( this, SIGNAL(categorySelected(const QString&)),
             this, SLOT(slotSetCategory(const QString&)) );
}

void AddressbookWindow::slotSetFont( int size )
{
    odebug << "void AddressbookWindow::slotSetFont( " << size << " )" << oendl;

    if (size > 2 || size < 0)
        size = 1;

    m_config.setFontSize( size );

    switch (size) {
    case 0:
        m_abView->setFont( QFont( defaultFont->family(), defaultFont->pointSize() - 2 ) );
        break;
    case 1:
         m_abView->setFont( *defaultFont );
        break;
    case 2:
         m_abView->setFont( QFont( defaultFont->family(), defaultFont->pointSize() + 2 ) );
        break;
    }
}



void AddressbookWindow::importvCard() {
        QString str = Opie::Ui::OFileDialog::getOpenFileName( 1,"/");//,"", "*", this );
        if(!str.isEmpty() ) {
        setDocument((const QString&) str );
    }

}
void AddressbookWindow::exportvCard()
{
    odebug << "void AddressbookWindow::exportvCard()" << oendl;
        QString filename = Opie::Ui::OFileDialog::getSaveFileName( 1,"/home/"); //,"", "*", this );
        if( !filename.isEmpty() &&  ( filename[filename.length()-1] != '/' ) ) {
        odebug << " Save to file " << filename << ", (" << filename.length()-1 << ")" << oendl;
        Opie::OPimContact curCont = m_abView->currentEntry();
        if ( !curCont.isEmpty() ) {
            Opie::OPimContactAccessBackend* vcard_backend = new Opie::OPimContactAccessBackend_VCard( QString::null,
                                                filename );
            Opie::OPimContactAccess* access = new Opie::OPimContactAccess ( "addressbook_exp", QString::null , vcard_backend, true );
            if ( access ) {
                access->add( curCont );
                access->save();
            }
            delete access;
        }
        else
            QMessageBox::critical( 0, "Export VCard",
                           QString( tr( "You have to select a contact !") ) );

    }
    else
        QMessageBox::critical( 0, "Export VCard",
                       QString( tr( "You have to set a filename !") ) );
}

void AddressbookWindow::setDocument( const QString &filename )
{
    odebug << "void AddressbookWindow::setDocument( " << filename << " )" << oendl;

    // Switch to default backend. This should avoid to import into
    // the personal database accidently.
    if ( m_actionPersonal->isOn() ) {
        m_actionPersonal->setOn( false );
        slotPersonalView();
    }

    if ( filename.find(".vcf") != int(filename.length()) - 4 ) {



        switch( QMessageBox::information( this, tr ( "Right file type ?" ),
                          tr( "The selected file \n does not end with \".vcf\".\n Do you really want to open it?" ),
                          tr( "&Yes" ), tr( "&No" ), QString::null,
                          0,      // Enter == button 0
                          2 ) ) { // Escape == button 2
        case 0:
            odebug << "YES clicked" << oendl;
            break;
        case 1:
            odebug << "NO clicked" << oendl;
            return;
            break;
        }
    }

    Opie::OPimContactAccessBackend* vcard_backend = new Opie::OPimContactAccessBackend_VCard( QString::null,
                                     filename );
    Opie::OPimContactAccess* access = new Opie::OPimContactAccess ( "addressbook", QString::null , vcard_backend, true );
    access->load();
    Opie::OPimContactAccess::List allList = access->allRecords();
    odebug << "Found number of contacts in File: " << allList.count() << oendl;

    if ( !allList.count() ) {
        QMessageBox::information( this, "Import VCard",
                      "It was impossible to import\nthe VCard.\n"
                      "The VCard may be corrupted!" );
    }

    bool doAsk = true;
    Opie::OPimContactAccess::List::Iterator it;
    for ( it = allList.begin(); it != allList.end(); ++it ) {
        odebug << "Adding Contact from: " << (*it).fullName() << oendl;
        if ( doAsk ) {
            switch( QMessageBox::information( this, tr ( "Add Contact?" ),
                              tr( "Do you really want to add \n%1 to your contacts?" )
                              .arg( (*it).fullName().latin1() ),
                              tr( "&Yes" ), tr( "&No" ), tr( "&All Yes"),
                              0,      // Enter == button 0
                              2 ) ) { // Escape == button 2
            case 0:
                odebug << "YES clicked" << oendl;
                m_abView->addEntry( *it );
                break;
            case 1:
                odebug << "NO clicked" << oendl;
                break;
            case 2:
                odebug << "YesAll clicked" << oendl;
                m_abView->addEntry( *it );
                doAsk = false;
                break;
            }
        }
        else
            m_abView->addEntry( *it );

    }

    delete access;
}

void AddressbookWindow::resizeEvent( QResizeEvent *e )
{
    QMainWindow::resizeEvent( e );


}

AddressbookWindow::~AddressbookWindow()
{
    if( ! m_forceClose ) {
        ToolBarDock dock;
        int dummy;
        bool bDummy;
        getLocation ( listTools, dock, dummy, bDummy, dummy );
        m_config.setToolBarDock( dock );
        m_config.save();
    }
}

int AddressbookWindow::create()
{
    return 0;
}

bool AddressbookWindow::remove( int /*uid*/ )
{
    return false;
}

void AddressbookWindow::beam( int /*uid*/ )
{
}

void AddressbookWindow::show( int /*uid*/ )
{
}

void AddressbookWindow::edit( int /*uid*/ )
{
}

void AddressbookWindow::add( const Opie::OPimRecord& )
{
}

void AddressbookWindow::slotItemNew()
{
    Opie::OPimContact cnt;
    if( !syncing ) {
        editEntry( NewEntry );
    }
    else {
        QMessageBox::warning(this, tr("Contacts"),
                     tr("Can not edit data, currently syncing"));
    }
}

void AddressbookWindow::slotItemEdit()
{
    if(!syncing) {
        if (m_actionPersonal->isOn()) {
            editPersonal();
        }
        else {
            editEntry( EditEntry );
        }
    }
    else {
        QMessageBox::warning( this, tr("Contacts"),
                      tr("Can not edit data, currently syncing") );
    }
}

void AddressbookWindow::slotItemDuplicate()
{
    if(!syncing)
    {
        Opie::OPimContact entry = m_abView->currentEntry();
        entry.assignUid();
        m_abView->addEntry( entry );
        m_abView->setCurrentUid( entry.uid() );
    }
    else
    {
        QMessageBox::warning( this, tr("Contacts"),
                              tr("Can not edit data, currently syncing") );
    }
}

void AddressbookWindow::slotItemDelete()
{
    if(!syncing) {
        Opie::OPimContact tmpEntry = m_abView ->currentEntry();

        // get a name, do the best we can...
        QString strName = tmpEntry.fullName();
        if ( strName.isEmpty() ) {
            strName = tmpEntry.company();
            if ( strName.isEmpty() )
                strName = "No Name";
        }


        if ( QPEMessageBox::confirmDelete( this, tr( "Contacts" ),
                           strName ) ) {
            m_abView->removeEntry( tmpEntry.uid() );
        }
    }
    else {
        QMessageBox::warning( this, tr("Contacts"),
                      tr("Can not edit data, currently syncing") );
    }
}

static const char * beamfile = "/tmp/obex/contact.vcf";

void AddressbookWindow::slotItemBeam()
{
    QString beamFilename;
    Opie::OPimContact c;
    if ( m_actionPersonal->isOn() ) {
        beamFilename = addressbookPersonalVCardName();
        if ( !QFile::exists( beamFilename ) )
            return; // can't beam a non-existent file
        Opie::OPimContactAccessBackend* vcard_backend = new Opie::OPimContactAccessBackend_VCard( QString::null,
                                            beamFilename );
        Opie::OPimContactAccess* access = new Opie::OPimContactAccess ( "addressbook", QString::null , vcard_backend, true );
        access->load();
        Opie::OPimContactAccess::List allList = access->allRecords();
        Opie::OPimContactAccess::List::Iterator it = allList.begin();  // Just take first
        c = *it;

        delete access;
    }
    else {
        unlink( beamfile ); // delete if exists
        mkdir("/tmp/obex/", 0755);
        c = m_abView -> currentEntry();
        Opie::OPimContactAccessBackend* vcard_backend = new Opie::OPimContactAccessBackend_VCard( QString::null,
                                            beamfile );
        Opie::OPimContactAccess* access = new Opie::OPimContactAccess ( "addressbook", QString::null , vcard_backend, true );
        access->add( c );
        access->save();
        delete access;

        beamFilename = beamfile;
    }

    odebug << "Beaming: " << beamFilename << oendl;

    Ir *ir = new Ir( this );
    connect( ir, SIGNAL( done(Ir*) ), this, SLOT( beamDone(Ir*) ) );
    QString description = c.fullName();
    ir->send( beamFilename, description, "text/x-vCard" );
}

void AddressbookWindow::slotItemFind()
{
}

void AddressbookWindow::slotConfigure()
{
    ConfigDlg* dlg = new ConfigDlg( this, "Config" );
    dlg -> setConfig( m_config );
    if ( QPEApplication::execDialog( dlg ) ) {
        odebug << "Config Dialog accepted!" << oendl;
        m_config = dlg -> getConfig();
        if ( m_curFontSize != m_config.fontSize() ) {
            odebug << "Font was changed!" << oendl;
            m_curFontSize = m_config.fontSize();
            emit slotSetFont( m_curFontSize );
        }
        m_abView -> setListOrder( m_config.orderList() );
    }

    delete dlg;
}

void AddressbookWindow::slotShowFind( bool show )
{
    if ( show )
    {
        // Display search bar
        m_searchBar->show();
        m_abView -> inSearch();
        m_searchEdit->setFocus();
    }
    else
    {
        // Hide search bar
        m_searchBar->hide();
        m_abView -> offSearch();
    }
}

void AddressbookWindow::slotFind()
{
    m_abView->slotDoFind( m_searchEdit->text(), m_config.beCaseSensitive(), m_config.useRegExp(), false);

    m_searchEdit->clearFocus();
    // m_abView->setFocus();

}

void AddressbookWindow::slotViewBack()
{
    // :SX showList();
}

void AddressbookWindow::writeMail()
{
    Opie::OPimContact c = m_abView -> currentEntry();
    QString name = c.fileAs();
    QString email = c.defaultEmail();

    // I prefer the OPIE-Environment variable before the
    // QPE-one..
    QString basepath = QString::fromLatin1( getenv("OPIEDIR") );
    if ( basepath.isEmpty() )
        basepath = QString::fromLatin1( getenv("QPEDIR") );

    // Try to access the preferred. If not possible, try to
    // switch to the other one..
    if ( m_config.useQtMail() ) {
        odebug << "Accessing: " << (basepath + "/bin/qtmail") << oendl;
        if ( QFile::exists( basepath + "/bin/qtmail" ) ) {
            odebug << "QCop" << oendl;
            QCopEnvelope e("QPE/Application/qtmail", "writeMail(QString,QString)");
            e << name << email;
            return;
        }
        else
            m_config.setUseOpieMail( true );
    }
    if ( m_config.useOpieMail() ) {
        odebug << "Accessing: " << (basepath + "/bin/opiemail") << oendl;
        if ( QFile::exists( basepath + "/bin/opiemail" ) ) {
            odebug << "QCop" << oendl;
            QCopEnvelope e("QPE/Application/opiemail", "writeMail(QString,QString)");
            e << name << email;
            return;
        }
        else
            m_config.setUseQtMail( true );
    }

}

void AddressbookWindow::beamDone( Ir *ir )
{

    delete ir;
    unlink( beamfile );
}


static void parseName( const QString& name, QString *first, QString *middle,
               QString * last )
{

    int comma = name.find ( "," );
    QString rest;
    if ( comma > 0 ) {
        *last = name.left( comma );
        comma++;
        while ( comma < int(name.length()) && name[comma] == ' ' )
            comma++;
        rest = name.mid( comma );
    }
    else {
        int space = name.findRev( ' ' );
        *last = name.mid( space+1 );
        rest = name.left( space );
    }
    int space = rest.find( ' ' );
    if ( space <= 0 ) {
        *first = rest;
    }
    else {
        *first = rest.left( space );
        *middle = rest.mid( space+1 );
    }

}


void AddressbookWindow::appMessage(const QCString &msg, const QByteArray &data)
{
        bool needShow = false;
        odebug << "Receiving QCop-Call with message " << msg << oendl;


    if (msg == "editPersonal()") {
        editPersonal();

        // Categories might have changed, so reload
        reloadCategories();
    }
    else if (msg == "editPersonalAndClose()") {
        editPersonal();
        close();
    }
    else if ( msg == "addContact(QString,QString)" ) {
        QDataStream stream(data,IO_ReadOnly);
        QString name, email;
        stream >> name >> email;

        Opie::OPimContact cnt;
        QString fn, mn, ln;
        parseName( name, &fn, &mn, &ln );
        //    odebug << " " << fn << " - " << mn " - " << ln << oendl;
        cnt.setFirstName( fn );
        cnt.setMiddleName( mn );
        cnt.setLastName( ln );
        cnt.insertEmails( email );
        cnt.setDefaultEmail( email );
        cnt.setFileAs();

        m_abView -> addEntry( cnt );

        // :SXm_abView()->init( cnt );
        editEntry( EditEntry );

        // Categories might have changed, so reload
        reloadCategories();
    }
    else if ( msg == "beamBusinessCard()" ) {
        QString beamFilename = addressbookPersonalVCardName();
        if ( !QFile::exists( beamFilename ) )
            return; // can't beam a non-existent file

        Ir *ir = new Ir( this );
        connect( ir, SIGNAL( done(Ir*) ), this, SLOT( beamDone(Ir*) ) );
        QString description = "mycard.vcf";
        ir->send( beamFilename, description, "text/x-vCard" );
    }
    else if ( msg == "show(int)" ) {
                raise();
        QDataStream stream(data,IO_ReadOnly);
        int uid;
        stream >> uid;

        odebug << "Showing uid: " << uid << oendl;

        // Deactivate Personal View..
        if ( m_actionPersonal->isOn() ) {
            m_actionPersonal->setOn( false );
            slotPersonalView();
        }

        // Reset category and show as card..
        m_abView -> setShowByCategory( QString::null );
        m_abView -> setCurrentUid( uid );
        slotViewSwitched ( AbView::CardView );

            needShow = true;


    }
    else if ( msg == "edit(int)" ) {
        QDataStream stream(data,IO_ReadOnly);
        int uid;
        stream >> uid;

        // Deactivate Personal View..
        if ( m_actionPersonal->isOn() ) {
            m_actionPersonal->setOn( false );
            slotPersonalView();
        }

        // Reset category and edit..
        m_abView -> setShowByCategory( QString::null );
        m_abView -> setCurrentUid( uid );
        slotItemEdit();

        // Categories might have changed, so reload
        reloadCategories();
    }

        if (needShow)
            QPEApplication::setKeepRunning();
}

void AddressbookWindow::editEntry( EntryMode entryMode )
{
    Opie::OPimContact entry;
    if ( !abEditor ) {
        abEditor = new ContactEditor( entry, this, "editor" );
    }
    if ( entryMode == EditEntry )
        abEditor->setEntry( m_abView -> currentEntry() );
    else if ( entryMode == NewEntry )
        abEditor->setEntry( entry );

    // Set the dialog caption
    if ( m_actionPersonal->isOn() )
        abEditor->setCaption( tr( "Edit My Personal Details" ) );
    else
        abEditor->setCaption( tr( "Edit Contact" ) );

    // fix the focus...
    abEditor->setNameFocus();
    if ( QPEApplication::execDialog( abEditor ) == QDialog::Accepted ) {
        setFocus();
        if ( entryMode == NewEntry ) {
            Opie::OPimContact insertEntry = abEditor->entry();
            insertEntry.assignUid();
            m_abView -> addEntry( insertEntry );
            m_abView -> setCurrentUid( insertEntry.uid() );
        }
        else {
            Opie::OPimContact replEntry = abEditor->entry();

            if ( !replEntry.isValidUid() )
                replEntry.assignUid();

            m_abView -> replaceEntry( replEntry );
        }

        // Categories might have changed, so reload
        reloadCategories();
    }
}

void AddressbookWindow::editPersonal()
{
    Opie::OPimContact entry;

    // Switch to personal view if not selected
    // but take care of the menu, too
    if ( ! m_actionPersonal->isOn() ) {
        odebug << "*** ++++" << oendl;
        m_actionPersonal->setOn( true );
        slotPersonalView();
    }

    if ( !abEditor ) {
        abEditor = new ContactEditor( entry, this, "editor" );
    }

    abEditor->setPersonalView( true );
    editEntry( EditEntry );
    abEditor->setPersonalView( false );

}


void AddressbookWindow::slotPersonalView()
{
    odebug << "slotPersonalView()" << oendl;

    bool personal = m_actionPersonal->isOn();

    // Disable actions when showing personal details
    setItemNewEnabled( !personal );
    setItemDuplicateEnabled( !personal );
    setItemDeleteEnabled( !personal );
    m_actionMail->setEnabled( !personal );
    setShowCategories( !personal );

    // Display appropriate view
    m_abView->showPersonal( personal );

    if ( personal )
    {
        setCaption( tr( "Contacts - My Personal Details") );

        // Set category to 'All' to make sure personal details is visible
        setViewCategory( "All" );
        m_abView->setShowByCategory( "All" );

        // Temporarily disable letter picker
        pLabel->hide();
    }
    else
    {
        setCaption( tr( "Contacts") );

        // Re-enable letter picker
        pLabel->show();
    }
}

void AddressbookWindow::reload()
{
    syncing = false;
    m_abView->clear();
    m_abView->reload();
}

void AddressbookWindow::flush()
{
    syncing = true;
    m_abView->save();
}


void AddressbookWindow::closeEvent( QCloseEvent *e )
{
    if ( m_forceClose ) {
        e->accept();
        return;
    }
    
    if ( active_view == AbView::CardView )
    {
        if ( !m_actionPersonal->isOn() ) {
            // Switch to table view only if not editing personal details
            slotViewSwitched( AbView::TableView );
        }
        else {
            // If currently editing personal details, switch off personal view
            m_actionPersonal->setOn( false );
            slotPersonalView();
        }

        e->ignore();
        return;
    }
    if(syncing) {
        /* shouldn't we save, I hear you say? well its already been set
           so that an edit can not occur during a sync, and we flushed
           at the start of the sync, so there is no need to save
           Saving however itself would cause problems. */
        e->accept();
        return;
    }
    //################## shouldn't always save
    // True, but the database handles this automatically ! (se)
    if ( save() )
        e->accept();
    else
        e->ignore();
}

/*
  Returns true if it is OK to exit
*/

bool AddressbookWindow::save()
{
    if ( !m_abView->save() ) {
        if ( QMessageBox::critical( 0, tr( "Out of space" ),
                        tr("Unable to save information.\n"
                           "Free up some space\n"
                           "and try again.\n"
                           "\nQuit anyway?"),
                        QMessageBox::Yes|QMessageBox::Escape,
                        QMessageBox::No|QMessageBox::Default )
             != QMessageBox::No )
            return true;
        else
            return false;
    }
    return true;
}

#ifdef __DEBUG_RELEASE
void AddressbookWindow::slotSave()
{
    save();
}
#endif


void AddressbookWindow::slotNotFound()
{
    odebug << "Got not found signal!" << oendl;
    QMessageBox::information( this, tr( "Not Found" ),
                 "<qt>" + tr( "Unable to find a contact for this search pattern!" ) + "</qt>" );


}
void AddressbookWindow::slotWrapAround()
{
    odebug << "Got wrap signal!" << oendl;
//     if ( doNotifyWrapAround )
//         QMessageBox::information( this, tr( "End of list" ),
//                       tr( "End of list. Wrap around now...!" ) + "\n" );

}

void AddressbookWindow::slotSetCategory( const QString &category )
{
    odebug << "void AddressbookWindow::slotSetCategory( " << category << " )" << oendl;

    // Tell the view about the selected category
    QString cat = category;
    if ( cat == tr( "All" ) )
        cat = QString::null;
    m_config.setCategory( cat );
    m_abView -> setShowByCategory( cat );
}

void AddressbookWindow::slotViewSwitched( int view )
{
    odebug << "void AddressbookWindow::slotViewSwitched( " << view << " )" << oendl;

    // Tell the view about the selected view
    m_abView -> setShowToView ( (AbView::Views) view );
    active_view = view;
}


void AddressbookWindow::slotListView()
{
    slotViewSwitched( AbView::TableView );
}

void AddressbookWindow::slotCardView()
{
    slotViewSwitched( AbView::CardView );
}

void AddressbookWindow::slotSetLetter( char c ) {

     m_abView->setShowByLetter( c, m_config.letterPickerSearch() );

}

