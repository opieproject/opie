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
#ifndef INLINEEDIT_H
#define INLINEEDIT_H

#include <qlineedit.h>

class InlineEdit : public QLineEdit
{
    Q_OBJECT
    
public:
    InlineEdit( QWidget * parent = 0, const char * name = 0 );

signals:
    void lostFocus();

protected:
    void focusOutEvent( QFocusEvent * );
};

#endif
