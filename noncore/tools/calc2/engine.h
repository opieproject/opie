/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef ENGINE_H
#define ENGINE_H

#include <qwidget.h>
#include <qstack.h>	// Instruction stack
#include <qstring.h>	// Display
#include "instruction.h"

// Possible states
enum State {
  sStart,	// start inputting a new number
  sAppend,	// continue inputting a number
  sError
};

// State machine
class Engine:public QWidget {

Q_OBJECT
public:
  Engine (QWidget * parent = 0, const char *name = 0):QWidget (parent, name) {
    hardReset();
    setRepresentation(rDec);
  };

  ~Engine () { };

  void immediateInstruction (Instruction *);
  void pushInstruction (Instruction *);
  void eval ();

  void pushValue (char);
  void del ();

  void openBrace ();
  void closeBrace ();

  void softReset () {	// clears the number being inputted
    decimalPlaces = -1;
    clearData(&num);
    displayData(num);
    state = sStart;
  };
  void hardReset () {	// a "real" reset of the stack
    stack.clear ();
    memClear();
    braces = 0;
    softReset ();
  };

  void memSave () {
    mem = num;
  };
  void memRecall () {
    num = mem;
    state = sStart;
    displayData(num);
  };
  void memClear () {
    clearData(&mem);
  };

  // rFraction will require a special display enabled here
  void setRepresentation(Representation);

  // you dont want to call this
  void decBraces(void){ braces--; };

private:
  void displayData(Data d);
  void clearData(Data *d);
  int calcBase();
  Data evalStack (Data, bool);
  Data num,mem;
  State state;
  QStack < Instruction > stack;
  Representation currentRep;
  int braces, decimalPlaces; // count of finishing 0's in num
  QString displayString; // saves instatiating it over and over

signals:
  void display(const QString &);
  void display(double); // could get rid of this and
  // use a QLabel instead.
  void setHexMode();
  void setBinMode();
  void setDecMode();
  void setOctMode();
};

#endif
