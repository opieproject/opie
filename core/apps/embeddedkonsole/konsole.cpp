/* ---------------------------------------------------------------------- */
/*                                                                        */
/* [main.C]                        Konsole                                */
/*                                                                        */
/* ---------------------------------------------------------------------- */
/*                                                                        */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>        */
/*                                                                        */
/* This file is part of Konsole, an X terminal.                           */
/*                                                                        */
/* The material contained in here more or less directly orginates from    */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org> */
/*                                                                        */
/* ---------------------------------------------------------------------- */
/*									      */
/* Ported Konsole to Qt/Embedded                                              */
/*									      */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*									      */
/* -------------------------------------------------------------------------- */

#include <qpe/resource.h>

#include <qdir.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qobjectlist.h>
#include <qtoolbutton.h>
#include <qpe/qpetoolbar.h>
#include <qpushbutton.h>
#include <qfontdialog.h>
#include <qglobal.h>
#include <qpainter.h>
#include <qpe/qpemenubar.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qtabwidget.h>
#include <qtabbar.h>
#include <qpe/config.h>

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "konsole.h"
#include "keytrans.h"

class EKNumTabBar : public QTabBar {
public:
    void numberTabs()
    {
	// Yes, it really is this messy. QTabWidget needs functions
	// that provide acces to tabs in a sequential way.
	int m=INT_MIN;
	for (int i=0; i<count(); i++) {
	    QTab* left=0;
	    QListIterator<QTab> it(*tabList());
	    int x=INT_MAX;
	    for( QTab* t; (t=it.current()); ++it ) {
		int tx = t->rect().x();
		if ( tx<x && tx>m ) {
		    x = tx;
		    left = t;
		}
	    }
	    if ( left ) {
		left->setText(QString::number(i+1));
		m = left->rect().x();
	    }
	}
    }
};

class EKNumTabWidget : public QTabWidget {
public:
    EKNumTabWidget(QWidget* parent) : QTabWidget(parent)
    {
    }

    void addTab(QWidget* w)
    {
	QTab* t = new QTab(QString::number(tabBar()->count()+1));
	QTabWidget::addTab(w,t);
    }

    void removeTab(QWidget* w)
    {
	removePage(w);
	((EKNumTabBar*)tabBar())->numberTabs();
    }
};

// This could be configurable or dynamicly generated from the bash history
// file of the user
static const char *commonCmds[] =
{
    "ls ",
    //"ls -la ",
    "cd ",
    "pwd",
    //"cat",
    //"less ",
    //"vi ",
    //"man ",
    "echo ",
    "set ",
    //"ps",
    "ps aux",
    //"tar",
    //"tar -zxf",
    "grep ",
    //"grep -i",
    //"mkdir",
    "cp ",
    "mv ",
    "rm ",
    "rmdir ",
    //"chmod",
    //"su",
//    "top",
    //"find",
    //"make",
    //"tail",
    "cardctl eject",
    "ifconfig ",
//    "iwconfig eth0 ",
    "nc localhost 7777",
    "nc localhost 7776",
    //"mount /dev/hda1",

/*
    "gzip",
    "gunzip",
    "chgrp",
    "chown",
    "date",
    "dd",
    "df",
    "dmesg",
    "fuser",
    "hostname",
    "kill",
    "killall",
    "ln",
    "ping",
    "mount",
    "more",
    "sort",
    "touch",
    "umount",
    "mknod",
    "netstat",
*/

    "exit",
    NULL
};


Konsole::Konsole(QWidget* parent, const char* name, WFlags fl) :
    QMainWindow(parent, name, fl)
{
    QStrList args;
    init("/bin/sh",args);
}

Konsole::Konsole(const char* name, const char* _pgm, QStrList & _args, int)
 : QMainWindow(0, name)
{
    init(_pgm,_args);
}

void Konsole::init(const char* _pgm, QStrList & _args)
{
  b_scroll = TRUE; // histon;
  n_keytab = 0;
  n_render = 0;

  setCaption( tr("Terminal") );
  setIcon( Resource::loadPixmap( "konsole" ) );

  Config cfg("Konsole");
  cfg.setGroup("Konsole");

  // initialize the list of allowed fonts ///////////////////////////////////
  cfont = cfg.readNumEntry("FontID", 1);
  QFont f = QFont("Micro", 4, QFont::Normal);
  f.setFixedPitch(TRUE);
  fonts.append(new VTFont(tr("Micro"), f));

  f = QFont("Fixed", 7, QFont::Normal);
  f.setFixedPitch(TRUE);
  fonts.append(new VTFont(tr("Small Fixed"), f));

  f = QFont("Fixed", 12, QFont::Normal);
  f.setFixedPitch(TRUE);
  fonts.append(new VTFont(tr("Medium Fixed"), f));

  // create terminal emulation framework ////////////////////////////////////
  nsessions = 0;
  tab = new EKNumTabWidget(this);
  tab->setTabPosition(QTabWidget::Bottom);
  connect(tab, SIGNAL(currentChanged(QWidget*)), this, SLOT(switchSession(QWidget*)));

  // create terminal toolbar ////////////////////////////////////////////////
  setToolBarsMovable( FALSE );
  QPEToolBar *menuToolBar = new QPEToolBar( this );
  menuToolBar->setHorizontalStretchable( TRUE );

  QPEMenuBar *menuBar = new QPEMenuBar( menuToolBar );

  fontList = new QPopupMenu( this );
  for(uint i = 0; i < fonts.count(); i++) {
    VTFont *fnt = fonts.at(i);
    fontList->insertItem(fnt->getName(), i);
  }
  fontChanged(cfont);

  connect( fontList, SIGNAL( activated(int) ), this, SLOT( fontChanged(int) ));

  menuBar->insertItem( tr("Font"), fontList );

  QPEToolBar *toolbar = new QPEToolBar( this );

  QAction *a;

  // Button Commands
  a = new QAction( tr("New"), Resource::loadPixmap( "konsole" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( newSession() ) ); a->addTo( toolbar );
  a = new QAction( tr("Enter"), Resource::loadPixmap( "konsole/enter" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitEnter() ) ); a->addTo( toolbar );
  a = new QAction( tr("Space"), Resource::loadPixmap( "konsole/space" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitSpace() ) ); a->addTo( toolbar );
  a = new QAction( tr("Tab"), Resource::loadPixmap( "konsole/tab" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitTab() ) ); a->addTo( toolbar );
  a = new QAction( tr("Up"), Resource::loadPixmap( "konsole/up" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitUp() ) ); a->addTo( toolbar );
  a = new QAction( tr("Down"), Resource::loadPixmap( "konsole/down" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitDown() ) ); a->addTo( toolbar );
  a = new QAction( tr("Paste"), Resource::loadPixmap( "paste" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitPaste() ) ); a->addTo( toolbar );
/*
  a = new QAction( tr("Up"), Resource::loadPixmap( "up" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitUp() ) ); a->addTo( toolbar );
  a = new QAction( tr("Down"), Resource::loadPixmap( "down" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitDown() ) ); a->addTo( toolbar );
*/
  
  QPEToolBar *secondToolBar = new QPEToolBar( this );
  secondToolBar->setHorizontalStretchable( TRUE );

  QComboBox *commonCombo = new QComboBox( secondToolBar );
//  commonCombo->setEditable( TRUE );
  for (int i = 0; commonCmds[i] != NULL; i++)
    commonCombo->insertItem( commonCmds[i], i );
  connect( commonCombo, SIGNAL( activated(int) ), this, SLOT( enterCommand(int) ));

  // create applications /////////////////////////////////////////////////////
  setCentralWidget(tab);

  // load keymaps ////////////////////////////////////////////////////////////
  KeyTrans::loadAll();
  for (int i = 0; i < KeyTrans::count(); i++)
  { KeyTrans* s = KeyTrans::find(i);
    assert( s );
  }

  se_pgm = _pgm;
  se_args = _args;

  // read and apply default values ///////////////////////////////////////////
  resize(321, 321); // Dummy.
  QSize currentSize = size();
  if (currentSize != size())
     defaultSize = size();
}

void Konsole::show()
{
  if ( !nsessions ) {
    newSession();
  }
  QMainWindow::show();
}

void Konsole::initSession(const char*, QStrList &)
{
  QMainWindow::show();
}

Konsole::~Konsole()
{
    while (nsessions > 0) {
	    doneSession(getTe()->currentSession, 0);
    }

  Config cfg("Konsole");
  cfg.setGroup("Konsole");
  cfg.writeEntry("FontID", cfont);
}

void Konsole::fontChanged(int f)
{
    VTFont* font = fonts.at(f);
    if (font != 0) {
        for(uint i = 0; i < fonts.count(); i++) {
	    fontList->setItemChecked(i, (i == (uint) f) ? TRUE : FALSE);
    }

	cfont = f;

        TEWidget* te = getTe();
	if (te != 0) {
	    te->setVTFont(font->getFont());
	}
    }
}

void Konsole::enterCommand(int c)
{
    TEWidget* te = getTe();
    if (te != 0) {
    QString text = commonCmds[c];
    te->emitText(text);
    }
}

void Konsole::hitEnter()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->emitText(QString("\r"));
    }
}

void Konsole::hitSpace()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->emitText(QString(" "));
    }
}

void Konsole::hitTab()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->emitText(QString("\t"));
    }
}

void Konsole::hitPaste()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->pasteClipboard();
    }
}

void Konsole::hitUp()
{
    TEWidget* te = getTe();
    if (te != 0) {
    QKeyEvent ke( QKeyEvent::KeyPress, Qt::Key_Up, 0, 0);
    QApplication::sendEvent( te, &ke );
    }
}

void Konsole::hitDown()
{
    TEWidget* te = getTe();
    if (te != 0) {
    QKeyEvent ke( QKeyEvent::KeyPress, Qt::Key_Down, 0, 0);
    QApplication::sendEvent( te, &ke );
    }
}

/**
   This function calculates the size of the external widget
   needed for the internal widget to be
 */
QSize Konsole::calcSize(int columns, int lines) {
    TEWidget* te = getTe();
    if (te != 0) {
    QSize size = te->calcSize(columns, lines);
    return size;
    } else {
	QSize size;
	return size;
    }
}

/**
    sets application window to a size based on columns X lines of the te
    guest widget. Call with (0,0) for setting default size.
*/

void Konsole::setColLin(int columns, int lines)
{
  if ((columns==0) || (lines==0))
  {
    if (defaultSize.isEmpty()) // not in config file : set default value
    {
      defaultSize = calcSize(80,24);
      // notifySize(24,80); // set menu items (strange arg order !)
    }
    resize(defaultSize);
  } else {
    resize(calcSize(columns, lines));
    // notifySize(lines,columns); // set menu items (strange arg order !)
  }
}

/*
void Konsole::setFont(int fontno)
{
  QFont f;
  if (fontno == 0)
    f = defaultFont = QFont( "Helvetica", 12  );
  else
  if (fonts[fontno][0] == '-')
    f.setRawName( fonts[fontno] );
  else
  {
    f.setFamily(fonts[fontno]);
    f.setRawMode( TRUE );
  }
  if ( !f.exactMatch() && fontno != 0)
  {
    QString msg = i18n("Font `%1' not found.\nCheck README.linux.console for help.").arg(fonts[fontno]);
    QMessageBox(this,  msg);
    return;
  }
  if (se) se->setFontNo(fontno);
  te->setVTFont(f);
  n_font = fontno;
}
*/

// --| color selection |-------------------------------------------------------

void Konsole::changeColumns(int columns)
{
  TEWidget* te = getTe();
  if (te != 0) {
  setColLin(columns,te->Lines());
  te->update();
  }
}

//FIXME: If a child dies during session swap,
//       this routine might be called before
//       session swap is completed.

void Konsole::doneSession(TESession*, int )
{
  TEWidget *te = getTe();
  if (te != 0) {
    te->currentSession->setConnect(FALSE);
    tab->removeTab(te);
    delete te->currentSession;
    delete te;
    nsessions--;
  }
  
  if (nsessions == 0) {
  close();
  }
}


void Konsole::newSession() {
  TEWidget* te = new TEWidget(tab);
  te->setBackgroundMode(PaletteBase);
  te->setVTFont(fonts.at(cfont)->getFont());
  tab->addTab(te);
  TESession* se = new TESession(this, te, se_pgm, se_args, "xterm");
  te->currentSession = se;
  connect( se, SIGNAL(done(TESession*,int)), this, SLOT(doneSession(TESession*,int)) );
  se->run();
  se->setConnect(TRUE);
  se->setHistory(b_scroll);
  tab->setCurrentPage(nsessions);
  nsessions++;
}

TEWidget* Konsole::getTe() {
  if (nsessions) {
    return (TEWidget *) tab->currentPage();
  } else {
    return 0;
  }
 }
 
void Konsole::switchSession(QWidget* w) {
  TEWidget* te = (TEWidget *) w;

  QFont teFnt = te->getVTFont();
  for(uint i = 0; i < fonts.count(); i++) {
    VTFont *fnt = fonts.at(i);
    bool cf = fnt->getFont() == teFnt;
    fontList->setItemChecked(i, cf);
    if (cf) {
      cfont = i;
    }
  }
}
