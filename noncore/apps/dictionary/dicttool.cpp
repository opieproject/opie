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
#include "dicttool.h"
#include "global.h"
#include <qstringlist.h>
#include <qlineedit.h>

DictTool::DictTool( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DictToolBase( parent, name, modal, fl )
{
}

DictTool::~DictTool()
{
}

void DictTool::setDictionary(int d)
{
    dict = (DictType)d;
}

void DictTool::setPattern(const QString& s)
{
    qWarning( "DictTool::setPattern(const QString&) not yet implemented!" ); 
}

void DictTool::setPickboardSet(const QString& s)
{
    QStringList sets = QStringList::split(" ",s);
    qWarning( "DictTool::setPickboardSet(const QString&) not yet implemented!" ); 
}

void DictTool::addWord()
{
    Global::addWords(word->text());
}

void DictTool::removeWord()
{
    qWarning( "DictTool::removeWord() not yet implemented!" ); 
}

