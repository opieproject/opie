/***************************************************************************
                             tinykate.cpp
                        Tiny KATE mainwindow
                             -------------------
    begin                : November 2002
    copyright            : (C) 2002 by Joseph Wenninger <jowenn@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                         *
 *   ONLY VERSION 2 OF THE LICENSE IS APPLICABLE                           *
 *                                                                         *
 ***************************************************************************/
#include <qwidget.h>
#include <qaction.h>
#include <qlayout.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>

#include <opie/ofiledialog.h>

#include "tinykate.h"
#include "pics/file-new.xpm"
#include "pics/file-open.xpm"
#include "pics/file-save.xpm"
#include "pics/edit-undo.xpm"
#include "pics/edit-redo.xpm"

#include <katedocument.h>
#include <katehighlight.h>
#include <kateview.h>

TinyKate::TinyKate( QWidget *parent, const char *name, WFlags f) :
    QMainWindow( parent, name, f )
{ 
  nextUnnamed=0;
  currentView=0;
  viewCount=0;
  setCaption(tr("TinyKATE"));

  setToolBarsMovable(FALSE);

        QPEToolBar *bar = new QPEToolBar( this );
        bar->setHorizontalStretchable( TRUE );
        QPEMenuBar *mb = new QPEMenuBar( bar );
        mb->setMargin( 0 );

  tabwidget=new OTabWidget(this);
  setCentralWidget(tabwidget);
  connect(tabwidget,SIGNAL(currentChanged( QWidget *)),this,SLOT(slotCurrentChanged(QWidget *)));

//FILE ACTIONS
        QPopupMenu *popup = new QPopupMenu( this );

  // Action for creating a new document
        QAction *a = new QAction( tr( "New" ), QPixmap((const char**)file_new_xpm ), QString::null, 0, this, 0 );
        a->addTo( popup );
        connect(a, SIGNAL(activated()), this, SLOT(slotNew()));

  // Action for opening an exisiting document
        a = new QAction( tr( "Open" ), QPixmap((const char**)file_open_xpm), QString::null, 0, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotOpen()));


  // Action for saving  document
        a = new QAction( tr( "Save" ), QPixmap((const char**)file_save_xpm), QString::null, 0, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotSave()));

  // Action for saving document to a new name
        a = new QAction( tr( "Save As" ), QPixmap((const char**)file_save_xpm), QString::null, 0, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotSaveAs()));

  // Action for closing the currently active document
        a = new QAction( tr( "Close" ), QPixmap(), QString::null, 0, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotClose()));
  

  mb->insertItem(tr("File"),popup);

//EDIT ACTIONS

  // Action for cutting text
        editCut = new QAction( tr( "Cut" ), Resource::loadPixmap( "cut" ), QString::null, 0, this, 0 );
        editCut->addTo( bar );

  // Action for Copying text
        editCopy = new QAction( tr( "Copy" ), Resource::loadPixmap( "copy" ), QString::null, 0, this, 0 );
        editCopy->addTo( bar );

  // Action for pasting text
        editPaste = new QAction( tr( "Paste" ), Resource::loadPixmap( "paste" ), QString::null, 0, this, 0 );
        editPaste->addTo( bar );


  // Action for finding / replacing  text
        editFindReplace = new QAction( tr( "Find/Replace" ), Resource::loadPixmap("find"), QString::null, 0, this, 0 );
        editFindReplace->addTo( bar );

  // Action for undo
        editUndo = new QAction( tr( "Undo" ), QPixmap((const char**)edit_undo_xpm), QString::null, 0, this, 0 );
        editUndo->addTo( bar );

  // Action for redo
        editRedo = new QAction( tr( "Redo" ), QPixmap((const char**)edit_redo_xpm), QString::null, 0, this, 0 );
        editRedo->addTo( bar );

//VIEW ACITONS
        popup = new QPopupMenu( this );
  
        viewIncFontSizes = new QAction( tr( "Font +" ), QString::null, 0, this, 0 );
        viewIncFontSizes->addTo( popup );

        viewDecFontSizes = new QAction( tr( "Font -" ), QString::null, 0, this, 0 );
        viewDecFontSizes->addTo( popup );

  mb->insertItem(tr("View"),popup);



        popup = new QPopupMenu( this );
  mb->insertItem(tr("Utils"),popup);

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


void TinyKate::slotOpen( )
{
  QString filename=OFileDialog::getOpenFileName(OFileSelector::EXTENDED_ALL);
  if (!filename.isEmpty()) {  
    open(filename);
  }
}

void TinyKate::open(const QString & filename)
{
    KateDocument *kd= new KateDocument(false, false, this,0,this);
    KTextEditor::View *kv;
    QFileInfo fi(filename);
    QString filenamed = fi.fileName();
    tabwidget->addTab(kv=kd->createView(tabwidget,"bLAH"),"tinykate/tinykate", filenamed );
    qDebug(filename);

    kd->setDocName( filenamed);
    kd->open(filename);
    viewCount++;
}

void TinyKate::slotCurrentChanged( QWidget * view)
{
  if (currentView) {

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
  
  connect(editCopy,SIGNAL(activated()),currentView,SLOT(copy()));
  connect(editCut,SIGNAL(activated()),currentView,SLOT(cut()));
  connect(editPaste,SIGNAL(activated()),currentView,SLOT(paste()));
  connect(editUndo,SIGNAL(activated()),currentView,SLOT(undo()));
  connect(editRedo,SIGNAL(activated()),currentView,SLOT(redo()));
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
  if (!viewCount) slotNew();
}

void TinyKate::slotSave() {
  // feel free to make this how you want
  if (currentView==0) return;

  //  KateView *kv = (KateView*) currentView;
  KateDocument *kd = (KateDocument*) currentView->document();
  //  qDebug("saving file "+kd->docName());
  if( kd->docName().isEmpty())
    slotSaveAs();
  else
     kd->saveFile();
  //    kv->save();
  //    kd->saveFile();
}

void TinyKate::slotSaveAs() {
  if (currentView==0) return;
  KateDocument *kd = (KateDocument*) currentView->document();

  QString filename=OFileDialog::getSaveFileName(OFileSelector::EXTENDED_ALL);
  if (!filename.isEmpty()) {
    qDebug("saving file "+filename);
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
