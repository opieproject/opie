/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [emulation_layer.cpp]        Terminal Emulation Decoder                    */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole - an X terminal for KDE                       */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*									      */
/* Ported Konsole to Qt/Embedded                                              */
/*									      */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*									      */
/* -------------------------------------------------------------------------- */
/*		     							      */
/* Modified to suit opie-console                                              */
/*									      */
/* Copyright (C) 2002 by opie developers <opie@handhelds.org>                 */
/*									      */
/* -------------------------------------------------------------------------- */

/*! \class EmulationLayer

    \brief Mediator between Widget and Screen.

   This class is responsible to scan the escapes sequences of the terminal
   emulation and to map it to their corresponding semantic complements.
   Thus this module knows mainly about decoding escapes sequences and
   is a stateless device w.r.t. the semantics.

   It is also responsible to refresh the Widget by certain rules.

   \sa Widget \sa Screen

   \par A note on refreshing

   Although the modifications to the current screen image could immediately
   be propagated via `Widget' to the graphical surface, we have chosen
   another way here.

   The reason for doing so is twofold.

   First, experiments show that directly displaying the operation results
   in slowing down the overall performance of emulations. Displaying
   individual characters using X11 creates a lot of overhead.

   Second, by using the following refreshing method, the screen operations
   can be completely separated from the displaying. This greatly simplifies
   the programmer's task of coding and maintaining the screen operations,
   since one need not worry about differential modifications on the
   display affecting the operation of concern.

   We use a refreshing algorithm here that has been adoped from rxvt/kvt.

   By this, refreshing is driven by a timer, which is (re)started whenever
   a new bunch of data to be interpreted by the emulation arives at `onRcvBlock'.
   As soon as no more data arrive for `BULK_TIMEOUT' milliseconds, we trigger
   refresh. This rule suits both bulk display operation as done by curses as
   well as individual characters typed.
   (BULK_TIMEOUT < 1000 / max characters received from keyboard per second).

   Additionally, we trigger refreshing by newlines comming in to make visual
   snapshots of lists as produced by `cat', `ls' and likely programs, thereby
   producing the illusion of a permanent and immediate display operation.

   As a sort of catch-all needed for cases where none of the above
   conditions catch, the screen refresh is also triggered by a count
   of incoming bulks (`bulk_incnt').
*/

/* FIXME
   - evtl. the bulk operations could be made more transparent.
*/

#include "emulation_layer.h"
#include "widget.h"
#include "screen.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qkeycode.h>


/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                               EmulationLayer                                  */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#define CNTL(c) ((c)-'@')

/*!
*/

EmulationLayer::EmulationLayer(Widget* gui)
: decoder((QTextDecoder*)NULL)
{
  this->gui = gui;

  screen[0] = new Screen(gui->Lines(),gui->Columns());
  screen[1] = new Screen(gui->Lines(),gui->Columns());
  scr = screen[0];

  bulk_nlcnt = 0; // reset bulk newline counter
  bulk_incnt = 0; // reset bulk counter
  connected  = FALSE;

  QObject::connect(&bulk_timer, SIGNAL(timeout()), this, SLOT(showBulk()) );
  QObject::connect(gui,SIGNAL(changedImageSizeSignal(int,int)),
                   this,SLOT(onImageSizeChange(int,int)));
  QObject::connect(gui,SIGNAL(changedHistoryCursor(int)),
                   this,SLOT(onHistoryCursorChange(int)));
  QObject::connect(gui,SIGNAL(keyPressedSignal(QKeyEvent*)),
                   this,SLOT(onKeyPress(QKeyEvent*)));
  QObject::connect(gui,SIGNAL(beginSelectionSignal(const int,const int)),
		   this,SLOT(onSelectionBegin(const int,const int)) );
  QObject::connect(gui,SIGNAL(extendSelectionSignal(const int,const int)),
		   this,SLOT(onSelectionExtend(const int,const int)) );
  QObject::connect(gui,SIGNAL(endSelectionSignal(const BOOL)),
		   this,SLOT(setSelection(const BOOL)) );
  QObject::connect(gui,SIGNAL(clearSelectionSignal()),
		   this,SLOT(clearSelection()) );
}

/*!
*/

EmulationLayer::~EmulationLayer()
{
  delete screen[0];
  delete screen[1];
  bulk_timer.stop();
}

/*! change between primary and alternate screen
*/

void EmulationLayer::setScreen(int n)
{
  scr = screen[n&1];
}

void EmulationLayer::setHistory(bool on)
{
  screen[0]->setScroll(on);
  if (!connected) return;
  showBulk();
}

bool EmulationLayer::history()
{
  return screen[0]->hasScroll();
}

void EmulationLayer::setCodec(int c)
{
  //FIXME: check whether we have to free codec
  codec = c ? QTextCodec::codecForName("utf8")
            : QTextCodec::codecForLocale();
  if (decoder) delete decoder;
  decoder = codec->makeDecoder();
}

void EmulationLayer::setKeytrans(int no)
{
  keytrans = KeyTrans::find(no);
}

void EmulationLayer::setKeytrans(const char * no)
{
  keytrans = KeyTrans::find(no);
}

// Interpreting Codes ---------------------------------------------------------

/*
   This section deals with decoding the incoming character stream.
   Decoding means here, that the stream is first seperated into `tokens'
   which are then mapped to a `meaning' provided as operations by the
   `Screen' class.
*/

/*!
*/

void EmulationLayer::onRcvChar(int c)
// process application unicode input to terminal
// this is a trivial scanner
{
  c &= 0xff;
  switch (c)
  {
    case '\b'      : scr->BackSpace();                 break;
    case '\t'      : scr->Tabulate();                  break;
    case '\n'      : scr->NewLine();                   break;
    case '\r'      : scr->Return();                    break;
    case 0x07      : gui->Bell();                      break;
    default        : scr->ShowCharacter(c);            break;
  };
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                             Keyboard Handling                             */
/*                                                                           */
/* ------------------------------------------------------------------------- */

/*!
*/

void EmulationLayer::onKeyPress( QKeyEvent* ev )
{
  if (!connected) return; // someone else gets the keys
  if (scr->getHistCursor() != scr->getHistLines());
    scr->setHistCursor(scr->getHistLines());
  if (!ev->text().isEmpty())
  { // A block of text
    // Note that the text is proper unicode.
    // We should do a conversion here, but since this
    // routine will never be used, we simply emit plain ascii.
    sendString( ev->text().ascii() ); //,ev->text().length());
  }
  else if (ev->ascii()>0)
  {
    QByteArray c = QByteArray( 1 );
    c.at( 0 ) = ev->ascii();
    // ibot: qbytearray is emited not char*
    emit sndBlock( (QByteArray) c );
  }
}

// Unblocking, Byte to Unicode translation --------------------------------- --

/*
   We are doing code conversion from locale to unicode first.
*/

void EmulationLayer::onRcvBlock(const QByteArray &s )
{
  bulkStart();
  bulk_incnt += 1;
  for (int i = 0; i < s.size(); i++)
  {
    //TODO: ibot: maybe decoding qbytearray to unicode in io_layer? 
    QString result = decoder->toUnicode(&s[i],1);
    int reslen = result.length();
    for (int j = 0; j < reslen; j++)
      onRcvChar(result[j].unicode());
    if (s[i] == '\n') bulkNewline();
  }
  bulkEnd();
}

// Selection --------------------------------------------------------------- --

void EmulationLayer::onSelectionBegin(const int x, const int y) {
  if (!connected) return;
  scr->setSelBeginXY(x,y);
  showBulk();
}

void EmulationLayer::onSelectionExtend(const int x, const int y) {
  if (!connected) return;
  scr->setSelExtentXY(x,y);
  showBulk();
}

void EmulationLayer::setSelection(const BOOL preserve_line_breaks) {
  if (!connected) return;
  QString t = scr->getSelText(preserve_line_breaks);
  if (!t.isNull()) gui->setSelection(t);
}

void EmulationLayer::clearSelection() {
  if (!connected) return;
  scr->clearSelection();
  showBulk();
}

// Refreshing -------------------------------------------------------------- --

#define BULK_TIMEOUT 20

/*!
   called when \n comes in. Evtl. triggers showBulk at endBulk
*/

void EmulationLayer::bulkNewline()
{
  bulk_nlcnt += 1;
  bulk_incnt = 0;  // reset bulk counter since `nl' rule applies
}

/*!
*/

void EmulationLayer::showBulk()
{
  bulk_nlcnt = 0;                       // reset bulk newline counter
  bulk_incnt = 0;                       // reset bulk counter
  if (connected)
  {
    Character* image = scr->getCookedImage();    // get the image
    gui->setImage(image,
                  scr->getLines(),
                  scr->getColumns());     // actual refresh
    free(image);
    //FIXME: check that we do not trigger other draw event here.
    gui->setScroll(scr->getHistCursor(),scr->getHistLines());
  }
}

void EmulationLayer::bulkStart()
{
  if (bulk_timer.isActive()) bulk_timer.stop();
}

void EmulationLayer::bulkEnd()
{
  if ( bulk_nlcnt > gui->Lines() || bulk_incnt > 20 )
    showBulk();                         // resets bulk_??cnt to 0, too.
  else
    bulk_timer.start(BULK_TIMEOUT,TRUE);
}

void EmulationLayer::setConnect(bool c)
{
  connected = c;
  if ( connected)
  {
    onImageSizeChange(gui->Lines(), gui->Columns());
    showBulk();
  }
  else
  {
    scr->clearSelection();
  }
}

// ---------------------------------------------------------------------------

/*!  triggered by image size change of the Widget `gui'.

    This event is simply propagated to the attached screens
    and to the related serial line.
*/

void EmulationLayer::onImageSizeChange(int lines, int columns)
{
  if (!connected) return;
  screen[0]->resizeImage(lines,columns);
  screen[1]->resizeImage(lines,columns);
  showBulk();
  emit ImageSizeChanged(lines,columns);   // propagate event to serial line
}

void EmulationLayer::onHistoryCursorChange(int cursor)
{
  if (!connected) return;
  scr->setHistCursor(cursor);
  showBulk();
}

void EmulationLayer::setColumns(int columns)
{
  //FIXME: this goes strange ways.
  //       Can we put this straight or explain it at least?
  emit changeColumns(columns);
}
