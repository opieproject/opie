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

#include "binaryimpl.h"
#include <instruction.h>

class iXOR : public Instruction {
public:
    iXOR():Instruction(){};
    ~iXOR(){};
    Data eval(Data num) {
	Data result;
	result.i = num.i ^ acc.i;
	return result;
    };
};
class iAND : public Instruction {
public:
    iAND():Instruction(){};
    ~iAND(){};
    Data eval(Data num) {
	Data result;
	result.i = num.i & acc.i;
	return result;
    };
};
class iNOT : public Instruction {    
public:
    iNOT():Instruction(){};
    ~iNOT(){};
    Data eval(Data num) {
	Data result;
	result.i = ~ num.i;
	return result;
    };
};
class iOR : public Instruction {
public:
    iOR():Instruction(){};
    ~iOR(){};
    Data eval(Data num) {
	Data result;
	result.i = num.i | acc.i;
	return result;
    };
};
class iLSH : public Instruction {
public:
    iLSH():Instruction(){};
    ~iLSH(){};
    Data eval(Data num) {
	Data result;
	result.i = num.i << 1;
	return result;
    };
};
class iRSH : public Instruction {
public:
    iRSH():Instruction(){};
    ~iRSH(){};
    Data eval(Data num) {
	Data result;
	result.i = num.i >> 1;
	return result;
    };
};

void FormBinaryImpl::val0Clicked() {
    engine->pushValue('0');
}

void FormBinaryImpl::val1Clicked() {
    engine->pushValue('1');
}

void FormBinaryImpl::XORClicked() {
    engine->pushInstruction(new iXOR());
}
void FormBinaryImpl::ANDClicked() {
    engine->pushInstruction(new iAND());
}
void FormBinaryImpl::NOTClicked() {
    engine->immediateInstruction(new iNOT());
}
void FormBinaryImpl::ORClicked() {
    engine->pushInstruction(new iOR());
}
void FormBinaryImpl::LSHClicked() {
    engine->immediateInstruction(new iLSH());
}
void FormBinaryImpl::RSHClicked() {
    engine->immediateInstruction(new iRSH());
}
