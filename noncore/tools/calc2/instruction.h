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
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

/* Internal representation of data
The first four types indicate an int,
that is, Data.i, and are incompatible
with the other two types.

- Plugin is responsible for telling engine
which Rep to use at any given time
- Instructions from that plugin only
have to handle that representation
- Engine is responsible for error-checking
according to its current rep and display */
enum Representation {
    rBin,
    rOct,
    rDec,
    rHex,
    rDouble,
    rFraction
};

// An atom of data
union Data {
    int i;
    double dbl;
    struct Fraction {
	int numerator, denominator;
    } fraction;
};

// Instruction base class
class Instruction {
public:
  Instruction (int p = 0) {
    precedence = p;
  };

  virtual ~ Instruction () {};

  virtual Data eval(Data) = 0;
    void setRep(Representation r) { rep = r; };

  Representation rep;
  Data acc;
  int precedence;
};

#endif
