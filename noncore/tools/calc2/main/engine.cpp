/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "engine.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qstring.h>
#include <qlcdnumber.h>

/* STD */
#include <math.h>

Data Engine::evalStack (Data num, bool inbrace = FALSE)
{
  if (state != sError) {
    Instruction *i;

// Pop the next op from the stack
    while (!stack.isEmpty () && (braces || !inbrace)) {
      i = stack.pop ();

// Check this ops prec vs next ops prec
      if (!stack.isEmpty ())
    if (i->precedence <= stack.top()->precedence)
      i->acc = evalStack (i->acc, inbrace);

// Evaluate this instruction
      num = i->eval (num);

// Error-check ( change this to work for all types )
      if (isnan (num.dbl) || isinf (num.dbl)) {
    odebug << "bad result from operation" << oendl;
    state = sError;
    clearData(&num);
    return num;
      }
    }
  }
  return num;
}

// Plugins call this to request the stack be evaluated
void Engine::eval ()
{
  num = evalStack (num);
  if (state != sError) {
    displayData(num);
    state = sStart;
  }
// if the user didnt close all their braces, its no big deal
  braces = 0;
}

void Engine::immediateInstruction (Instruction * i)
{
  if (state != sError) {
    i->setRep(currentRep);
    num = i->eval (num);
    displayData(num);
    state = sStart;
  }
}

void Engine::pushInstruction (Instruction * i)
{
  if (state != sError) {
    i->setRep(currentRep);
    i->acc = num;
    stack.push (i);
    state = sStart;
    }
}

void Engine::pushValue (char v)
{
  if (state == sAppend) {
    bool ok = FALSE;
    switch (currentRep) {
    case rDouble:
        displayString.append(v);
        num.dbl=displayString.toDouble(&ok);
        break;
    case rFraction:
        break;
    default:
        displayString.append(v);
        num.i=displayString.toInt(&ok, calcBase());
    };
    if (!ok) {
    state = sError;
    odebug << "pushValue() - num->string conversion" << oendl;
    } else {
    const QString constString = displayString;
    emit(display(constString));
    };

  } else if (state == sStart) {
    softReset();
    displayString.truncate(0);
    state = sAppend;
    pushValue (v);
  } else if (state == sError) {
    odebug << "in error state" << oendl;
    return;
  }
}

void Engine::del ()
{
    bool ok;
    switch (currentRep) {
    case rDouble:
        displayString.truncate(displayString.length());
        num.dbl=displayString.toDouble(&ok);
        break;
    case rFraction:
        odebug << "not available" << oendl;
        break;
    default:
        displayString.truncate(displayString.length());
        num.i = displayString.toInt(&ok, calcBase());
    };

    if (!ok) {
    state = sError;
    odebug << "del() - num->string conversion" << oendl;
    } else {
    const QString constString = displayString;
    emit(display(constString));
    };
}

void Engine::displayData(Data d) {
    switch (currentRep) {
    case rDouble:
        displayString.setNum(d.dbl);
        break;
    case rFraction:
        odebug << "fractional display not yet impl" << oendl;
        break;
    default:
        displayString.setNum(d.i, calcBase());
        break;
    };
    const QString constString= displayString;
    emit(display(constString));
}

// Returns the base when Rep is an integer type
int Engine::calcBase () {
    switch (currentRep) {
    case rBin:
        return 2;
    case rOct:
        return 8;
    case rDec:
        return 10;
    case rHex:
        return 16;
    default:
        state = sError;
        odebug << "Error - attempt to calc base for non-integer" << oendl;
        return 10;
    };
}

// Special instruction for internal use only
class iOpenBrace:public Instruction {
    public:
    iOpenBrace (Engine *e):Instruction (100) {engine = e;};
    ~iOpenBrace () {};

    Data eval (Data num) {
        engine->decBraces();
        return num;
    };
    private:
    Engine *engine;
};

void Engine::openBrace() {
    pushInstruction(new iOpenBrace(this));
}

void Engine::closeBrace() {
    braces++;evalStack(num,TRUE);
}

// will need to show and hide display widgets
void Engine::setRepresentation(Representation r) {
    currentRep = r;
    clearData(&num);
    clearData(&mem);
    state = sStart;
}

void Engine::clearData(Data *d) {
    d->i = d->fraction.numerator = d->fraction.denominator = 0;
    d->dbl = 0;
}

