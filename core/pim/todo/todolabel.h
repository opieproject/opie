/**********************************************************************
** Copyright (C) 2002 by Stefan Eilers (se, eilers.stefan@epost.de)
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
#ifndef TODOLABEL_H
#define TODOLABEL_H

#include <opie/todoevent.h>

#include <qtextview.h>

class TodoLabel : public QTextView
{
    Q_OBJECT

public:
    TodoLabel( QWidget *parent, const char *name = 0 );
    ~TodoLabel();

public slots:
    void init( const ToDoEvent &item);
    void sync();

signals:
    void okPressed();

protected:
    void keyPressEvent( QKeyEvent * );
    
private:
    ToDoEvent m_item;

};

#endif
