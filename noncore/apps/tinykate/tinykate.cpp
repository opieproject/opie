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

#include "cgotoline.h"

#include "tinykate.h"

#include <katedocument.h>
#include <katehighlight.h>
#include <kateview.h>
#include <kglobal.h>

TinyKate::TinyKate( QWidget *parent, const char *name, WFlags f) :
    QMainWindow( parent, name, f )
{ 
  nextUnnamed=0;
  currentView=0;
  viewCount=0;
  setCaption("TinyKATE       : line 1      col 1");
  KGlobal::setAppName("TinyKATE");

  curLine=curCol=1;
  
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
        QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null, CTRL+Key_N, this, 0 );
        a->addTo( popup );
        connect(a, SIGNAL(activated()), this, SLOT(slotNew()));

  // Action for opening an exisiting document
        a = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ), QString::null, CTRL+Key_O, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotOpen()));


  // Action for saving  document
        a = new QAction( tr( "Save" ), Resource::loadPixmap( "save" ), QString::null, CTRL+Key_S, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotSave()));

  // Action for saving document to a new name
        a = new QAction( tr( "Save As" ), Resource::loadPixmap( "save" ), QString::null, 0, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotSaveAs()));

  // Action for closing the currently active document
        a = new QAction( tr( "Close" ), Resource::loadPixmap( "close" ), QString::null, CTRL+Key_W, this, 0 );
  a->addTo(popup);
        connect(a, SIGNAL(activated()), this, SLOT(slotClose()));
  

  mb->insertItem(tr("File"),popup);

//EDIT ACTIONS

  // Action for cutting text
        editCut = new QAction( tr( "Cut" ), Resource::loadPixmap( "cut" ), QString::null, CTRL+Key_X, this, 0 );
        editCut->addTo( bar );

  // Action for Copying text
        editCopy = new QAction( tr( "Copy" ), Resource::loadPixmap( "copy" ), QString::null, CTRL+Key_C, this, 0 );
        editCopy->addTo( bar );

  // Action for pasting text
        editPaste = new QAction( tr( "Paste" ), Resource::loadPixmap( "paste" ), QString::null, CTRL+Key_V, this, 0 );
        editPaste->addTo( bar );


  // Action for finding / replacing  text
        editFindReplace = new QAction( tr( "Replace" ), Resource::loadPixmap("find"), QString::null, CTRL+Key_R, this, 0 );
        editFind = new QAction( tr( "Find" ), Resource::loadPixmap("find"), QString::null, CTRL+Key_F, this, 0 );
        editFind->addTo( bar );

  // Action for undo
        editUndo = new QAction( tr( "Undo" ), Resource::loadPixmap( "undo" ), QString::null, CTRL+Key_Z, this, 0 );
        editUndo->addTo( bar );

  // Action for redo
        editRedo = new QAction( tr( "Redo" ), Resource::loadPixmap( "redo" ), QString::null, CTRL+SHIFT+Key_Z, this, 0 );
        editRedo->addTo( bar );


//VIEW ACITONS
        popup = new QPopupMenu( this );
  
  a = new QAction( tr( "Goto" ),  QString::null, CTRL+Key_G, this, 0 );
        a->addTo( popup );
        connect(a, SIGNAL(activated()), this, SLOT(slotGoTo()));
  
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

  popup->insertSeparator();
  editFind->addTo(popup);
  editFindReplace->addTo(popup);

  if( qApp->argc() > 1) open(qApp->argv()[1]);
  else slotNew();

}

TinyKate::~TinyKate( )
{
  qWarning("TinyKate destructor\n");
  printf("~TinyKate()\n");
  if( KGlobal::config() != 0 ) {
  printf("~TinyKate(): delete configs...\n");
    qWarning("deleting KateConfig object..\n");
    delete KGlobal::config();
  }
}

void TinyKate::slotOpen( )
{
    QString filename = OFileDialog::getOpenFileName( OFileSelector::EXTENDED_ALL,
                                                QString::null);
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
    kd->open( filename );
    viewCount++;
}

void TinyKate::setDocument(const QString &doc)
{
  printf("tinykate: setDocument(\"%s\")\n",(const char*)doc);
  open(doc);
}

void TinyKate::slotCurrentChanged( QWidget * view)
{
  if (currentView) {

    disconnect(editCopy,SIGNAL(activated()),currentView,SLOT(copy()));
    disconnect(editCut,SIGNAL(activated()),currentView,SLOT(cut()));
    disconnect(editPaste,SIGNAL(activated()),currentView,SLOT(paste()));
    disconnect(editUndo,SIGNAL(activated()),currentView,SLOT(undo()));
    disconnect(editRedo,SIGNAL(activated()),currentView,SLOT(redo()));
    disconnect(editFindReplace,SIGNAL(activated()),currentView,SLOT(replace()));
    disconnect(editFind,SIGNAL(activated()),currentView,SLOT(find()));
        disconnect(viewIncFontSizes,SIGNAL(activated()), currentView,SLOT(slotIncFontSizes()));
        disconnect(viewDecFontSizes,SIGNAL(activated()), currentView,SLOT(slotDecFontSizes()));
    disconnect(hlmenu,SIGNAL(activated(int)), currentView,SLOT(setHl(int)));
    disconnect(utilSettings,SIGNAL(activated()), currentView,SLOT(configDialog()));
    disconnect(currentView,SIGNAL(newCurPos()),this,SLOT(slotCursorMoved()));
  }

  currentView=(KTextEditor::View*)view;
  
  connect(editCopy,SIGNAL(activated()),currentView,SLOT(copy()));
  connect(editCut,SIGNAL(activated()),currentView,SLOT(cut()));
  connect(editPaste,SIGNAL(activated()),currentView,SLOT(paste()));
  connect(editUndo,SIGNAL(activated()),currentView,SLOT(undo()));
  connect(editRedo,SIGNAL(activated()),currentView,SLOT(redo()));
  connect(editFindReplace,SIGNAL(activated()),currentView,SLOT(replace()));
  connect(editFind,SIGNAL(activated()),currentView,SLOT(find()));
  connect(viewIncFontSizes,SIGNAL(activated()), currentView,SLOT(slotIncFontSizes()));
  connect(viewDecFontSizes,SIGNAL(activated()), currentView,SLOT(slotDecFontSizes()));
  connect(hlmenu,SIGNAL(activated(int)), currentView,SLOT(setHl(int)));
  connect(utilSettings,SIGNAL(activated()), currentView,SLOT(configDialog()));
  connect(currentView,SIGNAL(newCurPos()),this,SLOT(slotCursorMoved()));
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

  QString filename=OFileDialog::getSaveFileName(OFileSelector::EXTENDED_ALL,
                                                QString::null);
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

void TinyKate::slotGoTo()
{
  int l,c;
  if (currentView==0) return;
  
  currentView->getCursorPosition(&l,&c);

  CGotoLine g(l);
  
  if(g.exec()==QDialog::Accepted)
  {
    currentView->setCursorPosition( g.line(), c );
  }
}

void TinyKate::slotCursorMoved()
{
  char buf[100];
  int newLine,newCol;
  currentView->getCursorPosition(&newLine,&newCol);
  newLine++;
  newCol++;
  if((newLine!=curLine)||(newCol!=curCol))
  {
    sprintf(buf,"TinyKATE       : line %-6d col %d",newLine,newCol);
    setCaption(buf);
    curLine=newLine;
    curCol=newCol;
  }
}
