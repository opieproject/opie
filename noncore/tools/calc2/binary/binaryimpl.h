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
#ifndef FORMBINARYINPUTIMPL
#define FORMBINARYINPUTIMPL

#include "binary.h"
#include <engine.h>

class FormBinaryImpl : public FormBinary {
Q_OBJECT
public:
    FormBinaryImpl(Engine *e,QWidget *p) : FormBinary (p,"Binary") {
	engine = e;
	engine->setRepresentation(rBin);
    };
    ~FormBinaryImpl(){};
private:
    Engine *engine;

private slots:
    void val0Clicked();
    void val1Clicked();

    void XORClicked();
    void ANDClicked();
    void NOTClicked();
    void ORClicked();

    void LSHClicked();
    void RSHClicked();
};

#endif
