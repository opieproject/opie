/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [TEmuVt102.h]            X Terminal Emulation                              */
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
/* Ported embedded-konsole to opie-terminal                                   */
/*									      */
/* Copyright (C) 2002 by opie developers <opie@handhelds.org>                 */
/*									      */
/* -------------------------------------------------------------------------- */

#ifndef VT102EMU_H
#define VT102EMU_H

#include "widget_layer.h"
#include "screen.h"
#include "emulation_layer.h"
#include <qtimer.h>
#include <stdio.h>

//

#define MODE_AppScreen (MODES_SCREEN+0)
#define MODE_AppCuKeys (MODES_SCREEN+1)
#define MODE_AppKeyPad (MODES_SCREEN+2)
#define MODE_Mouse1000 (MODES_SCREEN+3)
#define MODE_Ansi      (MODES_SCREEN+4)
#define MODE_total     (MODES_SCREEN+5)

struct DECpar
{
  BOOL mode[MODE_total];
};

struct CharCodes
{
  // coding info
  char charset[4]; //
  int  cu_cs;      // actual charset.
  bool graphic;    // Some VT100 tricks
  bool pound  ;    // Some VT100 tricks
  bool sa_graphic; // saved graphic
  bool sa_pound;   // saved pound
};

class Vt102Emulation: public EmulationLayer
{ Q_OBJECT

public:

  Vt102Emulation(WidgetLayer* gui);
  ~Vt102Emulation();

public slots: // signals incoming from Widget
 
  void onKeyPress(QKeyEvent*);
  void onMouse(int cb, int cx, int cy);

signals:

  void changeTitle(int,const QString&);
  void prevSession();
  void nextSession();

public:

  void reset();

  /**
   * receive a char from IOLayer
   */
  void onRcvChar(int cc);
  
  /**
   * sends a list of bytes to the IOLayer
   */
  void sendString(const QByteArray&);

  /**
   * @deprecated use QByteArray instead
   * see sendString() above
   */
  void sendString(const char *);

public:
    
  BOOL getMode    (int m);

  void setMode    (int m);
  void resetMode  (int m);
  void saveMode   (int m);
  void restoreMode(int m);
  void resetModes();

  void setConnect(bool r);

private:

  void resetToken();
#define MAXPBUF 80
  void pushToToken(int cc);
  int pbuf[MAXPBUF]; //FIXME: overflow?
  int ppos;
#define MAXARGS 15
  void addDigit(int dig);
  void addArgument();
  int argv[MAXARGS];
  int argc;
  void initTokenizer();
  int tbl[256];

  void scan_buffer_report(); //FIXME: rename
  void ReportErrorToken();   //FIXME: rename

  void tau(int code, int p, int q);
  void XtermHack();

  // 

  void reportTerminalType();
  void reportStatus();
  void reportAnswerBack();
  void reportCursorPosition();
  void reportTerminalParms(int p);

protected:

  unsigned short applyCharset(unsigned short c);
  void setCharset(int n, int cs);
  void useCharset(int n);
  void setAndUseCharset(int n, int cs);
  void saveCursor();
  void restoreCursor();
  void resetCharset(int scrno);
  CharCodes charset[2];

  DECpar currParm;
  DECpar saveParm;
};

#endif // ifndef ANSIEMU_H
