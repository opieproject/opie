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
#ifndef DICTTOOL_H
#define DICTTOOL_H
#include "dicttoolbase.h"

class DictTool : public DictToolBase
{ 
    Q_OBJECT

public:
    DictTool( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DictTool();

public slots:
    void setDictionary(int);
    void setPattern(const QString&);
    void setPickboardSet(const QString&);

private slots:
    void addWord();
    void removeWord();

private:
    enum DictType { All, Local, System };
    DictType dict;
};

#endif // DICTTOOL_H
