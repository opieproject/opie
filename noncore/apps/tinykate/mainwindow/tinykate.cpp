/***************************************************************************
                             tinykate.cpp
                        Tiny KATE mainwindow
                             -------------------
    begin                : November 2002
    copyright            : (C) 2002 by Joseph Wenninger <jowenn@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free softwaSre; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                         *
 *   ONLY VERSION 2 OF THE LICENSE IS APPLICABLE                           *
 *                                                                         *
 ***************************************************************************/

#include "tinykate.h"

#include "katedocument.h"
#include "kglobal.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/ofiledialog.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qaction.h>
#include <qtoolbutton.h>
#include <qmenubar.h>
#include <qmessagebox.h>


using namespace Opie::Ui;
TinyKate::TinyKate( QWidget *parent, const char *name, WFlags f) :
        QMainWindow( parent, name, f )
{
    shutDown=false;
    nextUnnamed=0;
    currentView=0;
    viewCount=0;
    setCaption(tr("TinyKATE"));
    KGlobal::setAppName("TinyKATE");

    QMenuBar *mb = new QMenuBar( this );
    mb->setMargin( 0 );

    tabwidget=new OTabWidget(this);
    setCentralWidget(tabwidget);
    connect(tabwidget,SIGNAL(currentChanged(QWidget*)),this,SLOT(slotCurrentChanged(QWidget*)));

    //FILE ACTIONS
    QPopupMenu *popup = new QPopupMenu( this );

    // Action for creating a new document
    QAction *a = new QAction( tr( "New" ),  Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
    a->addTo( popup );
    connect(a, SIGNAL(activated()), this, SLOT(slotNew()));

    // Action for opening an exisiting document
    a = new QAction( tr( "Open" ),Resource::loadPixmap( "fileopen" ) , QString::null, 0, this, 0 );
    a->addTo(popup);
    connect(a, SIGNAL(activated()), this, SLOT(slotOpen()));


    // Action for saving  document
    a = new QAction( tr( "Save" ), Resource::loadPixmap( "save" ) , QString::null, 0, this, 0 );
    a->addTo(popup);
    connect(a, SIGNAL(activated()), this, SLOT(slotSave()));

    // Action for saving document to a new name
    a = new QAction( tr( "Save As" ),Resource::loadPixmap( "save" ) , QString::null, 0, this, 0 );
    a->addTo(popup);
    connect(a, SIGNAL(activated()), this, SLOT(slotSaveAs()));

    // Action for closing the currently active document
    a = new QAction( tr( "Close" ), Resource::loadPixmap( "quit_icon" ) , QString::null, 0, this, 0 );
    a->addTo(popup);
    connect(a, SIGNAL(activated()), this, SLOT(slotClose()));


    mb->insertItem(tr("File"),popup);

    //EDIT ACTIONS

    // Action for cutting text
    editCut = new QToolButton( 0 );
    editCut->setAutoRaise( true );
    editCut->setIconSet(  Resource::loadPixmap( "cut" ) );

    // Action for Copying text
    editCopy = new QToolButton( 0 );
    editCopy->setAutoRaise( true );
    editCopy->setIconSet( Resource::loadPixmap( "copy" ) );

    // Action for pasting text
    editPaste =  new QToolButton( 0 );
    editPaste->setAutoRaise( true );
    editPaste->setIconSet( Resource::loadPixmap( "paste" ) );

    // Action for finding / replacing  text
    editFindReplace = new QToolButton( 0 );
    editFindReplace->setAutoRaise( true );
    editFindReplace->setIconSet( Resource::loadPixmap("find") );

    // Action for undo
    editUndo = new QToolButton( 0 );
    editUndo->setAutoRaise( true );
    editUndo->setIconSet( Resource::loadPixmap( "undo" ) );

    // Action for redo
    editRedo = new QToolButton( 0 );
    editRedo->setAutoRaise( true );
    editRedo->setIconSet( Resource::loadPixmap( "redo" ) );

    //VIEW ACITONS
    popup = new QPopupMenu( this );

    viewIncFontSizes = new QAction( tr( "Font +" ), QString::null, 0, this, 0 );
    viewIncFontSizes->addTo( popup );

    viewDecFontSizes = new QAction( tr( "Font -" ), QString::null, 0, this, 0 );
    viewDecFontSizes->addTo( popup );

    mb->insertItem(tr("View"),popup);

    popup = new QPopupMenu( this );
    mb->insertItem(tr("Utils"),popup);


    mb->insertItem( editCut );
    mb->insertItem( editCopy );
    mb->insertItem( editPaste );
    mb->insertItem( editFindReplace );
    mb->insertItem( editUndo );
    mb->insertItem(  editRedo );


    //Highlight management
    hlmenu=new QPopupMenu(this);
    HlManager *hlm=HlManager::self();
    for (int i=0;i<hlm->highlights();i++)
    {
        hlmenu->insertItem(hlm->hlName(i),i);
    }
    popup->insertItem(tr("Highlighting"),hlmenu);


    utilSettings = new QAction( tr( "Settings" ), QString::null, 0, this, 0 );
    utilSettings->addTo( popup);

    if( qApp->argc() > 1) open(qApp->argv()[1]);
    else slotNew();

}

TinyKate::~TinyKate( )
{
    owarn << "TinyKate destructor\n" << oendl;

    shutDown=true;
    while (currentView!=0)
    {
        slotClose();
    }

    if( KGlobal::config() != 0 )
    {
        owarn << "deleting KateConfig object..\n" << oendl;
        delete KGlobal::config();
    }
}

void TinyKate::slotOpen( )
{
    QString filename = OFileDialog::getOpenFileName( OFileSelector::EXTENDED_ALL,
                       QString::null);
    if (!filename.isEmpty())
    {
        open(filename);
    }
}

void TinyKate::open(const QString & filename)
{
    KateDocument *kd= new KateDocument(false, false, this,0,this);
    KTextEditor::View *kv;
    QString realFileName;
    //check if filename is a .desktop file
    if ( filename.find( ".desktop", 0, true ) )
    {
        switch ( QMessageBox::warning( this, tr( "TinyKATE" ),
                                       tr("TinyKATE has detected<BR>you selected a <B>.desktop</B> file.<BR>Open <B>.desktop</B> file or <B>linked</B> file?" ),
                                       tr(".desktop File"),
                                       tr("Linked Document"), 0, 1, 1 ) )
        {
        case 0: //desktop
            realFileName = filename;
            break;
        case 1: //linked
            DocLnk docLnk( filename );
            realFileName = docLnk.file();
            break;
        };
    }

    QFileInfo fileInfo( realFileName );
    QString filenamed = fileInfo.fileName();
    tabwidget->addTab(kv=kd->createView(tabwidget,"bLAH"),"tinykate/tinykate", filenamed );
    odebug << realFileName << oendl;

    kd->setDocName( filenamed);
    kd->open( realFileName );
    viewCount++;
}

void TinyKate::setDocument(const QString& fileref)
{
    open( fileref );
}

void TinyKate::slotCurrentChanged( QWidget * view)
{
    if (currentView)
    {

        disconnect(editCopy,SIGNAL(activated()),currentView,SLOT(copy()));
        disconnect(editCut,SIGNAL(activated()),currentView,SLOT(cut()));
        disconnect(editPaste,SIGNAL(activated()),currentView,SLOT(paste()));
        disconnect(editUndo,SIGNAL(activated()),currentView,SLOT(undo()));
        disconnect(editRedo,SIGNAL(activated()),currentView,SLOT(redo()));
        disconnect(viewIncFontSizes,SIGNAL(activated()), currentView,SLOT(slotIncFontSizes()));
        disconnect(viewDecFontSizes,SIGNAL(activated()), currentView,SLOT(slotDecFontSizes()));
        disconnect(hlmenu,SIGNAL(activated(int)), currentView,SLOT(setHl(int)));
        disconnect(utilSettings,SIGNAL(activated()), currentView,SLOT(configDialog()));
    }

    currentView=(KTextEditor::View*)view;

    connect(editCopy,SIGNAL(clicked()),currentView,SLOT(copy()));
    connect(editCut,SIGNAL(clicked()),currentView,SLOT(cut()));
    connect(editPaste,SIGNAL(clicked()),currentView,SLOT(paste()));
    connect(editUndo,SIGNAL(clicked()),currentView,SLOT(undo()));
    connect(editRedo,SIGNAL(clicked()),currentView,SLOT(redo()));
    connect(viewIncFontSizes,SIGNAL(activated()), currentView,SLOT(slotIncFontSizes()));
    connect(viewDecFontSizes,SIGNAL(activated()), currentView,SLOT(slotDecFontSizes()));
    connect(hlmenu,SIGNAL(activated(int)), currentView,SLOT(setHl(int)));
    connect(utilSettings,SIGNAL(activated()), currentView,SLOT(configDialog()));

}

void TinyKate::slotNew( )
{
    KateDocument *kd= new KateDocument(false, false, this,0,this);
    KTextEditor::View *kv;
    tabwidget->addTab(kv=kd->createView(tabwidget,"BLAH"),
                      "tinykate/tinykate",
                      tr("Unnamed %1").arg(nextUnnamed++));
    viewCount++;
}

void TinyKate::slotClose( )
{
    if (currentView==0) return;
    KTextEditor::View *dv=currentView;
    currentView=0;
    tabwidget->removePage(dv);
    delete dv->document();
    viewCount--;
    if ((!viewCount) && (!shutDown))  slotNew();
}

void TinyKate::slotSave()
{
    // feel free to make this how you want
    if (currentView==0) return;

    //  KateView *kv = (KateView*) currentView;
    KateDocument *kd = (KateDocument*) currentView->document();
    //  odebug << "saving file "+kd->docName() << oendl;
    if( kd->docName().isEmpty())
        slotSaveAs();
    else
        kd->saveFile();
    //    kv->save();
    //    kd->saveFile();
}

void TinyKate::slotSaveAs()
{
    if (currentView==0) return;
    KateDocument *kd = (KateDocument*) currentView->document();

    QString filename= OFileDialog::getSaveFileName(OFileSelector::EXTENDED_ALL,
                      QString::null);
    if (!filename.isEmpty())
    {
        odebug << "saving file "+filename << oendl;
        QFileInfo fi(filename);
        QString filenamed = fi.fileName();
        kd->setDocFile( filename);
        kd->setDocName( filenamed);
        kd->saveFile();
        //   KTextEditor::View *dv = currentView;
        //     tabwidget->changeTab( dv, filenamed);
        // need to change tab label here
    }

}
