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

#include <qapplication.h>
#include <stdlib.h>
#include <qstringlist.h>

#include <qpe/filemanager.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/ir.h>

// #include <qsocket.h>
// #include <qclipboard.h>
#include <qmultilineedit.h>
#include <qlistbox.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include <qdir.h>
#include <qfile.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtimer.h>

/* XPM */
static char * notes_xpm[] = {
"16 16 11 1",
"   c None",
".  c #000000",
"+  c #7F7F7F",
"@  c #BFBFBF",
"#  c #BFC1FF",
"$  c #FF0000",
"%  c #FFFFFF",
"&  c #00037F",
"*  c #0006FF",
"=  c #0005BF",
"-  c #7F0000",
"            ..  ",
"            ..  ",
"           ...  ",
"           .+   ",
"          .@.   ",
"        . .+    ",
"      ..#.@.    ",
"    ..###.+..   ",
"  ..###$...##.. ",
"..###$$$%+$$##&.",
".*=####$-###&=&.",
".=**=###==&=&=..",
" ..=**=#&=&=..  ",
"   ..=*=&=..    ",
"     ..=..      ",
"       .        "};


NotesControl::NotesControl( QWidget *parent, const char *name )
        : QFrame( parent, name,/* WDestructiveClose | */WStyle_StaysOnTop )
//        : QFrame( parent, name, WDestructiveClose | WStyle_StaysOnTop | WType_Popup )
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    loaded=false;
    edited=false;
    doPopulate=true;
    QVBoxLayout *vbox = new QVBoxLayout( this,0, -1, "Vlayout" );
    QHBoxLayout *hbox = new QHBoxLayout( this, 0, -1, "HLayout" );

    view = new QMultiLineEdit(this, "OpieNotesView");

    box = new QListBox(this, "OpieNotesBox");

    QPEApplication::setStylusOperation( box->viewport(),QPEApplication::RightOnHold);

    box->setFixedHeight(50);

    vbox->setMargin( 6 );
    vbox->setSpacing( 3 );

    vbox->addWidget( view);
    vbox->addWidget( box);

    setFixedHeight(180);
    QWidget *wid = QPEApplication::desktop();
    setFixedWidth( wid->width()-10 /*200*/);

    setFocusPolicy(QWidget::StrongFocus);

    newButton= new QPushButton( this, "newButton" );
    newButton->setText(tr("New"));
    hbox->addWidget( newButton);

    saveButton= new QPushButton( this, "saveButton" );
    saveButton->setText(tr("Save"));
    hbox->addWidget( saveButton);

    deleteButton= new QPushButton( this, "deleteButton" );
    deleteButton->setText(tr("Delete"));
    hbox->addWidget( deleteButton);
    
    vbox->addItem(hbox);

    connect( box, SIGNAL( mouseButtonPressed( int, QListBoxItem *, const QPoint&)),
             this,SLOT( boxPressed(int, QListBoxItem *, const QPoint&)) );

    connect(box, SIGNAL(highlighted(const QString&)), this, SLOT(slotBoxSelected(const QString &)));

    connect( &menuTimer, SIGNAL( timeout() ), SLOT( showMenu() ) );

    connect(view,SIGNAL( textChanged() ), this, SLOT(slotViewEdited() ) );

    connect(newButton, SIGNAL(clicked()), this, SLOT(slotNewButton()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSaveButton()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteButtonClicked()));

    populateBox();
    load();
    setCaption("Notes");
      //  parent->setFocus();    
}

void NotesControl::slotSaveButton() {
    slotNewButton();
    populateBox();
}

void NotesControl::slotDeleteButtonClicked() {
    switch ( QMessageBox::warning(this,tr("Delete?")
                                  ,tr("Do you really want to<BR><B> delete</B> this note ?")
                                  ,tr("Yes"),tr("No"),0,1,1) ) {
      case 0: 
          slotDeleteButton();
          break;
    };
}

void NotesControl::slotDeleteButton() {

    QString selectedText = box->currentText();
    qDebug("deleting "+selectedText);

    if( !selectedText.isEmpty()) {

        Config cfg("Notes");
        cfg.setGroup("Docs");
        int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );
        QString entryName, entryName2;;
        for ( int i = 0; i < noOfFiles; i++ ) {
            entryName.sprintf( "File%i", i + 1 );
            if(selectedText == cfg.readEntry( entryName )) {
                qDebug("removing %s, %d", selectedText.latin1(), i);
                for ( int j = i; j < noOfFiles; j++ ) {
                    entryName.sprintf( "File%i", i + 1  );
                    entryName2.sprintf( "File%i", i + 2 );
                    QString temp = cfg.readEntry(entryName2);
                    qDebug("move "+temp);
                    cfg.writeEntry(entryName, temp);
                    i++;
                }
                cfg.writeEntry("NumberOfFiles", noOfFiles-1 );
                entryName.sprintf( "File%i", noOfFiles );
                cfg.removeEntry(entryName);
                cfg.write();
                DocLnk nf(selectedText);
                nf.removeFiles();
            }
        }
        populateBox();
    }
}

void NotesControl::slotNewButton() {
    if(edited) save();
    view->clear();
    view->setFocus();    
}

void NotesControl::slotBeamButton() {
  Ir ir;
  if(!ir.supported()){
  } else {
      this->hide();
    QString selectedText = box->currentText();
    if( !selectedText.isEmpty()) {
        QString file = QDir::homeDirPath()+"/"+selectedText;
        QFile f(file);
           Ir *irFile = new Ir(this, "IR");
          connect( irFile, SIGNAL(done(Ir*)), this, SLOT( slotBeamFinished( Ir * )));
          irFile->send( file, "Note", "text/plain" );
    }
  }
}

void NotesControl::slotBeamFinished(Ir *) {
    this->show();
}

void NotesControl::boxPressed(int mouse, QListBoxItem *, const QPoint&) {
  switch (mouse) {
    case 1:{
    }
    break;
  case 2:
    menuTimer.start( 500, TRUE );
    break;
  };
}

void NotesControl::slotBoxSelected(const QString &itemString) {
    if(edited) {
        save();
    }
    loaded=false;
    edited=false;
    load(itemString);
}


void NotesControl::showMenu() {
  QPopupMenu *m = new QPopupMenu(0);

  m->insertItem(  tr( "Beam Out" ), this,  SLOT( slotBeamButton() ));
  m->insertSeparator();
  m->insertItem(  tr( "Delete" ), this,  SLOT( slotDeleteButton() ));
  m->setFocus();
  m->exec( QCursor::pos() );

  if(m) delete m;

}

void NotesControl::focusOutEvent ( QFocusEvent * e) {
    if( e->reason() == QFocusEvent::Popup)
        save();
    else {
        if(!loaded) {
        populateBox();
        load();
        }
    }
    QWidget::focusOutEvent(e);
}

void NotesControl::save() {
    Config cfg("Notes");
    cfg.setGroup("Docs");
    if( edited) {
        QString rt = view->text();
        if(!rt.isEmpty()) {
            QString pt = rt.simplifyWhiteSpace();
            int i = pt.find( ' ' );
            QString docname = pt;
            if ( i > 0 )
                docname = pt.left( i );
              // remove "." at the beginning
            while( docname.startsWith( "." ) )
                docname = docname.mid( 1 );
            docname.replace( QRegExp("/"), "_" );
              // cut the length. filenames longer than that don't make sense
              // and something goes wrong when they get too long.
            if ( docname.length() > 40 )
                docname = docname.left(40);
            if ( docname.isEmpty() )
                docname = "Empty Text";
            qDebug(docname);

            if( oldDocName != docname) {
                int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );
                QString entryName;
                entryName.sprintf( "File%i", noOfFiles + 1 );
                cfg.writeEntry( entryName,docname );
                cfg.writeEntry("NumberOfFiles", noOfFiles+1 );
                cfg.write();
            }
            else
                qDebug("oldname equals docname");
            doc = new DocLnk(docname);

            doc->setType("text/plain");
            doc->setFile(docname);
            doc->setName(docname);
    
            FileManager fm;
            if ( !fm.saveFile( *doc, rt ) ) {
            }

            oldDocName=docname;
            edited=false;
            qDebug("save");
            if (doPopulate)
                populateBox();
        }
    cfg.writeEntry( "LastDoc",oldDocName );
    cfg.write();    
        
    }
}

void NotesControl::populateBox() {
    box->clear();
    qDebug("populate");
    Config cfg("Notes");
    cfg.setGroup("Docs");
    int noOfFiles = cfg.readNumEntry("NumberOfFiles", 0 );
    QStringList list;
    QString entryName;
    for ( int i = 0; i < noOfFiles; i++ ) {
        entryName.sprintf( "File%i", i + 1 );
        list.append(cfg.readEntry( entryName ));
    }
    list.sort();
    box->insertStringList(list,-1);
    doPopulate=false;
    update();
}

void NotesControl::load() {

    if(!loaded) {
        Config cfg("Notes");
        cfg.setGroup("Docs");
        QString lastDoc=cfg.readEntry( "LastDoc","");
        DocLnk nf;
        nf.setType("text/plain");
        nf.setFile(lastDoc);

        loadDoc(nf);
        loaded=true;
        oldDocName=lastDoc;
        cfg.writeEntry( "LastDoc",oldDocName );
        cfg.write();
    }
}

void NotesControl::load(const QString & file) {
    qDebug("loading "+file);
    if(!loaded) {
        DocLnk nf;
        nf.setType("text/plain");
        nf.setFile( file);

        loadDoc(nf);
        loaded=true;
    }
//    view->setFocus();
    oldDocName=file;
    Config cfg("Notes");
    cfg.setGroup("Docs");
    cfg.writeEntry( "LastDoc",oldDocName );
    cfg.write();
}

void NotesControl::loadDoc( const DocLnk &f) {
    FileManager fm;
    QString txt;
    if ( !fm.loadFile( f, txt ) ) {
        return;
    }
    view->setText(txt);
}

void NotesControl::slotViewEdited() {
     if(loaded) {
         edited=true;
     }
}

// void NotesControl::keyReleaseEvent( QKeyEvent *e) {

//     switch ( e->state() ) {
//           case ControlButton:
//               if(e->key() == Key_C) { //copy
//                   qDebug("copy");
//     QClipboard *cb = QApplication::clipboard();
//     QString text;

//     // Copy text from the clipboard (paste)
//     text = cb->text();
//               }
//               if(e->key() == Key_X) { //cut
//               }
//               if(e->key() == Key_V) { //paste
//     QClipboard *cb = QApplication::clipboard();
//     QString text;
// //view
//     cb->setText();
//               }
//               break;
//         };
// QWidget::keyReleaseEvent(e);
// }

//===========================================================================

NotesApplet::NotesApplet( QWidget *parent, const char *name )
        : QWidget( parent, name ) {
    setFixedHeight( 18 );
    setFixedWidth( 14 );
   vc = new NotesControl;
}

NotesApplet::~NotesApplet() {
}

void NotesApplet::mousePressEvent( QMouseEvent *) {
    if( !vc->isHidden()) {
        vc->doPopulate=false;
        vc->save();
        vc->close();
    } else {
//    vc = new NotesControl;
//    QPoint curPos = mapToGlobal( rect().topLeft() );
        vc->show();
        vc->move( 2, 28);
        vc->doPopulate=true;
        vc->populateBox();
        vc->doPopulate=false;
        vc->loaded=false;

        vc->load();
//        this->setFocus();
        vc->view->setFocus();
    }
}

void NotesApplet::paintEvent( QPaintEvent* ) {
    QPainter p(this);
    p.drawPixmap( 0, 1,  ( const char** ) notes_xpm );
}

