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

#ifndef STDINSTRUCTION_H
#define STDINSTRUCTION_H

#include <qpe/qmath.h>
#include "instruction.h"

// Useful instructions for plugin writers
// If you use them, take note of their precedence
class iAdd:public Instruction {
public:
  iAdd ():Instruction (10) { };
  ~iAdd () { };
  Data eval (Data num) {
    Data result;
    switch (rep) {
	case rDouble:
	    result.dbl =  acc.dbl + num.dbl;
	    break;
	default:
	    result.i = acc.i + num.i;
    };
    return result;
  };
};
class iSub:public Instruction {
public:
  iSub ():Instruction (10) { };
  ~iSub () { };
  Data eval (Data num) {
    Data result;
    switch (rep) {
	case rDouble:
	    result.dbl = acc.dbl - num.dbl;
	    break;
	default:
	    result.i = acc.i - num.i;
    };
    return result;
  };
};
class iMul:public Instruction {
public:
  iMul ():Instruction (20) { };
  ~iMul () { };
  Data eval (Data num) {
    Data result;
    switch (rep) {
	case rDouble:
	    result.dbl = acc.dbl * num.dbl;
	    break;
	default:
	    result.i = acc.i * num.i;
    };
    return result;
  };
};
class iDiv:public Instruction {
public:
  iDiv ():Instruction (20) { };
  ~iDiv () { };
  Data eval (Data num) {
    Data result;
    switch (rep) {
	case rDouble:
	    result.dbl = acc.dbl / num.dbl;
	    break;
	default:
	    result.i = acc.i / num.i;
    };
    return result;
  };
};

// Immediate double instructions only
class iSin:public Instruction {
public:
  iSin ():Instruction () { };
  ~iSin () { };
  Data eval (Data num) {
    Data result;
    result.dbl = qSin(num.dbl);
    return result;
  };
};
class iCos:public Instruction {
public:
  iCos ():Instruction () { };
  ~iCos () { };
  Data eval (Data num) {
    Data result;
    result.dbl = qCos(num.dbl);
    return result;
  };
};
class iTan:public Instruction {
public:
  iTan ():Instruction () { };
  ~iTan () {};
  Data eval (Data num) {
    Data result;
    result.dbl = qTan(num.dbl);
    return result;
  };
};
#endif
