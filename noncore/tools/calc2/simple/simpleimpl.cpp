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

#include <qpushbutton.h>
#include <qlcdnumber.h>

#include "simpleimpl.h"
#include <stdinstructions.h>

void FormSimpleImpl::CEClicked() {
    engine->hardReset();
}

void FormSimpleImpl::MCClicked() {
    engine->memClear();
}

void FormSimpleImpl::MRClicked() {
    engine->memRecall();
}

void FormSimpleImpl::MPlusClicked() {
    engine->memSave();
}

void FormSimpleImpl::evalClicked() {
    engine->eval();
}

void FormSimpleImpl::addClicked ()
{
  engine->pushInstruction (new iAdd ());
}

void FormSimpleImpl::subClicked ()
{
  engine->pushInstruction (new iSub ());
}

void FormSimpleImpl::mulClicked ()
{
  engine->pushInstruction (new iMul ());
}

void FormSimpleImpl::divClicked ()
{
  engine->pushInstruction (new iDiv ());
}

void FormSimpleImpl::decimalClicked ()
{
  engine->pushValue ('.');
}

void FormSimpleImpl::val1Clicked ()
{
  engine->pushValue ('1');
}

void FormSimpleImpl::val2Clicked ()
{
  engine->pushValue ('2');
}

void FormSimpleImpl::val3Clicked ()
{
  engine->pushValue ('3');
}

void FormSimpleImpl::val4Clicked ()
{
  engine->pushValue ('4');
}

void FormSimpleImpl::val5Clicked ()
{
  engine->pushValue ('5');
}

void FormSimpleImpl::val6Clicked ()
{
  engine->pushValue ('6');
}

void FormSimpleImpl::val7Clicked ()
{
  engine->pushValue ('7');
}

void FormSimpleImpl::val8Clicked ()
{
  engine->pushValue ('8');
}

void FormSimpleImpl::val9Clicked ()
{
  engine->pushValue ('9');
}

void FormSimpleImpl::val0Clicked ()
{
  engine->pushValue ('0');
}
