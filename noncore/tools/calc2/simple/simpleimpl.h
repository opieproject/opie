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

#ifndef STANDARDIMPL_H
#define STANDARDIMPL_H

#include <qpe/qmath.h>
#include <qlcdnumber.h>

#include "simple.h"
#include "engine.h"
#include "instruction.h"

class FormSimpleImpl:public FormSimple {
Q_OBJECT
public:
  FormSimpleImpl (Engine *e, QWidget * parent = 0, const char *name = 0)
  :FormSimple (parent, name) {engine = e;engine->setRepresentation(rDouble);};

  ~FormSimpleImpl () { };

private:
    Engine *engine;

private slots:
    void MPlusClicked();
    void MCClicked();
    void MRClicked();
    void CEClicked();
    void evalClicked();
  void addClicked ();
  void decimalClicked ();
  void divClicked ();
  void mulClicked ();
  void subClicked ();
  void val0Clicked ();
  void val1Clicked ();
  void val2Clicked ();
  void val3Clicked ();
  void val4Clicked ();
  void val5Clicked ();
  void val6Clicked ();
  void val7Clicked ();
  void val8Clicked ();
  void val9Clicked ();
};

#endif
